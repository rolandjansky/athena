/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDBACCESSSVC_SOURCECOMPALG_H
#define RDBACCESSSVC_SOURCECOMPALG_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <fstream>
#include <vector>

class RDBAccessSvc;

class SourceCompAlg : public AthAlgorithm 
{
 public:
  SourceCompAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

 private:
  Gaudi::Property<std::string> m_globalTag {this
      , "GlobalTag"
      , ""
      , "Global geometry tag. If empty, all locked and supported tags will be compared"};

  std::vector<std::string> m_connNames{"Session1","Session2"};

  std::vector<std::string> getGlobalTags(RDBAccessSvc* rdbAccessSvc
					 , std::ofstream& log);

  StatusCode compareGlobalTags(const std::vector<std::string>& globalTags
			       , RDBAccessSvc* rdbAccessSvc
			       , std::ofstream& log);
};

#endif
