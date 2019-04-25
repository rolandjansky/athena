/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TrkVKalVrtCore_TrkVKalUtils_H
#define _TrkVKalVrtCore_TrkVKalUtils_H
#include <math.h>
#include <algorithm>

namespace Trk {

  struct Vect3DF{ 
    double X; double Y; double Z;
    Vect3DF(){ X=Y=Z=0.; }
    void Set(double arr[3]){ X=arr[0]; Y=arr[1]; Z=arr[2]; }
    double Dist3D( Vect3DF & v) { return sqrt(pow(X-v.X,2)+pow(Y-v.Y,2)+pow(Z-v.Z,2)); }
  };

  struct VectMOM { double Px; double Py; double Pz; double E; };

  inline void cfdcopy(double *source, double *target, int n)
    {  std::copy(source, source + n, target); }

  inline double cfddist3D(double *V1, double *V2)
    { return sqrt( (V2[0]-V1[0])*(V2[0]-V1[0]) + (V2[1]-V1[1])*(V2[1]-V1[1]) + (V2[2]-V1[2])*(V2[2]-V1[2]) ); }


//  
//  Implementation of fortran function SIGN()
//------------------------------------------------
  inline double d_sign(double value, double sign)
    {
      if( value >= 0){
        if (sign >=0) return  value;
        if (sign < 0) return -value;
      }
      if( value < 0){
        if (sign >=0) return -value;
        if (sign < 0) return  value;
      }
      return value;
   }



} /* End namespace */
#endif
