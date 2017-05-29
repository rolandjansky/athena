/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAW2DIGITOOL_H
#define AFP_RAW2DIGITOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AFP_Raw2Digi/IAFP_Raw2DigiTool.h"
#include <string>

class AFP_Raw2DigiTool
  : virtual public ::IAFP_Raw2DigiTool,
    public ::AthAlgTool {
public:
  AFP_Raw2DigiTool(const std::string &type, const std::string &name, const IInterface *parent);	
  virtual ~AFP_Raw2DigiTool();
  virtual StatusCode initialize();
  StatusCode recoSiHits();
  virtual StatusCode finalize();

protected:
  std::string m_rawDataContainerName;
  std::string m_AFPSiHitsContainerName;
};
#endif 
