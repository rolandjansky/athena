/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BarrelAuxFunctions.h"

#include "GeoGenericFunctions/Abs.h"
#include "GeoGenericFunctions/Sin.h"
#include "GeoGenericFunctions/Cos.h"
#include "GeoGenericFunctions/ATan.h"
#include "GeoGenericFunctions/Rectangular.h"
#include "GeoGenericFunctions/Mod.h"

#include <cfloat>

GeoGenfun::FunctionNoop LArGeo::Fx(double r, GeoGenfun::GENFUNCTION G, const double Cenx[], const double Ceny[] )
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
  GeoGenfun::GENFUNCTION result =  (Cos(G)*(Cenx[i]+Cenx[j])/2-Sin(G)*(Ceny[i]+Ceny[j])/2) ;
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::Fy(double r, GeoGenfun::GENFUNCTION G, const double Cenx[], const double Ceny[] )
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  int i = (int)rint(r-.1), j = (int)rint(r+.1) ;
  GeoGenfun::GENFUNCTION result = (Sin(G)*(Cenx[i]+Cenx[j])/2+Cos(G)*(Ceny[i]+Ceny[j])/2) ;
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::Del1(GeoGenfun::GENFUNCTION G)
{
  GeoGenfun::Cos Cos;
  GeoGenfun::Sin Sin;
  GeoGenfun::GENFUNCTION result = (Cos(  G ) * Sin( G ) );
  return GeoGenfun::FunctionNoop(&result);
}

GeoGenfun::FunctionNoop LArGeo::Del2(GeoGenfun::GENFUNCTION G)
{
  GeoGenfun::Cos Cos;
  GeoGenfun::GENFUNCTION result = (Cos(  G ) * Cos( G ) );
  return GeoGenfun::FunctionNoop(&result);
}


GeoGenfun::FunctionNoop LArGeo::ATan2(GeoGenfun::GENFUNCTION y, GeoGenfun::GENFUNCTION x)
{
  // Manufacture a Theta Function:
  GeoGenfun::Rectangular Theta;
  Theta.x0().setValue(0.0);
  Theta.x1().setValue(DBL_MAX);
  Theta.baseline().setValue(0.0);
  Theta.height().setValue(1.0);

  // Manufacture an ATan function:
  GeoGenfun::ATan ATan;
  

  // Manufacture a Mod function, putting this on the range (0-2PI)
  GeoGenfun::Mod Mod2Pi(2*M_PI);

  // Now take ATan if x is positive 
  
  GeoGenfun::GENFUNCTION result = Theta(x)*ATan(y/x) + Theta(-x)*(Mod2Pi(ATan(y/x)+M_PI)); 
  return GeoGenfun::FunctionNoop(&result);

}
