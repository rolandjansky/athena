/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H
#define MUONCOMBINEDALGS_MUONCOMBINEDINDETEXTENSIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include <string>

namespace MuonCombined {
  class IMuonCombinedInDetExtensionTool;
}

class MuonCombinedInDetExtensionAlg : public AthAlgorithm
{
 public:
  MuonCombinedInDetExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator);

  ~MuonCombinedInDetExtensionAlg();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  ToolHandleArray<MuonCombined::IMuonCombinedInDetExtensionTool> m_muonCombinedInDetExtensionTools;
  SG::ReadHandleKey<InDetCandidateCollection> m_indetCandidateCollectionName;
};


#endif
