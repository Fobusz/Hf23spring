#ifndef GTEST_LITE_H
#define GTEST_LITE_H

/**
 * \file gtest_lite.h
 *
 * Google gtest keretrendszerhez hasonló rendszer.
 * Sz.I. 2015., 2016., 2017. (_Has_X)
 *
 * A tesztelés legalapvetőbb funkcióit támogató függvények és makrók.
 * Nem szálbiztos megvalósítás.
 * Szabadon felhasználható, bővíthető.
 *
 * Használati példa:
 *   Teszteljük az f(x)=2*x függvényt:
 * int f(int x) { return 2*x; }
 *
 * int main() {
 *   TEST(TeszEsetNeve, TesztNeve)
 *     EXPECT_EQ(0, f(0));
 *     EXPECT_EQ(4, f(2)) << "A függvény hibás eredményt adott" << std::endl;
 *     ...
 *   END
 * ...
 *
 * A működés részleteinek megértése szorgalmi feladat.
 */

#include <iostream>
#include <cassert>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <fstream>
#ifdef MEMTRACE
#include "memtrace.h"
#endif

// Két makró az egyes tesztek elé és mögé:
// A két makró a kapcsos zárójelekkel egy új blokkot hoz létre, amiben
// a nevek lokálisak, így elkerülhető a névütközés.
/// Teszt kezdete. A makró paraméterezése hasonlít a gtest
/// paraméterezéséhez. Így az itt elkészített testek könnyen átemelhetők
/// a gtest keretrendszerbe.
/// @param C - teszteset neve (csak a gtest kompatibilitás miatt van külön neve az eseteknek)
/// @param N - teszt neve
#define TEST(C, N) { gtest_lite::test.begin(#C"."#N);

/// Teszteset vége.
#define END gtest_lite::test.end(); }

// Eredmények vizsgálatát segítő makrók.
// A paraméterek és a funkciók a gtest keretrendszerrel megegyeznek.
/// Sikeres teszt makrója
#define SUCCEED() gtest_lite::test.tstatus(true, __FILE__, __LINE__)

/// Sikertelen teszt makrója
#define FAIL() gtest_lite::test.tstatus(false, __FILE__, __LINE__)

/// Azonosságot elváró makró
#define EXPECT_EQ(expected, actual) EXPECTCMP((expected) == (actual), expected, actual)

/// Eltérést elváró makró
#define EXPECT_NE(expected, actual) EXPECTNE((expected) != (actual), expected, actual)

/// Igaz értéket elváró makró
#define EXPECT_TRUE(actual) EXPECTCMP(actual, "true", actual)

/// Hamis értéket elváró makró
#define EXPECT_FALSE(actual) EXPECTCMP(!(actual), "false", actual)

/// Valós számok azonosságát elváró makró
#define EXPECT_DOUBLE_EQ(expected, actual) EXPECTCMP(gtest_lite::almostEQ(expected, actual), expected, actual)

/// C stringek (const char *) azonosságát tesztelő makró
#define EXPECT_STREQ(expected, actual) ((actual != NULL) ? \
    EXPECTCMP(strcmp(expected, actual) == 0, expected, actual) : \
    EXPECT(false, "STR_EQ NULL pointert kapott!"))

/// C stringek (const char *) eltéréset tesztelő makró
#define EXPECT_STRNE(expected, actual) ((actual != NULL) ? \
    EXPECTNE(strcmp(expected, actual) != 0, expected, actual) : \
    EXPECT(false, "STR_EQ NULL pointert kapott!"))

