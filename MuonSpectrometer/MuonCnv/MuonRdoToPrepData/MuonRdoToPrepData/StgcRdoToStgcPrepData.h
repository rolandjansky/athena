/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRDOTOSTGCPREPDATA_H
#define STGCRDOTOSTGCPREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"

#include "MuonPrepRawData/TgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepDataContainer.h"

class StgcRdoToStgcPrepData : public AthAlgorithm {

public:

  StgcRdoToStgcPrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  ToolHandle< Muon::IMuonRdoToPrepDataTool >    m_decoderTool; //!< Tool used to do actual decoding.
  SG::WriteHandleKey<Muon::sTgcPrepDataContainer> m_prdContainer;
};

#endif /// TGCRDOTOSTGCPREPDATA_H


