/*********************************
Memoriaszivárgás-detektor
Készítette: Peregi Tamás, BME IIT, 2011
            petamas@iit.bme.hu
Módosítások:
Szeberényi Imre:
	kanári, 2013.
	VS 2012, 2015.,
	mem_dump, 2016.
	memset felszabadításkor, 2018.
*********************************/

#ifndef MEMTRACE_H
#define MEMTRACE_H

#if defined(MEMTRACE)

/*ha definiálva van, akkor a hibákat ebbe a fájlba írja, egyébként stderr-re*/
/*#define MEMTRACE_ERRFILE MEMTRACE.ERR*/

/*ha definiálva van, akkor futás közben láncolt listát épít. Javasolt a használata*/
#define MEMTRACE_TO_MEMORY

/*ha definiálva van, akkor futás közben fájlba írja a foglalásokat*/
/*ekkor nincs ellenorzes, csak naplozas*/
/*#define MEMTRACE_TO_FILE*/

/*ha definiálva van, akkor a megálláskor automatikus riport készül */
#define MEMTRACE_AUTO

/*ha definiálva van, akkor malloc()/calloc()/realloc()/free() követve lesz*/
#define MEMTRACE_C

#ifdef MEMTRACE_C
	/*ha definiálva van, akkor free(NULL) nem okoz hibát*/
	#define ALLOW_FREE_NULL
#endif

#ifdef __cplusplus
	/*ha definiálva van, akkor new/delete/new[]/delete[] követve lesz*/
	#define MEMTRACE_CPP
#endif

#if defined(__cplusplus) && defined(MEMTRACE_TO_MEMORY)
	/*ha definiálva van, akkor atexit helyett objektumot használ*/
	/*ajanlott bekapcsolni*/
	#define USE_ATEXIT_OBJECT
#endif

/******************************************/
/* INNEN NE MODOSITSD                     */
/******************************************/
#ifdef NO_MEMTRACE_TO_FILE
	#undef MEMTRACE_TO_FILE
#endif

#ifdef NO_MEMTRACE_TO_MEMORY
	#undef MEMTRACE_TO_MEMORY
#endif

#ifndef MEMTRACE_AUTO
    #undef USE_ATEXIT_OBJECT
#endif

#ifdef __cplusplus
	#define START_NAMESPACE namespace memtrace {
	#define END_NAMESPACE } /*namespace*/
	#define TRACEC(func) memtrace::func
	#include <new>
#else
	#define START_NAMESPACE
	#define END_NAMESPACE
	#define TRACEC(func) func
#endif

// THROW deklaráció változatai
#if defined(_MSC_VER)
  // VS rosszul kezeli az __cplusplus makrot
  #if _MSC_VER < 1900
    // * nem biztos, hogy jó így *
	#define THROW_BADALLOC
	#define THROW_NOTHING
  #else
    // C++11 vagy újabb
	#define THROW_BADALLOC noxcept(false)
	#define THROW_NOTHING noexcept
  #endif
#else
  #if __cplusplus < 201103L
	// C++2003 vagy régebbi
	#define THROW_BADALLOC throw (std::bad_alloc)
	#define THROW_NOTHING throw ()
  #else
    // C++11 vagy újabb
	#define THROW_BADALLOC noexcept(false)
	#define THROW_NOTHING noexcept
  #endif
#endif

START_NAMESPACE
	int allocated_blocks();
END_NAMESPACE

#if defined(MEMTRACE_TO_MEMORY)
START_NAMESPACE
        int mem_check(void);
END_NAMESPACE
#endif

#if defined(MEMTRACE_TO_MEMORY) && defined(USE_ATEXIT_OBJECT)
#include <cstdio>
START_NAMESPACE
	class atexit_class {
		private:
			static int counter;
			static int err;
		public:
			atexit_class() {
#if defined(CPORTA) && !defined(CPORTA_NOSETBUF)
			    if (counter == 0) {
                        setbuf(stdout, 0);
                        setbuf(stderr, 0);
			    }
#endif
			counter++;
			}

			int check() {
				if(--counter == 0)
					err = mem_check();
                return err;
			}

			~atexit_class() {
			    check();
            }
	};

static atexit_class atexit_obj;

END_NAMESPACE
#endif/*MEMTRACE_TO_MEMORY && USE_ATEXIT_OBJECT*/

/*Innentõl csak a "normál" include esetén kell */
#ifndef FROM_MEMTRACE_CPP
#include <stdlib.h>
#ifdef __cplusplus
	#include <iostream>
/* ide gyûjtjük a nemtrace-szel összeakadó headereket, hogy elõbb legyenek */

	#include <fstream>  // VS 2013 headerjében van deleted definíció
	#include <sstream>
	#include <vector>
	#include <algorithm>
#endif
#ifdef MEMTRACE_CPP
	namespace std {
		typedef void (*new_handler)();
}
#endif

#ifdef MEMTRACE_C
START_NAMESPACE
	#undef malloc
	#define malloc(size) TRACEC(traced_malloc)(size,#size,__LINE__,__FILE__)
	void * traced_malloc(size_t size, const char *size_txt, int line, const char * file);

	#undef calloc
	#define calloc(count,size) TRACEC(traced_calloc)(count, size, #count","#size,__LINE__,__FILE__)
	void * traced_calloc(size_t count, size_t size, const char *size_txt, int line, const char * file);

	#undef free
	#define free(p) TRACEC(traced_free)(p, #p,__LINE__,__FILE__)
	void traced_free(void * p, const char *size_txt, int line, const char * file);

	#undef realloc
	#define realloc(old,size) TRACEC(traced_realloc)(old,size,#size,__LINE__,__FILE__)
	void * traced_realloc(void * old, size_t size, const char *size_txt, int line, const char * file);

	void mem_dump(void const *mem, size_t size, FILE* fp);


END_NAMESPACE
#endif/*MEMTRACE_C*/

#ifdef MEMTRACE_CPP
START_NAMESPACE
	#undef set_new_handler
	#define set_new_handler(f) TRACEC(_set_new_handler)(f)
	void _set_new_handler(std::new_handler h);

	void set_delete_call(int line, const char * file);
END_NAMESPACE

void * operator new(size_t size, int line, const char * file) THROW_BADALLOC;
void * operator new[](size_t size, int line, const char * file) THROW_BADALLOC;
void * operator new(size_t size) THROW_BADALLOC;
void * operator new[](size_t size) THROW_BADALLOC;
void operator delete(void * p)  THROW_NOTHING;
void operator delete[](void * p) THROW_NOTHING;

/* Visual C++ 2012 miatt kell, mert háklis, hogy nincs megfelelõ delete, bár senki sem használja */
void operator delete(void *p, int, const char *) THROW_NOTHING;
void operator delete[](void *p, int, const char *) THROW_NOTHING;


#define new new(__LINE__, __FILE__)
#define delete memtrace::set_delete_call(__LINE__, __FILE__),delete

#ifdef CPORTA
//#define system(...)  // system(__VA_ARGS__)
#endif

#endif /*MEMTRACE_CPP*/

#endif /*FROM_MEMTRACE_CPP*/
#endif /*MEMCHECK*/
#endif /*MEMTRACE_H*/
