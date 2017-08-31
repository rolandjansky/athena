/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONSEGMENTAGALG_H
#define MUONCOMBINEDALGS_MUONSEGMENTAGALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "GaudiKernel/ToolHandle.h"
#include <string>

namespace MuonCombined {
  class IMuonSegmentTagTool;
}

class MuonSegmentTagAlg : public AthAlgorithm
{
 public:
  MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonSegmentTagAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandle<MuonCombined::IMuonSegmentTagTool> m_muonSegmentTagTool;
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName;
  std::string m_muonSegmenteCollectionName;
};


#endif
