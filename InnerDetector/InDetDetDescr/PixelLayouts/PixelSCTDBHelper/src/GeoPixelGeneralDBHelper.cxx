/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds the Ladder services (Omega + Al tube + glue + part of pigtail + connector)
//
//
#include "PixelSCTDBHelper/GeoPixelGeneralDBHelper.h"

#include "AthenaKernel/MsgStreamMember.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include <cmath>
#include <sstream>
#include <vector>


GeoPixelGeneralDBHelper::GeoPixelGeneralDBHelper(const PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics)
{

  switchTable_ptr = geoAccessor().getTable("PixelSwitches");
  pixelCommon_ptr = geoAccessor().getTable("PixelCommon");
  pixelBarrelGen_ptr = geoAccessor().getTable("PixelBarrelGeneral");
  pixelEndcapGen_ptr = geoAccessor().getTable("PixelEndcapGeneral");
  pixelStave_ptr = geoAccessor().getTable("PixelStave");
  pixelLayer_ptr = geoAccessor().getTable("PixelLayer");

  //    pixelEnvelope_ptr = geoAccessor().getTable("PixelEnvelope");

  m_versionName = geoAccessor().getString(switchTable_ptr,"VERSIONNAME");
  m_bIBL=false;
  m_bSLHC=false;
  if (m_versionName == "SLHC") m_bSLHC = true;
  if (m_versionName == "IBL") m_bIBL = true;

}

GeoPixelGeneralDBHelper::~GeoPixelGeneralDBHelper()
{
}



// ------------- Pixel switches ----------------------------------------------------

bool GeoPixelGeneralDBHelper::PixelBuildServices() const
{
  int tmp=geoAccessor().getInt(switchTable_ptr,"BUILDSERVICES");

  return(tmp>0);
}


int GeoPixelGeneralDBHelper::PixelStaveLayout(int layer) const
{
  if (!slhc() && !ibl()) return 0;

  int defaultLayout = 0;
  if (slhc()) defaultLayout = 1;

  int index = PixelStaveIndex(layer);

  if (!geoAccessor().testField(pixelStave_ptr,"LAYOUT",index)) return defaultLayout;
  return geoAccessor().getInt(pixelStave_ptr,"LAYOUT",index);
}


int GeoPixelGeneralDBHelper::PixelStaveIndex(int layer) const
{
  if (!slhc() && !ibl()) return 0;

   if (!geoAccessor().testField(pixelLayer_ptr,"STAVEINDEX",layer)) return 0;
  return geoAccessor().getInt(pixelLayer_ptr,"STAVEINDEX",layer);
}




double GeoPixelGeneralDBHelper::PixelRMin()  const
{
//   if (geoAccessor().getTableSize(pixelEnvelope_ptr)) {
//     double rmin = PixelEnvelopeRMin(0);  
//     for (unsigned int i = 1; i < geoAccessor().getTableSize(pixelEnvelope_ptr); i++) {
//       rmin = std::min(rmin, PixelEnvelopeRMin(i));
//     } 
//     return rmin;
//   } else {      

    return geoAccessor().getDouble(pixelCommon_ptr,"RMIN")*CLHEP::mm;
    //  }
}

double GeoPixelGeneralDBHelper::PixelRMax()  const
{
//   if (geoAccessor().getTableSize(pixelEnvelope_ptr)) {
//     double  rmax = PixelEnvelopeRMax(0);  
//     for (unsigned int i = 1; i < geoAccessor().getTableSize(pixelEnvelope_ptr); i++) {
//       rmax = std::max(rmax, PixelEnvelopeRMax(i));
//     } 
//     return rmax;
//   } else {     
    
    return geoAccessor().getDouble(pixelCommon_ptr,"RMAX")*CLHEP::mm;
    //  }
}

double GeoPixelGeneralDBHelper::PixelHalfLength()  const
{

//   if (geoAccessor().getTableSize(pixelEnvelope_ptr)) {
//     // The table should contain only +ve z values.
//     return PixelEnvelopeZ(geoAccessor().getTableSize(pixelEnvelope_ptr) - 1);
//   } else {
    return geoAccessor().getDouble(pixelCommon_ptr,"HALFLENGTH",0)*CLHEP::mm;
    //  }
}

bool GeoPixelGeneralDBHelper::PixelSimpleEnvelope()  const
{
  // Return true if the envelope can be built as a simple tube.
  // otherwise it will be built as a PCON.
  // True if size is 0 or 1.

  return (!(geoAccessor().getTableSize(pixelEnvelope_ptr) > 1));
}

