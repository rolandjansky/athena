/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardParameters.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>
#include <cmath>

namespace InDetDDSLHC {

SCT_ForwardParameters::SCT_ForwardParameters(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ParametersBase(athenaComps),
    m_SctFwdGeneral(),
    m_SctFwdWheel(),
    m_SctFwdRing(),
    m_SctFwdWheelRingMap(),
    m_SctFwdDiscSupport()
{
  if (sctdb) { // Will be zero for old text file format.
    m_SctFwdGeneral      = sctdb->fwdGeneral();
    m_SctFwdWheel        = sctdb->fwdWheel();
    m_SctFwdRing         = sctdb->fwdRing();
    m_SctFwdWheelRingMap = sctdb->fwdWheelRingMap();
    m_SctFwdDiscSupport  = sctdb->fwdDiscSupport();
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"========== Initialize Database Forward Parameters =======" 
					    << endmsg;
    
    // Fill wheel/ring map
    for (unsigned int indexTmp = 0; indexTmp < db()->getTableSize(m_SctFwdWheelRingMap); ++indexTmp) {
      int wheelTmp = db()->getInt(m_SctFwdWheelRingMap,"WHEELNUM",indexTmp);
      int ringTmp = db()->getInt(m_SctFwdWheelRingMap,"RINGNUM",indexTmp);
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
  return db()->getInt(m_SctFwdGeneral,"LAYOUTTYPE");
}

//
// Forward Wheel
//
double SCT_ForwardParameters::materialIncreaseFactor(int iWheel) const
{
  return db()->getDouble(m_SctFwdDiscSupport,"MATSCALEFACTOR",iWheel);
}

int
SCT_ForwardParameters::fwdNumWheels() const
{
  return db()->getInt(m_SctFwdGeneral,"NUMWHEELS");
}


double
SCT_ForwardParameters::fwdWheelZPosition(int iWheel) const
{
  return db()->getDouble(m_SctFwdWheel,"ZPOSITION",iWheel) * Gaudi::Units::mm;
}

int
SCT_ForwardParameters::fwdWheelNumRings(int iWheel) const
{
  return db()->getInt(m_SctFwdWheel,"NUMRINGS",iWheel);
}

int 
SCT_ForwardParameters::fwdWheelRingType(int iWheel, int iRingIndex) const
{
  // get the row number in the m_SctFwdWheelRingMap table
  int index = getRingMapIndex(iWheel,iRingIndex);
  if (index >= 0) {
    return db()->getInt(m_SctFwdWheelRingMap,"RINGTYPE",index);
  } else {
    return 0;
  }
}

int
SCT_ForwardParameters::getRingMapIndex(int iWheel, int iRingIndex) const
{
  int index =  m_wheelRingIndexMap.find(iWheel, iRingIndex);
  if (index < 0)  msg(MSG::ERROR) << "Index not found for wheel,ring =  " << iWheel << ", " << iRingIndex << endmsg;
  return index;
}

//
// Forward DiscSupport
//
double
SCT_ForwardParameters::fwdDiscSupportInnerRadius(int iWheel) const
{
  return db()->getDouble(m_SctFwdDiscSupport,"INNERRADIUS",iWheel) * Gaudi::Units::mm;
} 

double
SCT_ForwardParameters::fwdDiscSupportOuterRadius(int iWheel) const
{
  return db()->getDouble(m_SctFwdDiscSupport,"OUTERRADIUS",iWheel) * Gaudi::Units::mm;
}
 
double
SCT_ForwardParameters::fwdDiscSupportThickness(int iWheel) const
{
  return db()->getDouble(m_SctFwdDiscSupport,"THICKNESS",iWheel) * Gaudi::Units::mm;
} 

std::string
SCT_ForwardParameters::fwdDiscSupportMaterial(int iWheel) const
{
  return db()->getString(m_SctFwdDiscSupport,"MATERIAL",iWheel);
}

//
// Forward Ring
//

int 
SCT_ForwardParameters::fwdRingNumModules(int iRingType) const
{
  return db()->getInt(m_SctFwdRing,"NUMMODULES",iRingType);
}


double
SCT_ForwardParameters::fwdRingInnerRadius(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"INNERRADIUS",iRingType) * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingMiddleRadius(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"MIDDLERADIUS",iRingType) * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingOuterRadius(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"OUTERRADIUS",iRingType) * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingOffset(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"OFFSET",iRingType) * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingModuleStagger(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"MODULESTAGGER",iRingType) * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdRingPhiOfRefModule(int iRingType) const
{
  return db()->getDouble(m_SctFwdRing,"PHIOFREFMODULE",iRingType) * Gaudi::Units::radian;
}

int
SCT_ForwardParameters::fwdRingStaggerOfRefModule(int iRingType) const
{
  return db()->getInt(m_SctFwdRing,"STAGGEROFREFMODULE",iRingType);
}

int
SCT_ForwardParameters::fwdWheelDoubleSided(int iRingType) const
{
  return db()->getInt(m_SctFwdRing,"DOUBLESIDED",iRingType);
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
  return db()->getDouble(m_SctFwdGeneral,"INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdIntermediateRadius() const
{
  return db()->getDouble(m_SctFwdGeneral,"RINTERMEDIATE") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdOuterRadius() const
{
  return db()->getDouble(m_SctFwdGeneral,"OUTERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdZMin() const
{
  return db()->getDouble(m_SctFwdGeneral,"ZMIN") * Gaudi::Units::mm;
} 

double
SCT_ForwardParameters::fwdZIntermediate() const
{
  return db()->getDouble(m_SctFwdGeneral,"ZINTERMEDIATE") * Gaudi::Units::mm;
}

double
SCT_ForwardParameters::fwdZMax() const
{
  return db()->getDouble(m_SctFwdGeneral,"ZMAX") * Gaudi::Units::mm;
}

}
