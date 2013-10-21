/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1GeometrySystems/VP1GeomUtils.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"

//______________________________________________________________________________________
QStringList VP1GeomUtils::geoMaterialToStringList( const GeoMaterial* mat )
{
  //Fixme: Show all info.
  QStringList l;
  l << mat->getName().c_str();
  return l;
}


//______________________________________________________________________________________
QStringList VP1GeomUtils::geoElementToStringList( const GeoElement* elem )
{
  //Fixme: Show all info.
  QStringList l;
  l << elem->getName().c_str();
  return l;
}
