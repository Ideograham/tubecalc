#define HANDMADE_MATH_CPP_MODE
#include "HandmadeMath.h"

struct bendTableRow
{
    hmm_vec3 Point;
    r32 Radius;
    r32 Angle;
};

struct bendTableRowVector
{
    hmm_vec3 Vector;
    r32 Magnitude;
    r32 Length;
};

struct bendTablePlane
{
    hmm_vec3 PlaneNormal;
    r32 BendAngle;
    r32 ArcLength;
    r32 PathAdjustment;
};

struct bendTableTubeRotation
{
    r32 RotationAngle;
};

struct bendTable
{
    u32 RowCount;
    r32 CenterlineLength;

    bendTableRow        *Row;
    bendTableRowVector  *Vec;
    bendTablePlane      *Plane;
    bendTableTubeRotation *TubeRotation;
};