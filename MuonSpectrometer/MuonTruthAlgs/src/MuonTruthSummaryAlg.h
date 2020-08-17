/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H
#define MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonTruthSummaryTool.h"

#include <string>

class MuonTruthSummaryAlg: public AthAlgorithm {
 public:
  MuonTruthSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MuonTruthSummaryAlg()=default;

  virtual StatusCode initialize();
  virtual StatusCode execute();

 private:
   ToolHandle<Muon::IMuonTruthSummaryTool> m_truthSummaryTool{this,"MuonTruthSummaryTool","MuonTruthSummaryTool"};

   Gaudi::Property<std::string> m_segmentContainerName{this,"SegmentContainerName","MuonSegments"};
   Gaudi::Property<std::string> m_msTracksContainerName{this,"MSTracksContainerName","MuonSpectrometerTracks"};
   Gaudi::Property<std::string> m_msTrackletContainerName{this,"MSTrackletsContainerName","MSonlyTracklets"};
};

#endif //> !MUONTRUTHALGS_MUONTRUTHSUMMARYALGS_H