unsigned int GeoPixelGeneralDBHelper::PixelEnvelopeNumPlanes()  const
{
  return geoAccessor().getTableSize(pixelEnvelope_ptr);
}

double GeoPixelGeneralDBHelper::PixelEnvelopeZ(int i)  const
{
  double zmin =  geoAccessor().getDouble(pixelEnvelope_ptr,"Z",i) * CLHEP::mm;
  if (zmin < 0) msg(MSG::ERROR) << "PixelEnvelope table should only contain +ve z values" << endreq;
  return std::abs(zmin);
}

double GeoPixelGeneralDBHelper::PixelEnvelopeRMin(int i)  const
{
  return geoAccessor().getDouble(pixelEnvelope_ptr,"RMIN",i) * CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelEnvelopeRMax(int i)  const
{
  return geoAccessor().getDouble(pixelEnvelope_ptr,"RMAX",i) * CLHEP::mm;
}


int GeoPixelGeneralDBHelper::PixelBarrelNLayer()  const
{
  return geoAccessor().getInt(pixelBarrelGen_ptr,"NLAYER");
}

// PixelBarrelGeneral
double GeoPixelGeneralDBHelper::PixelBarrelRMin()  const
{
  return geoAccessor().getDouble(pixelBarrelGen_ptr,"RMIN")*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelBarrelRMax()  const
{
  return geoAccessor().getDouble(pixelBarrelGen_ptr,"RMAX")*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelBarrelHalfLength()  const
{
  return geoAccessor().getDouble(pixelBarrelGen_ptr,"HALFLENGTH")*CLHEP::mm;
}


int GeoPixelGeneralDBHelper::PixelEndcapNDisk() const
{
  return geoAccessor().getInt(pixelEndcapGen_ptr,"NDISK");
}

// Endcap container
double  GeoPixelGeneralDBHelper::PixelEndcapRMin() const
{
  return geoAccessor().getDouble(pixelEndcapGen_ptr,"RMIN")*CLHEP::mm;
}

double  GeoPixelGeneralDBHelper::PixelEndcapRMax()  const
{
  return geoAccessor().getDouble(pixelEndcapGen_ptr,"RMAX")*CLHEP::mm;
}

double  GeoPixelGeneralDBHelper::PixelEndcapZMin()  const
{
  return geoAccessor().getDouble(pixelEndcapGen_ptr,"ZMIN")*CLHEP::mm;
}

double  GeoPixelGeneralDBHelper::PixelEndcapZMax() const
{
  return geoAccessor().getDouble(pixelEndcapGen_ptr,"ZMAX")*CLHEP::mm;
}


/*

double GeoPixelGeneralDBHelper::PixelECCarbonRMin(std::string a, int idisk) const
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

double GeoPixelGeneralDBHelper::PixelECCarbonRMax(std::string a, int idisk) const
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

// double GeoPixelGeneralDBHelper::PixelECCarbonThickness(string a, int idisk) {

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
double  GeoPixelGeneralDBHelper::PixelDiskZPosition(int idisk) const 
{
  return geoAccessor().getDouble(diskTable_ptr,"ZDISK",idisk)*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelDiskSupportRMin(int isup, int idisk) const
{
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMIN";
  return geoAccessor().getDouble(diskTable_ptr,field.str(),idisk)*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelDiskSupportRMax(int isup, int idisk) const
{
  std::ostringstream field;
  field <<"SUP"<< isup+1 <<"RMAX";
  return geoAccessor().getDouble(diskTable_ptr,field.str(),idisk)*CLHEP::mm;
}

double  GeoPixelGeneralDBHelper::PixelDiskRMin(int idisk) const
{
  return geoAccessor().getDouble(diskTable_ptr,"RIDISK",idisk)*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelECSiDz1(int idisk) const
{
  return geoAccessor().getDouble(diskTable_ptr,"DZCOUNTER",idisk)*CLHEP::mm;
}

double GeoPixelGeneralDBHelper::PixelECSiDz2(int idisk) const
{
  return PixelECSiDz1(idisk);
}

int GeoPixelGeneralDBHelper::PixelECNSectors1(int idisk) const
{
  return geoAccessor().getInt(diskTable_ptr,"NMODULE",idisk);
}

int GeoPixelGeneralDBHelper::PixelECNSectors2(int idisk) const 
{
  return PixelECNSectors1(idisk);
}
*/
