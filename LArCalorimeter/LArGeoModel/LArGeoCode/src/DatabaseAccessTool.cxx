/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoCode/DatabaseAccessTool.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include <map>

class DatabaseAccessTool::Clockwork {

public:

  IRDBAccessSvc *pAccessSvc;
  std::string detectorKey;
  std::string detectorNode;
  std::map<std::string, IRDBRecordset_ptr> recMap;
};


DatabaseAccessTool::DatabaseAccessTool ()
  :m_cw(new Clockwork())
{
  IGeoModelSvc *geoModel;
  ISvcLocator  *svcLocator=Gaudi::svcLocator();
  if (svcLocator->service ("GeoModelSvc",geoModel)!=StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }
 
  if (svcLocator->service("RDBAccessSvc",m_cw->pAccessSvc)!=StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }
 
  // Obtain the geometry version information:
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();
  
  m_cw->detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  m_cw->detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  
}

DatabaseAccessTool::~DatabaseAccessTool () 
{
  delete m_cw;
}

double DatabaseAccessTool::getDouble(const std::string & TableName,  
				     const std::string & FallbackVersion, 
				     const std::string & ColumnName) const  {

  std::map<std::string, IRDBRecordset_ptr>::iterator m=m_cw->recMap.find(TableName);
  
  IRDBRecordset_ptr rec = (m==m_cw->recMap.end()) ? m_cw->pAccessSvc->getRecordsetPtr(TableName,m_cw->detectorKey,m_cw->detectorNode): (*m).second;
  if (rec->size()==0) {
    rec = m_cw->pAccessSvc->getRecordsetPtr(TableName,FallbackVersion);
    if (rec->size()==0) {
      throw std::runtime_error((std::string("Cannot find the Table: ") + TableName).c_str());
    }
  }
  double retval = (*rec)[0]->getDouble(ColumnName);
  if (m==m_cw->recMap.end()) {
    m_cw->recMap[TableName]=rec;
  }
  return retval;
}
