#ifndef __STELLAR_OBJECT_H
#define __STELLAR_OBJECT_H

// Struct to represent a stellar object axis
struct Axis
{
public:
    float hours;
    float mins;
    float secs;

    float toDecimal();
};

struct Coords
{
public:
    Axis RA;
    Axis Dec;

};

class StellarObject
{
public:
    StellarObject(float ra_h, float ra_m, float ra_s,
                  float dec_h, float dec_m, float dec_s);

    bool setRA(float hour, float min, float sec);
    bool setDec(float hour, float min, float sec);
    char* getName() { return _name; }
    Coords getCoords() { return _coords; }

private:
    char _name[32];
    Coords _coords;
};

#endif // __STELLAR_OBJECT_H


