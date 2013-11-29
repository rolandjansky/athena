/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDHELPERSVALIDATION_H
#define IDHELPERSVALIDATION_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonIdHelpers/MuonIdHelper.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

// ==============================================================================
//
// Author
// ------
// Philipp Fleischmann <Philipp.Fleischmann@cern.ch>
//
// ******************************************************************************

class Identifier;

namespace Muon {
  class IdHelpersValidation;


/////////////////////////////////////////////////////////////////////////////

class IdHelpersValidation:public AthAlgorithm {
  public:
    IdHelpersValidation(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~IdHelpersValidation(); 

  public:
    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

  private:
    StatusCode dumpIds(const MuonIdHelper* helper, const std::string& name, const std::string& level);
    StatusCode checkMdtIds();
    StatusCode checkCscIds();
    StatusCode checkRpcIds();
    StatusCode checkTgcIds();
    StatusCode checkMmIds();
    StatusCode checkStgcIds();

    StatusCode worstStatus(StatusCode c1, StatusCode c2);

  private:

    const MdtIdHelper*  m_mdtIdHelper;
    const CscIdHelper*  m_cscIdHelper;
    const RpcIdHelper*  m_rpcIdHelper;
    const TgcIdHelper*  m_tgcIdHelper;
    const MmIdHelper*   m_mmIdHelper;
    const sTgcIdHelper* m_stgcIdHelper;

    std::string m_FilePrefix;

    bool m_DoCheckMdtIds;
    bool m_DoCheckCscIds;
    bool m_DoCheckRpcIds;
    bool m_DoCheckTgcIds;
    bool m_DoCheckMmIds;
    bool m_DoCheckStgcIds;

    bool m_MdtIdSanityCheck;
    bool m_CscIdSanityCheck;
    bool m_RpcIdSanityCheck;
    bool m_TgcIdSanityCheck;
    bool m_MmIdSanityCheck;
    bool m_sTgcIdSanityCheck;

  };

} // end namespace

#endif // IDHELPERSVALIDATION_H
