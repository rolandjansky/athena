/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H
#define MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"


class MuonTruthSummaryAlg: public ::AthAlgorithm {
 public:
  MuonTruthSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MuonTruthSummaryAlg();

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private:
   std::string                                      m_segmentContainerName;
   std::string                                      m_msTracksContainerName;
   std::string                                      m_msTrackletContainerName;
   ToolHandle<Muon::IMuonTruthSummaryTool>          m_truthSummaryTool;
};

#endif //> !MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H
