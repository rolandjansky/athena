/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_DataBase_H
#define SCT_SLHC_GeoModel_SCT_DataBase_H

#include "SCT_SLHC_GeoModel/SCT_GeoModelAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;

namespace InDetDDSLHC {

class SCT_DataBase
{

  
public:

  SCT_DataBase(const SCT_GeoModelAthenaComps * athenaComps);


  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr topLevelTable() const;

  IRDBRecordset_ptr sctEnvelope() const;

  IRDBRecordset_ptr brlSensor() const;
  IRDBRecordset_ptr brlModule() const;

  IRDBRecordset_ptr brlLadder() const;
  IRDBRecordset_ptr brlServPerLayer() const;
  IRDBRecordset_ptr brlSkiZ() const;
  IRDBRecordset_ptr brlLayer() const;
  IRDBRecordset_ptr brlGeneral() const;

  IRDBRecordset_ptr fwdSensor() const;
  IRDBRecordset_ptr fwdSpine() const;
  IRDBRecordset_ptr fwdModule() const;

  IRDBRecordset_ptr fwdRing() const;
  IRDBRecordset_ptr fwdWheel() const;
  IRDBRecordset_ptr fwdWheelRingMap() const;
  IRDBRecordset_ptr fwdDiscSupport() const;
  IRDBRecordset_ptr fwdGeneral() const;

  // Return the SCT version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:

  const SCT_GeoModelAthenaComps * m_athenaComps;

  std::string m_sctVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_topLevel;
  IRDBRecordset_ptr m_envelope;

  IRDBRecordset_ptr m_brlSensor;
  IRDBRecordset_ptr m_brlModule;
  //IRDBRecordset_ptr m_brlSki;
  IRDBRecordset_ptr m_brlLadder;
  IRDBRecordset_ptr m_brlServPerLayer;
  IRDBRecordset_ptr m_brlSkiZ;
  IRDBRecordset_ptr m_brlLayer;
  IRDBRecordset_ptr m_brlGeneral;
  IRDBRecordset_ptr m_fwdSensor;
  IRDBRecordset_ptr m_fwdHybrid;
  IRDBRecordset_ptr m_fwdSpine;
  IRDBRecordset_ptr m_fwdModule;
  IRDBRecordset_ptr m_fwdRing;
  IRDBRecordset_ptr m_fwdWheel;
  IRDBRecordset_ptr m_fwdWheelRingMap;
  IRDBRecordset_ptr m_fwdDiscSupport;
  IRDBRecordset_ptr m_fwdGeneral;


};
}
#endif //SCT_SLHC_GeoModel_SCT_DataBase_H
