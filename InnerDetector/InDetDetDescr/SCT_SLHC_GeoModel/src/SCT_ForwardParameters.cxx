/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"


#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>
#include <cmath>

namespace InDetDDSLHC {

SCT_ForwardParameters::SCT_ForwardParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    SctFwdGeneral(),
    SctFwdWheel(),
    SctFwdRing(),
    SctFwdWheelRingMap(),
    SctFwdDiscSupport()
{
  if (sctdb) { // Will be zero for old text file format.
    SctFwdGeneral      = sctdb->fwdGeneral();
    SctFwdWheel        = sctdb->fwdWheel();
    SctFwdRing         = sctdb->fwdRing();
    SctFwdWheelRingMap = sctdb->fwdWheelRingMap();
    SctFwdDiscSupport  = sctdb->fwdDiscSupport();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Forward Parameters =======" 
					    << endreq;
    
    // Fill wheel/ring map
    for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(SctFwdWheelRingMap); ++indexTmp) {
      int wheelTmp = db()->getInt(SctFwdWheelRingMap,"WHEELNUM",indexTmp);
      int ringTmp = db()->getInt(SctFwdWheelRingMap,"RINGNUM",indexTmp);
      m_wheelRingIndexMap.add(wheelTmp,ringTmp,indexTmp);
    }
  }
}


SCT_ForwardParameters::~SCT_ForwardParameters()
{
}

int 
SCT_ForwardParameters::layoutType() const
{
  return db()->getInt(SctFwdGeneral,"LAYOUTTYPE");
}

//
// Forward Wheel
//
double SCT_ForwardParameters::materialIncreaseFactor(int iWheel) const
{
  return db()->getDouble(SctFwdDiscSupport,"MATSCALEFACTOR",iWheel);
}

int
SCT_ForwardParameters::fwdNumWheels() const
{
  return db()->getInt(SctFwdGeneral,"NUMWHEELS");
}


double
SCT_ForwardParameters::fwdWheelZPosition(int iWheel) const
{
  return db()->getDouble(SctFwdWheel,"ZPOSITION",iWheel) * CLHEP::mm;
}

int
SCT_ForwardParameters::fwdWheelNumRings(int iWheel) const
{
  return db()->getInt(SctFwdWheel,"NUMRINGS",iWheel);
}

int 
SCT_ForwardParameters::fwdWheelRingType(int iWheel, int iRingIndex) const
{
  // get the row number in the SctFwdWheelRingMap table
  int index = getRingMapIndex(iWheel,iRingIndex);
  if (index >= 0) {
    return db()->getInt(SctFwdWheelRingMap,"RINGTYPE",index);
  } else {
    return 0;
  }
}

int
SCT_ForwardParameters::getRingMapIndex(int iWheel, int iRingIndex) const
{
  int index =  m_wheelRingIndexMap.find(iWheel, iRingIndex);
  if (index < 0)  msg(MSG::ERROR) << "Index not found for wheel,ring =  " << iWheel << ", " << iRingIndex << endreq;
  return index;
}

//
// Forward DiscSupport
//
double
SCT_ForwardParameters::fwdDiscSupportInnerRadius(int iWheel) const
{
  return db()->getDouble(SctFwdDiscSupport,"INNERRADIUS",iWheel) * CLHEP::mm;
} 

double
SCT_ForwardParameters::fwdDiscSupportOuterRadius(int iWheel) const
{
  return db()->getDouble(SctFwdDiscSupport,"OUTERRADIUS",iWheel) * CLHEP::mm;
}
 
double
SCT_ForwardParameters::fwdDiscSupportThickness(int iWheel) const
{
  return db()->getDouble(SctFwdDiscSupport,"THICKNESS",iWheel) * CLHEP::mm;
} 

std::string
SCT_ForwardParameters::fwdDiscSupportMaterial(int iWheel) const
{
  return db()->getString(SctFwdDiscSupport,"MATERIAL",iWheel);
}

//
// Forward Ring
//

int 
SCT_ForwardParameters::fwdRingNumModules(int iRingType) const
{
  return db()->getInt(SctFwdRing,"NUMMODULES",iRingType);
}


double
SCT_ForwardParameters::fwdRingInnerRadius(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"INNERRADIUS",iRingType) * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdRingMiddleRadius(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"MIDDLERADIUS",iRingType) * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdRingOuterRadius(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"OUTERRADIUS",iRingType) * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdRingOffset(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"OFFSET",iRingType) * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdRingModuleStagger(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"MODULESTAGGER",iRingType) * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdRingPhiOfRefModule(int iRingType) const
{
  return db()->getDouble(SctFwdRing,"PHIOFREFMODULE",iRingType) * CLHEP::radian;
}

int
SCT_ForwardParameters::fwdRingStaggerOfRefModule(int iRingType) const
{
  return db()->getInt(SctFwdRing,"STAGGEROFREFMODULE",iRingType);
}

int
SCT_ForwardParameters::fwdWheelDoubleSided(int iRingType) const
{
  return db()->getInt(SctFwdRing,"DOUBLESIDED",iRingType);
}

// Returns +/-1 
/*
int
SCT_ForwardParameters::fwdWheelStereoType(m_iWheel, int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_STEREOTYPE", iRing);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
  //--- return m_rdb->fwdWheel(iWheel)->getInt("STEREOTYPE");
}
*/
//
// Forward General
//

double
SCT_ForwardParameters::fwdInnerRadius() const
{
  return db()->getDouble(SctFwdGeneral,"INNERRADIUS") * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdIntermediateRadius() const
{
  return db()->getDouble(SctFwdGeneral,"RINTERMEDIATE") * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdOuterRadius() const
{
  return db()->getDouble(SctFwdGeneral,"OUTERRADIUS") * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdZMin() const
{
  return db()->getDouble(SctFwdGeneral,"ZMIN") * CLHEP::mm;
} 

double
SCT_ForwardParameters::fwdZIntermediate() const
{
  return db()->getDouble(SctFwdGeneral,"ZINTERMEDIATE") * CLHEP::mm;
}

double
SCT_ForwardParameters::fwdZMax() const
{
  return db()->getDouble(SctFwdGeneral,"ZMAX") * CLHEP::mm;
}

}
