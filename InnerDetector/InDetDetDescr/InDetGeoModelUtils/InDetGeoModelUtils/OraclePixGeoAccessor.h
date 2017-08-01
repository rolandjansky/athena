/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef OraclePixGeoAccessor_H
#define OraclePixGeoAccessor_H

#include "RDBAccessSvc/IRDBAccessSvc.h"

#include <string>

//class PixelGeoModelAthenaComps;
namespace InDetDD { class AthenaComps;}
class IGeometryDBSvc;
class IGeoModelSvc;
class IRDBAccessSvc;
class InDetMaterialManager;

class OraclePixGeoAccessor {
public:

  OraclePixGeoAccessor( const InDetDD::AthenaComps* athenaComps, const std::string& det);
  OraclePixGeoAccessor( IGeometryDBSvc* geomDB, IRDBAccessSvc* rdbSvc, IGeoModelSvc *geoModelSvc, const std::string& det);

  IRDBRecordset_ptr getTable( const std::string & name) const;
  IRDBRecordset_ptr getTableInDet( const std::string & name) const;

  /// The following methods will first look in the text file if provided and then look in the database.
  double        getDouble( IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  int           getInt( IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  std::string   getString( IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const; 
  bool          testField( IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  bool          testFieldTxt( IRDBRecordset_ptr recordSet, const std::string & name, int index=0) const;
  unsigned int  getTableSize( IRDBRecordset_ptr recordSet) const;

  double        getDouble( const std::string& tableName, const std::string & name, int index=0) const;
  int           getInt( const std::string& tableName, const std::string & name, int index=0) const;
  std::string   getString( const std::string& tableName, const std::string & name, int index=0) const; 
  bool          testField( const std::string& tableName, const std::string & name, int index=0) const;

  std::string versionTag() const { return m_detectorKey; } 

private:

  IGeometryDBSvc* m_geomDB;
  IRDBAccessSvc*  m_rdbSvc;
  std::string     m_detectorKey;
  std::string     m_detectorNode;
  std::string     m_detectorKeyInDet;
  std::string     m_detectorNodeInDet;

  IGeometryDBSvc* db() const {return m_geomDB;}

};

#endif