/// Kivételt várunk
#define EXPECT_THROW(statement, exception_type) try { gtest_lite::test.tmp = false; statement; } \
    catch (exception_type) { gtest_lite::test.tmp = true; } \
    catch (...) { } \
    EXPECTTHROW(statement, "kivetelt dob.", "nem dobott '"#exception_type"' kivetelt.")

/// Kivételt várunk és továbbdobjuk -- ilyen nincs a gtest-ben
#define EXPECT_THROW_THROW(statement, exception_type) try { gtest_lite::test.tmp = false; statement; } \
    catch (exception_type) { gtest_lite::test.tmp = true; throw; } \
    EXPECTTHROW(statement, "kivetelt dob.", "nem dobott '"#exception_type"' kivetelt.")

/// Nem várunk kivételt
#define EXPECT_NO_THROW(statement) try { gtest_lite::test.tmp = true; statement; } \
    catch (...) { gtest_lite::test.tmp = false; }\
    EXPECTTHROW(statement, "nem dob kivetelt.", "kivetelt dobott.")

/// Segédmakró egy adattag, vagy tagfüggvény létezésének tesztelésére futási időben
/// Ötlet:
/// https://cpptalk.wordpress.com/2009/09/12/substitution-failure-is-not-an-error-2
#define CREATE_Has_(X) \
template<typename T> struct Has_##X {  \
    struct Fallback { int X; };         \
    struct Derived : T, Fallback {};    \
    template<typename C, C> struct ChT; \
    template<typename D> static char (&f(ChT<int Fallback::*, &D::X>*))[1]; \
    template<typename D> static char (&f(...))[2]; \
    static bool const member = sizeof(f<Derived>(0)) == 2; \
};

/// Segédfüggvény egy publikus adattag, vagy tagfüggvény létezésének tesztelésére fordítási időben
void hasMember(...) {}


/// -----------------------------------
/// Belső megvalósításhoz tartozó makrók, és osztályok.
/// Nem célszerű közvetlenül használni, vagy módosítani
/// -----------------------------------

/// EXPECT: makró, hogy könnyen lecserélhető legyen
#define EXPECT(expr, msg) gtest_lite::test.expect(expr, __FILE__, __LINE__, #msg)

/// EXPECTEXP: általános kifejezés kiértékelése
#define EXPECTEXP(expr, exp, act) gtest_lite::test.expect(expr, __FILE__, __LINE__, #expr) \
    << "**A(z) '"#act << "'kifejezes\n**   erteke: " << std::boolalpha << (act) \
    << "\n**   elvart: " << (exp) << std::endl

/// EXPECTCMP: összehasonlítás
#define EXPECTCMP(expr, exp, act) gtest_lite::test.expect(expr, __FILE__, __LINE__, #act) \
    << "**A(z) '"#act << "'kifejezes\n**   erteke: " << std::boolalpha << (act) \
    << "\n**   elvart: " << (exp) << std::endl

/// EXPECTNE: összehasonlítás
#define EXPECTNE(expr, exp, act) gtest_lite::test.expect(expr, __FILE__, __LINE__, #act) \
    << "**A(z) '"#act << "'kifejezes\n**   erteke: " << std::boolalpha << (act) \
    << "\n**   elvart, hogy nem: " << (exp) << std::endl

/// EXPECTTHROW: kivételkezelés
#define EXPECTTHROW(statement, exp, act) gtest_lite::test.expect(gtest_lite::test.tmp, __FILE__, __LINE__, #statement) \
    << "**Az utasitas " << (act) \
    << "\n**Azt vartuk, hogy " << (exp) << std::endl

#ifdef CPORTA
#define GTINIT(is)  \
    int magic;      \
    is >> magic;
#else
#define GTINIT(IS)
#endif // CPORTA

#ifdef CPORTA
#define GTEND(os)  \
    os << magic << (gtest_lite::test.fail() ? " NO" : " OK?") << std::endl;
#else
#define GTEND(os)
#endif // CPORTA

/// gtest_lite: a keretrendszer függvényinek és objektumainak névtere
namespace gtest_lite {

/// Tesztek állapotát tároló osztály.
/// Egyetlen egy statikus példány keletkezik, aminek a
/// destruktora a futás végén hívódik meg.
struct Test {
    int sum;            ///< tesztek számlálója
    int failed;         ///< hibás tesztek
    bool status;        ///< éppen futó teszt státusza
    bool tmp;           ///< temp a kivételkezeléshez;
    std::string name;   ///< éppen futó teszt neve
    std::fstream null;  ///< nyelő, ha nem kell kiírni semmit

    Test() :sum(0), failed(0), status(false), null("/dev/null") {}
    /// Teszt kezdete
    void begin(const char *n) {
        name = n; status = true;
#ifndef CPORTA
        std::cerr << "\n---> " << name << std::endl;
#endif // CPORTA
        ++sum;
    }
    /// Teszt vége
    void end() {
#ifdef CPORTA
        if (!status)
#endif // CPORTA
        std::cerr << (status ? "     SIKERES" : "** HIBAS ****") << "\t" << name << " <---" << std::endl;
    }

    bool fail() { return failed; }

    /// Eredményt adminisztráló tagfüggvény True a jó eset.
    std::ostream& expect(bool st, const char *file, int line, const char *expr) {
        if (!st) {
            ++failed;
            status = false;
            std::string str(file);
            size_t i = str.rfind("\\");
            if (i == std::string::npos) i = str.rfind("/");
            if (i == std::string::npos) i = 0; else i++;
            return std::cerr << "\n**** HIBA: " << &file[i] << "(" << line << "): " << expr << " ****" << std::endl;
        }
        return null;
    }

    /// Eredményt adminisztráló tagfüggvény True a jó eset, mindig ír
    std::ostream& tstatus(bool st, const char *file, int line) {
        if (!st) {
            ++failed;
            status = false;
        }
        std::string str(file);
        size_t i = str.rfind("\\");
        if (i == std::string::npos) i = str.rfind("/");
        if (i == std::string::npos) i = 0; else i++;
        return std::cerr << (status ? "** SIKERES" : "** HIBAS") << " TESZT "<< &file[i] << "(" << line << ") **" << std::endl;
     }

    /// Destruktor
    ~Test() {
#ifdef CPORTA
        if (failed)
#endif // CPORTA
            std::cerr << "\n==== TESZT VEGE ==== HIBAS/OSSZES: " << failed << "/" << sum << std::endl;
    }
};

/// Egytelen statikus példány
static Test test;

/// Segédfüggvény valós számok összehasonlításához
/// Nem bombabiztos, de nekünk most jó lesz
/// Elméleti hátér:
/// http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
inline bool almostEQ(double a, double  b) {
    // eps: ha a relatív, vagy abszolút hiba ettől kisebb, akkor elfogadjuk
    double eps = 10 * std::numeric_limits<double>::epsilon(); // 10-szer a legkisebb érték
    if (a == b) return true;
    if (fabs(a - b) < eps)
       return true;
    double aa = fabs(a);
    double ba = fabs(b);
    if (aa < ba) {
        aa = ba;
        ba = fabs(a);
    }
    return (aa - ba) < aa * eps;
}

} // namespace gtest_lite

#endif // GTEST_LITE_H
