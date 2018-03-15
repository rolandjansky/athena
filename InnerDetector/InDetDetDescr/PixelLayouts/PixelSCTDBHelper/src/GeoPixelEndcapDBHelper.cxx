/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelEndcapDBHelper.h"
#include "PixelSCTDBHelper/GeoPixelHelperStaveTypes.h"
#include "PixelSCTDBHelper/GeoPixelHelperMaterialMap.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelEndcapDBHelper::GeoPixelEndcapDBHelper( PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics)
{

  diskTable_ptr = geoAccessor().getTable("PixelDisk");
  ringTable_ptr = geoAccessor().getTable("PixelRing");
  endcapGenTable_ptr = geoAccessor().getTable("PixelEndcapGeneral");

}

GeoPixelEndcapDBHelper::~GeoPixelEndcapDBHelper()
{
}


int GeoPixelEndcapDBHelper::PixelEndcapNDisk() const
{
  return geoAccessor().getInt(endcapGenTable_ptr,"NDISK");
}

// Endcap container
double  GeoPixelEndcapDBHelper::PixelEndcapRMin() const
{
  return geoAccessor().getDouble(endcapGenTable_ptr,"RMIN")*CLHEP::mm;
}

double  GeoPixelEndcapDBHelper::PixelEndcapRMax()  const
{
  return geoAccessor().getDouble(endcapGenTable_ptr,"RMAX")*CLHEP::mm;
}

double  GeoPixelEndcapDBHelper::PixelEndcapZMin()  const
{
  return geoAccessor().getDouble(endcapGenTable_ptr,"ZMIN")*CLHEP::mm;
}

double  GeoPixelEndcapDBHelper::PixelEndcapZMax() const
{
  return geoAccessor().getDouble(endcapGenTable_ptr,"ZMAX")*CLHEP::mm;
}




double GeoPixelEndcapDBHelper::PixelECCarbonRMin(std::string a, int idisk) const
{

  int isup=0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }

  return PixelDiskSupportRMin(isup,idisk);
}

double GeoPixelEndcapDBHelper::PixelECCarbonRMax(std::string a, int idisk) const
{
  int isup=0;
  if (a == "Inner") {
    isup = 0;
  } else if(a == "Central") {
    isup = 1;
  } else {
    isup = 2;
  }
  return PixelDiskSupportRMax(isup,idisk);
}

// double GeoPixelEndcapDBHelper::PixelECCarbonThickness(string a, int idisk) {

//   int isup=0;
//   if (a == "Inner") {
//     isup = 0;
//   } else if(a == "Central") {
//     isup = 1;
//   } else {
//     isup = 2;
//   }
//   return PixelDiskSupportThickness(isup,idisk);
// }

// Endcap Inner 
double  GeoPixelEndcapDBHelper::PixelDiskZPosition(int idisk) const 
{
  return geoAccessor().getDouble(diskTable_ptr,"ZDISK",idisk)*CLHEP::mm;
}

double GeoPixelEndcapDBHelper::PixelDiskSupportRMin(int isup, int idisk) const
{
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMIN";
  return geoAccessor().getDouble(diskTable_ptr,field.str(),idisk)*CLHEP::mm;
}

double GeoPixelEndcapDBHelper::PixelDiskSupportRMax(int isup, int idisk) const
{
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMAX";
  return geoAccessor().getDouble(diskTable_ptr,field.str(),idisk)*CLHEP::mm;
}

double  GeoPixelEndcapDBHelper::PixelDiskRMin(int idisk) const
{
  return geoAccessor().getDouble(diskTable_ptr,"RIDISK",idisk)*CLHEP::mm;
}

double GeoPixelEndcapDBHelper::PixelECSiDz1(int idisk) const
{
  return geoAccessor().getDouble(diskTable_ptr,"DZCOUNTER",idisk)*CLHEP::mm;
}

double GeoPixelEndcapDBHelper::PixelECSiDz2(int idisk) const
{
  return PixelECSiDz1(idisk);
}

int GeoPixelEndcapDBHelper::PixelECNSectors1(int idisk) const
{
  return geoAccessor().getInt(diskTable_ptr,"NMODULE",idisk);
}

int GeoPixelEndcapDBHelper::PixelECNSectors2(int idisk) const 
{
  return PixelECNSectors1(idisk);
}
