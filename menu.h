//
// Created by Tomi on 2023. 05. 28..
//

#ifndef KONFREADER_MENU_H
#define KONFREADER_MENU_H

#include <iostream>
#include <fstream>

#include "konf_hash.hpp"

using namespace std;

/**
 * getFileLocation
 * @return - the location of the configuration file
 */
string getFileLocation();

/**
 * menuSwitch |
 * basically the menu of the program, this is the user interface
 * @param konfObj - a konfHash object
 */
void menuSwitch(KonfHash konfObj);

#endif //KONFREADER_MENU_H
