#pragma once

struct NeilButtons {
    int upKey;
    int downKey;
    int leftKey;
    int rightKey;
    int startKey;
    int selectKey;
    int rKey;
    int lKey;
    int aKey;
    int bKey;
    int xKey;
    int yKey;
    int axis0;
    int axis1;
    int axis2;
    int axis3;
    int touch;

    //TI-83 Keys
    int yequals;
    int window;
    int zoom;
    int trace;
    int graph;

    int second;
    int mode;
    int del;

    int alpha;
    int xt;
    int stat;

    int math;
    int apps;
    int prgm;
    int vars;
    int clear;

    int xneg1;
    int sin;
    int cos;
    int tan;
    int triangle;

    int xsquared;
    int comma;
    int parenopen;
    int parenclose;
    int divide;

    int log;
    int key7;
    int key8;
    int key9;
    int mul;

    int ln;
    int key4;
    int key5;
    int key6;
    int minus;

    int store;
    int key1;
    int key2;
    int key3;
    int plus;

    int on;
    int key0;
    int period;
    int dash;
    int enter;



};

struct NeilVirtualButton {
    int x;
    int y;
    int width;
    int height;
    bool hover;
    bool pressed;
    int* key;
    bool lastPressed;
};

extern struct NeilButtons neilbuttons;
extern struct NeilButtons lastButtons;
extern int snesWidth;
extern int snesHeight;
