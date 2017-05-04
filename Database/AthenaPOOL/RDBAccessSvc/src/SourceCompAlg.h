/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDBACCESSSVC_SOURCECOMPALG_H
#define RDBACCESSSVC_SOURCECOMPALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "boost/shared_ptr.hpp"

#include <string>
#include <map>
#include <vector>

typedef std::map<std::string,IRDBRecordset_ptr> NodeToRecordsetMap;
typedef boost::shared_ptr<NodeToRecordsetMap> NodeToRecordsetMap_ptr;
typedef std::vector<NodeToRecordsetMap_ptr> NodeToRecordsetMapsVec;

class SourceCompAlg : public AthAlgorithm 
{
 public:
  SourceCompAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~SourceCompAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  std::string m_globalTag;
  NodeToRecordsetMapsVec m_recoMaps;
};

#endif
