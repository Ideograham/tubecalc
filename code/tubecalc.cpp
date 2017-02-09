#include <stdlib.h>
#include <stdio.h>
#include "ig_base.h"
#include "BendTable.h"

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_CPP_MODE
#include "HandmadeMath.h"

internal void
printVec3(hmm_vec3 v3, b32 newline = TRUE)
{
    printf("X:%.3f, Y:%.3f, Z:%.3f", v3.X, v3.Y, v3.Z);
    if (newline)
        printf("\n");
}


inline r32 
calcBendAngle(hmm_vec3 a, hmm_vec3 b)
{
    r32 Result = HMM_ACosF(HMM_Dot(a, b) / (HMM_Length(a) * HMM_Length(b)));

    return(Result);
}

inline r32
ToDegrees(float Radians)
{
    float Result = 0.0f;

    Result = Radians * (180.0f / HMM_PI32);
    return (Result);
}


internal void
flareDemo()
{
   hmm_vec3 flare[3] =
        {   {-7.104f,  -8.021f,   -11.599f},
            {-4.571f,  -8.021f,   -10.136f},
            {-3.627f,  -6.365f,   -8.559f} 
        };
    
    hmm_vec3 diff[2];
    diff[0] = flare[1] - flare[0];
    diff[1] = flare[2] - flare[1];

    printVec3(diff[0]);
    printVec3(diff[1]);

    r32 dot = HMM_Dot(diff[0], diff[1]);

    r32 theta = HMM_ACosF(dot / (HMM_Length(diff[0]) * HMM_Length(diff[1])));

    hmm_vec3 plane = HMM_Normalize(HMM_Cross(diff[0], diff[1]));

    printf("v1: %f; v2: %f; angle: %f\n", 
        HMM_Length(diff[0]), 
        HMM_Length(diff[1]), 
        ToDegrees(calcBendAngle(diff[0], diff[1])));

    printVec3(plane);
}

internal void
fillBendTable(bendTable *BendTable, hmm_vec4 *Rows, u32 Count)
{
    //printf("Count is %d\n", Count);
    Assert(BendTable);
    Assert(!BendTable->RowCount);

    //TODO(ig): allocate this with an arena
    BendTable->Row = (bendTableRow*)malloc(sizeof(bendTableRow) * (size_t)Count);
    BendTable->Vec = (bendTableRowVector*)malloc(sizeof(bendTableRowVector) * (size_t)(Count - 1));
    BendTable->Plane = (bendTablePlane*)malloc(sizeof(bendTablePlane) * (size_t)(Count - 2));
    if (Count > 3)
    {
        BendTable->TubeRotation = (bendTableTubeRotation*)malloc(sizeof(bendTableTubeRotation) * (size_t)(Count - 3));
    }

    //Copy Rows
    for (u32 iter = 0; iter < Count; iter++)
    {
        BendTable->Row[iter].Point = Rows[iter].XYZ;
        BendTable->Row[iter].Radius = Rows[iter].W;
        BendTable->Row[iter].Angle = 0.0f;
        BendTable->RowCount++;
    }
    Assert(Count == BendTable->RowCount);

    //Calculate Vectors
    for (u32 iter = 0; iter < (Count - 1); iter++)
    {
        BendTable->Vec[iter].Vector = BendTable->Row[iter+1].Point - BendTable->Row[iter].Point;
        BendTable->Vec[iter].Magnitude = HMM_Length(BendTable->Vec[iter].Vector);
        BendTable->Vec[iter].Length = 0.0f;
    }

    //Calculate Planes, Angles
    for (u32 iter = 0; iter < (Count - 2); iter++)
    {
        BendTable->Plane[iter].PlaneNormal = HMM_Normalize(HMM_Cross(BendTable->Vec[iter].Vector, BendTable->Vec[iter+1].Vector));
        BendTable->Plane[iter].BendAngle = calcBendAngle(BendTable->Vec[iter].Vector, BendTable->Vec[iter+1].Vector);
        BendTable->Plane[iter].ArcLength = BendTable->Plane[iter].BendAngle * BendTable->Row[iter+1].Radius;
        BendTable->Plane[iter].PathAdjustment = BendTable->Row[iter+1].Radius * HMM_TanF(BendTable->Plane[iter].BendAngle / 2.0f);
        //printf("PathAdjustment: %f %d \n", BendTable->Plane[iter].PathAdjustment, iter);
    }


    //Calculate Tube Roation Angles
    for (u32 iter = 0; iter < (Count - 3); iter++)
    {
        BendTable->TubeRotation[iter].RotationAngle = 
            HMM_ACosF(
                HMM_Dot(BendTable->Plane[iter].PlaneNormal, 
                        BendTable->Plane[iter+1].PlaneNormal));
    }

    //Calculate Side Lengths
    BendTable->CenterlineLength = 0.0f;

    r32 prevAdj = 0.0f;
    for (u32 iter = 0; iter < (Count - 1); iter++)
    {
        r32 postAdj = 0.0f;
        if (iter < (Count - 2))
        {
            postAdj = BendTable->Plane[iter].PathAdjustment;
        }
        
        r32 Len =         
                BendTable->Vec[iter].Magnitude - prevAdj - postAdj;

        BendTable->Vec[iter].Length = Len;

        //printf("__prev %f, post %f, len %f\n", prevAdj, postAdj, Len);
        prevAdj = postAdj;

        BendTable->CenterlineLength += Len;
        if (iter < (Count - 2))
            BendTable->CenterlineLength += BendTable->Plane[iter].ArcLength;
    }
}

internal void
printBendTable(bendTable *BendTable)
{
    printf("Bend Table Calculator\n");
    printf("%d Rows, %f Total Length\n", BendTable->RowCount, BendTable->CenterlineLength);

    for (u32 r=0; r<BendTable->RowCount; r++)
    {
        printVec3(BendTable->Row[r].Point, FALSE);
        printf(" R: %.1f A: %.2f ", BendTable->Row[r].Radius, ToDegrees(BendTable->Row[r].Angle));
        if (r > 0)
        {
            printVec3(BendTable->Vec[r-1].Vector, FALSE);
            printf(" LEN: %.2f ", BendTable->Vec[r-1].Length);
            if (r > 1)
            {
                printVec3(BendTable->Plane[r-2].PlaneNormal, FALSE);
                printf(" Ang: %.2f Arc: %.2f Adj: %.2f", ToDegrees(BendTable->Plane[r-2].BendAngle), BendTable->Plane[r-2].ArcLength, BendTable->Plane[r-2].PathAdjustment);
                
                if (r > 2)
                {
                    printf(" TubeRot: %f ", ToDegrees(BendTable->TubeRotation[r-3].RotationAngle));
                }            
            }
        }
        printf("\n");
    }
}

int
main(int argc, char **args)
{

    bendTable BendTable = {};

    hmm_vec4 table_rows[] =
        {   {0.0f,  0.0f,   0.0f,   0.0f},
            {0.0f,  0.0f,   2.93f,  3.50f},
            {8.02f, 4.33f,  10.70f, 3.50f},
            {8.02f, 6.89f,  12.18f, 0.0f}
        };

    fillBendTable(&BendTable, table_rows, ArrayCount(table_rows));
    printBendTable(&BendTable);

    return 0;
}