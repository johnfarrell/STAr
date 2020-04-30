/*
 * ScrollingMenu
 * 
 * Author: John Farrell
 * 
 * ScrollingMenu is designed to be a simple to use library
 * for creating interactive menus on LCD screens for arduino.
 */

#ifndef __SCROLLING_MENU_H
#define __SCROLLING_MENU_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <LiquidCrystal.h>;

class MenuItem
{
public:
    MenuItem(const char *name);

    void addSubmenu(MenuItem &menu);
    void removeSubmenu(const char *name);
    void removeSubmenu(int index);

private:
    MenuItem *_children;

    char *_name;
    int _numChildren;
};

class ScrollingMenu
{
public:
    ScrollingMenu() {};
    ScrollingMenu(LiquidCrystal &lcd);

    MenuItem* 

private:
    MenuItem *_root;
}


#endif /* __SCROLLING_MENU_H */