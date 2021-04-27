/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file BarrelAuxFunctions.h
 *
 * @brief Namespace with auxiliary functions used by BarrelConstruction and
 *        ElStraightSectionBuilder classes
 *
 **/

#ifndef LARGEOBARREL_BARRELAUXFUNCTIONS_H
#define LARGEOBARREL_BARRELAUXFUNCTIONS_H

#include "GeoGenericFunctions/FunctionNoop.h"


namespace LArGeo {

  GeoGenfun::FunctionNoop Fx(double r
			     , GeoGenfun::GENFUNCTION G
			     , const double Cenx[]
			     , const double Ceny[]);

  GeoGenfun::FunctionNoop Fy(double r
			     , GeoGenfun::GENFUNCTION G
			     , const double Cenx[]
			     , const double Ceny[]);

  GeoGenfun::FunctionNoop Del1(GeoGenfun::GENFUNCTION G);
  GeoGenfun::FunctionNoop Del2(GeoGenfun::GENFUNCTION G);

  GeoGenfun::FunctionNoop ATan2(GeoGenfun::GENFUNCTION y
				, GeoGenfun::GENFUNCTION x);

}

#endif
