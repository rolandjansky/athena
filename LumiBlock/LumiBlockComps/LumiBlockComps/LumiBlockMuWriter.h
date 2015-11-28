/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

/**
   @class LumiBlockMuWriter
   @brief Algorithm to write interactions per crossing (mu) into EventInfo
   @author E.Torrence
**/

#ifndef LUMIBLOCKCOMPS_LumiBlockMuWriter_H
#define LUMIBLOCKCOMPS_LumiBlockMuWriter_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "LumiBlockComps/ILuminosityTool.h"
#include "LumiCalc/CoolQuery.h"

#include <string>

class LumiBlockMuWriter: public AthAlgorithm {
 public:
  LumiBlockMuWriter(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode          initialize();
  StatusCode          execute();
  StatusCode          finalize();
  
 private:
  ToolHandle<ILuminosityTool> m_lumiTool;

  // Direct DB access properties for pileup events
  bool m_writePileupEvents; // Attempt to write mu values into pileup (overlay) events.  Default is FALSE
  StringProperty m_lumiDatabase;
  StringProperty m_lumiFolder;
  StringProperty m_lumiTag;
  unsigned int m_lumiChannel;

  CoolQuery* m_lumiDB;

  CoolQuery::LumiFolderData getDBLuminosity(unsigned int run, unsigned int lb);
 

};

#endif

#endif

