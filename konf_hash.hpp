#ifndef KONFREADER_KONF_HASH_H
#define KONFREADER_KONF_HASH_H

#include <string>
#include <iostream>
#include <vector>
#include <valarray>
#include <sstream>
#include <array>
#include <fstream>
#include <ostream>

using namespace std;

class KonfHash
{
private:
    /**
     * the basis of the hash table
     */
    array<vector<pair<string, string>>, 256> table;

    /**
     * getValue
     * @param key - the key that belongs to the wanted value
     * @return the value that belongs to the key, in string
     */
    string getValue(const string &key);

    /**
     * hashFunction
     * @param keyInString - key from input
     * @return hashed key
     */
    static inline int hashFunction(const string &keyInString)
    {
        size_t sizeS = keyInString.size();
        return (keyInString[0] + keyInString[1] + keyInString[sizeS - 1] + keyInString[sizeS - 2]) % 256;
    }

    /**
     * toType |
     * get a value, convert it from string to its right type and return it
     * @tparam U - the wanted type
     * @param value - the value we want to convert
     * @return the value converted to the specific type
     * @throw runtime_error - if there is a type mismatch
     */
    template<typename U>
    U toType(const string &value);

    /**
     * getpos |
     * get a key and returns its position in the array
     * @param key - a key of a key-value pair
     * @return the position in int
     * @throw runtime_error - if the key isn't a member of the table
     */
    int getPos(const string &key);

    /**
     * isMember
     * @param key
     * @return true, if the key is already member of the table
     */
    bool isMember(const string &key);

public:

    KonfHash() = default;
    KonfHash(const KonfHash &) = default;

    /**
     * post |
     * get the hash of the inputted key and post key-value pair to the table
     * @param element - a key-value pair
     */
    void post(const pair<string, string>& element);

    /**
     * get |
     * returns the value of a key-value pair in the proper type
     * @param key - the key in string of the sought value
     * @return type specific value of the key
     */
    template<typename T>
    T get(const string &key);

    /**
     * put |
     * update an existing key-value pairs value
     * @param newElement - key-value pair
     */
    void put(const pair<string, string>& newElement);

    /**
     * del |
     * deletes a key-value pair from the table
     * @param key - String key of the value to delete
     */
    void del(const string &key);

    /**
     * readFile |
     * reads a configuration file and post the key-value pairs to the table
     * @param fileLocation - the exact location of the configuration file
     */
    void readFile(const string &fileLocation);

    /**
     * saveToFile |
     * create a file, then write all the elements of the table in the file, in the right format
     * @param name - name of the new save file
     */
    void saveToFile(const string &name);


    ~KonfHash();

};

//**********************************************************************************************************************


inline void KonfHash::post(const pair<string, string>& element)
{
    if (!isMember(element.first))
    {
        int hashValue = hashFunction(element.first);
        table[hashValue].push_back(element);
    }
    else
    {
        cerr << "The key is already used for another value! --> " << element.first << endl;
    }


}


inline void KonfHash::put(const pair<string, string>& newElement)
{
    int hashValue = hashFunction(newElement.first);

    try
    {
        auto index = getPos(newElement.first);
        table[hashValue][index] = newElement;
    }
    catch (runtime_error& except)
    {
        cerr << "There is no such element!" << endl;
        return;
    }
}


inline void KonfHash::del(const string &key)
{
    int hashValue = hashFunction(key);

    try
    {
        auto index = getPos(key);
        table[hashValue].erase(table[hashValue].begin() + index);
    }
    catch (runtime_error& except)
    {
        cerr << "There is no such element!" << endl;
        return;
    }
}


inline string KonfHash::getValue(const string &key)
{
    int hashedKey = hashFunction(key);

    try
    {
        auto index = getPos(key);
        return table[hashedKey][index].second;
    }
    catch (runtime_error& except)
    {
        return "";
    }
}

template<typename U>
inline U KonfHash::toType(const string &value)
{
    U convertedValue;

    stringstream stream(value);
    stream >> convertedValue;

    if (stream.fail())
    {
        runtime_error e(value);
        throw e;
    }
    return convertedValue;
}

template<typename T>
inline T KonfHash::get(const string &key)
{
    return getValue(key);
}

template<>
inline string KonfHash::get(const string &key)
{
    return getValue(key);
}

template<>
inline bool KonfHash::get(const string &key)
{
    return getValue(key) == "true";
}

template<>
inline int KonfHash::get(const string &key)
{
    try
    {
        auto value = toType<int>(getValue(key));
        return value;
    }
    catch (runtime_error& except)
    {
        cerr << "Type mismatch or missing value!" << endl;
        return -1;
    }

}

template<>
inline double KonfHash::get(const string &key)
{
    try
    {
        auto value = toType<double>(getValue(key));
        return value;
    }
    catch (runtime_error& except)
    {
        cerr << "Type mismatch" << endl;
        return -1.0;
    }
}

inline void KonfHash::readFile(const string &fileLocation)
{
    fstream file;
    string line;

    file.open(fileLocation, ios::in);

    pair<string, string> tempP;
    string t1, t2;

    while (getline(file, line))
    {
        if (line[0] != '#')
        {
            t1 = line.substr(0, line.find('='));
            t2 = line.substr(line.find('=') + 1);
            tempP = make_pair(t1, t2);
            post(tempP);
        }
    }

    file.close();
}


inline int KonfHash::getPos(const string &key)
{
    int hashedKey = hashFunction(key);
    int pos = 0;

    for (auto &i : table[hashedKey])
    {
        if (i.first == key)
        {
            return pos;
        }
        pos++;
    }

    runtime_error except(key);
    throw except;
}

inline bool KonfHash::isMember(const string &key)
{
    try
    {
        getPos(key);
        return true;
    }
    catch (runtime_error& except)
    {
        return false;
    }
}

inline void KonfHash::saveToFile(const string &name)
{
    string fileName = "../" + name + ".txt";

    ofstream saveFile;

    saveFile.open(fileName);

    for (auto &i : table)
    {
        for (auto &j : i)
        {
            saveFile << j.first << "=" << j.second << endl;
        }
    }
}

inline KonfHash::~KonfHash() = default;

#endif //KONFREADER_KONF_HASH_H
