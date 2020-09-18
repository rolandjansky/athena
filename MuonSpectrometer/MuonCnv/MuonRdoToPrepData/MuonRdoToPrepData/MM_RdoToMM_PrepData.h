/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RDOTOMM_PREPDATA_H
#define MM_RDOTOMM_PREPDATA_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonCnvToolInterfaces/IMuonRdoToPrepDataTool.h"


class MM_RdoToMM_PrepData : public AthAlgorithm {

public:

  MM_RdoToMM_PrepData(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();

private:

  ToolHandle<Muon::IMuonRdoToPrepDataTool> m_tool{this,"DecodingTool","Muon::MmRdoToPrepDataTool/MM_PrepDataProviderTool"};
  
  bool                                    m_print_inputRdo; //!<< If true, will dump information about the input RDOs.
  bool                                    m_print_prepData; //!<< If true, will dump information about the resulting PRDs.

};

#endif /// CSCRDOTOCSCPREPDATA_H

