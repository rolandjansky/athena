/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"

#include "InDetGeoModelUtils/InDetDDAthenaComps.h"
//#include "PixelGeoModel/PixelGeoModelAthenaComps.h"
#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

#include <iostream>
using namespace std; // for DEBUG

OraclePixGeoAccessor::OraclePixGeoAccessor( const InDetDD::AthenaComps* athenaComps,
					    const std::string& det)
{
  DecodeVersionKey versionKey( athenaComps->geoModelSvc(), det);  // normally det = "Pixel"
  m_detectorKey  = versionKey.tag();
  m_detectorNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(athenaComps->geoModelSvc(), "InnerDetector");
  m_detectorKeyInDet  = indetVersionKey.tag();
  m_detectorNodeInDet = indetVersionKey.node();

  cout << "OraclePixGeoAccessor: detectorKey= " << m_detectorKey
       << " detectorNode= " << m_detectorNode << endl;

  m_rdbSvc = athenaComps->rdbAccessSvc();
  m_geomDB = athenaComps->geomDB();

}

OraclePixGeoAccessor::OraclePixGeoAccessor( IGeometryDBSvc* geomDB, IRDBAccessSvc* rdbSvc, IGeoModelSvc *geoModelSvc,
					    const std::string& det):
  m_geomDB(geomDB),m_rdbSvc(rdbSvc)
{
  DecodeVersionKey versionKey( geoModelSvc, det);  // normally det = "Pixel"
  m_detectorKey  = versionKey.tag();
  m_detectorNode = versionKey.node();

  // Get version tag and node for InnerDetector.
  DecodeVersionKey indetVersionKey(geoModelSvc, "InnerDetector");
  m_detectorKeyInDet  = indetVersionKey.tag();
  m_detectorNodeInDet = indetVersionKey.node();

  cout << "OraclePixGeoAccessor: detectorKey= " << m_detectorKey
       << " detectorNode= " << m_detectorNode << endl;
}


IRDBRecordset_ptr OraclePixGeoAccessor::getTable( const std::string & name) const
{
  IRDBRecordset_ptr table = m_rdbSvc->getRecordsetPtr( name, m_detectorKey, m_detectorNode);
  if (!table.get()||table->size()==0) cout << "WARNING: table " << name << " does not exist / is empty" << endl;
  return table;
}

IRDBRecordset_ptr OraclePixGeoAccessor::getTableInDet( const std::string & name) const
{
  IRDBRecordset_ptr table = m_rdbSvc->getRecordsetPtr( name, m_detectorKeyInDet, m_detectorNodeInDet);
  if (table.get() == 0 || table->size()==0) cout << "WARNING: table " << name << " does not exist /is empty" << endl;
  return table;
}

double OraclePixGeoAccessor::getDouble( IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
//   //if (db()->testField( recordSet, name, index)) {

//   std::cout<<"getDouble "<<recordSet->nodeName()<<" "<<name<<" "<<index<<"   "<<recordSet->size()<<std::endl; 
//   std::cout<<"getDouble : "<<db()->getDouble( recordSet, name, index)<<std::endl;

//    return db()->getDouble( recordSet, name, index);
//     //} 
//   return 0.;

  if (db()->testField( recordSet, name, index)) {
    
    return db()->getDouble( recordSet, name, index);
  } 
  return 0.;
}

int OraclePixGeoAccessor::getInt( IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  if (db()->testField( recordSet, name, index)) {
    return db()->getInt( recordSet, name, index);
  } 
  std::cout<<"OracleOixGeoAccessor : getInt not found "<<name<<std::endl;
  return 0;
}

std::string OraclePixGeoAccessor::getString( IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  if (db()->testField( recordSet, name, index)) {
    return db()->getString( recordSet, name, index);
  } 
  return "";
}

bool OraclePixGeoAccessor::testField( IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  return db()->testField( recordSet, name, index);
}

bool OraclePixGeoAccessor::testFieldTxt( IRDBRecordset_ptr recordSet, const std::string & name, int index) const
{
  return db()->testFieldTxt( recordSet, name, index);
}

unsigned int OraclePixGeoAccessor::getTableSize( IRDBRecordset_ptr recordSet) const
{
  if(recordSet.get()==0 || recordSet->size()==0 )return 0;
  return db()->getTableSize( recordSet);
}

double OraclePixGeoAccessor::getDouble( const std::string& tableName, const std::string & name, int index) const
{
  return db()->getDouble( tableName, name, index);
}
int OraclePixGeoAccessor::getInt( const std::string& tableName, const std::string & name, int index) const
{
  return db()->getInt( tableName, name, index);
}
std::string OraclePixGeoAccessor::getString( const std::string& tableName, const std::string & name, int index) const
{
  return db()->getString( tableName, name, index);
}
bool OraclePixGeoAccessor::testField( const std::string& tableName, const std::string & name, int index) const
{
  return db()->testField( tableName, name, index);
}
