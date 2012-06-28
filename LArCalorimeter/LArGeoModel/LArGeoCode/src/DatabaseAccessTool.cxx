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
  :cw(new Clockwork())
{
  IGeoModelSvc *geoModel;
  ISvcLocator  *svcLocator=Gaudi::svcLocator();
  if (svcLocator->service ("GeoModelSvc",geoModel)!=StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }
 
  if (svcLocator->service("RDBAccessSvc",cw->pAccessSvc)!=StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }
 
  // Obtain the geometry version information:
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();
  
  cw->detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  cw->detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  
  cw->pAccessSvc->connect();

}

DatabaseAccessTool::~DatabaseAccessTool () 
{
  cw->pAccessSvc->disconnect();
  delete cw;
}

double DatabaseAccessTool::getDouble(const std::string & TableName,  
				     const std::string & FallbackVersion, 
				     const std::string & ColumnName) const  {

  std::map<std::string, IRDBRecordset_ptr>::iterator m=cw->recMap.find(TableName);
  
  IRDBRecordset_ptr rec = (m==cw->recMap.end()) ? cw->pAccessSvc->getRecordsetPtr(TableName,cw->detectorKey,cw->detectorNode): (*m).second;
  if (rec->size()==0) {
    rec = cw->pAccessSvc->getRecordsetPtr(TableName,FallbackVersion);
    if (rec->size()==0) {
      throw std::runtime_error((std::string("Cannot find the Table: ") + TableName).c_str());
    }
  }
  double retval = (*rec)[0]->getDouble(ColumnName);
  if (m==cw->recMap.end()) {
    cw->recMap[TableName]=rec;
  }
  return retval;
}
