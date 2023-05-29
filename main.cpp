#include "menu.h"

using namespace std;

int main()
{
    KonfHash k1;
    k1.readFile(getFileLocation());
    menuSwitch(k1);

    return 0;
}