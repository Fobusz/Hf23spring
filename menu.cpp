//
// Created by Tomi on 2023. 05. 28..
//


#include <algorithm>
#include "menu.h"


using namespace std;


/**
 * source: https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case ||
 * lower |
 * @param str - a string with uppercase and lowercase letters
 * @return a string with all letters transformed to lowercase
 */
static string lower(string str)
{

    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return str;
}

string getFileLocation()
{
    bool exist = false;

    string fileLocation;

    while (!exist)
    {
        cout << "Please provide a path to the configuration file!" << endl;
        cin >> fileLocation;

        fstream file(fileLocation);
        exist = file.good();
    }

    return fileLocation;
}

void menuSwitch(KonfHash konfObj)
{

    string numb = "0";

    while (numb != "7")
    {
        cout << "\nPlease choose one of the options below by type in the number before the option!\n"
             << "1 - get back a value\n"
             << "2 - add a new key-value pair\n"
             << "3 - update an existing key-value pair\n"
             << "4 - delete an existing key-value pair\n"
             << "5 - read in another file\n"
             << "6 - save all key-value pair into a new file\n"
             << "7 - exit the program" << endl;

        cin >> numb;

        if (numb == "1")
        {
            cout << "Please enter a key!" << endl;

            string key;
            cin >> key;

            cout << "Please enter the type of the value!" << endl;

            string type;
            cin >> type;

            bool ok = false;

            while (!ok)
            {
                if (lower(type) == "string")
                {
                    cout << konfObj.get<string>(key) << endl;
                    ok = true;
                } else if (lower(type) == "int")
                {
                    cout << konfObj.get<int>(key) << endl;
                    ok = true;
                } else if (lower(type) == "double")
                {
                    cout << konfObj.get<double>(key) << endl;
                    ok = true;
                } else if (lower(type) == "bool")
                {
                    cout << konfObj.get<bool>(key) << endl;
                    ok = true;
                } else
                {
                    cout << "Please enter the type of the value!" << endl;
                    cin >> type;
                }
            }
        }
        if (numb == "2")
        {
            cout << "Please enter a key!" << endl;

            string key;
            cin >> key;

            cout << "Please enter a value!" << endl;

            string value;
            cin >> value;

            konfObj.post(make_pair(key, value));

        }
        if (numb == "3")
        {
            cout << "Please enter a key!" << endl;

            string key;
            cin >> key;

            cout << "Please enter a value!" << endl;

            string value;
            cin >> value;

            konfObj.put(make_pair(key, value));

        }
        else if (numb == "4")
        {
            cout << "Please enter a key!" << endl;

            string key;
            cin >> key;

            konfObj.del(key);
        }
        else if (numb == "5")
        {
            string location = getFileLocation();
            konfObj.readFile(location);
        }
        else if (numb == "6")
        {
            cout << "Please enter a name for the new file!" << endl;

            string fileName;
            cin >> fileName;
            konfObj.saveToFile(fileName);
        }
        else if (numb == "7")
        {
            cout << "So Long, and Thanks for All the Fish" << endl;
        }
    }
}
