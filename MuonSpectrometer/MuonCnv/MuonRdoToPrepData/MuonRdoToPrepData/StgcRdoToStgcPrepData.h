/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRDOTOSTGCPREPDATA_H
#define STGCRDOTOSTGCPREPDATA_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

class StgcRdoToStgcPrepData : public AthAlgorithm {

public:

  StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:
  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_decoderTool{this,"DecodingTool","Muon::sTgcRdoToPrepDataTool/STGC_PrepDataProviderTool"};
  SG::WriteHandleKey<Muon::sTgcPrepDataContainer> m_prdContainer;
};

#endif /// TGCRDOTOSTGCPREPDATA_H


