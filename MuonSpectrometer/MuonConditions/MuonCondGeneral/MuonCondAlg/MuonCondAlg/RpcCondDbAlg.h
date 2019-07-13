/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_RPCCONDDBALG_H
#define MUONCONDALG_RPCCONDDBALG_H

//STL includes
#include <string>
#include <sstream>
#include <vector>
#include <zlib.h>

//Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ToolHandle.h"

//Athena includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "Identifier/Identifier.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCondData/RpcCondDbData.h"
#include "MuonCondSvc/MdtStringUtils.h"

//forward declarations
class Identifier;
class MuonIdHelper;
class StatusCode;

namespace Muon {
  class MuonIdHelperTool;
}



class RpcCondDbAlg: public AthAlgorithm{

public:

    RpcCondDbAlg( const std::string & name, ISvcLocator* svc);
    virtual ~RpcCondDbAlg() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute   () override;
    virtual StatusCode finalize  () override;

 
private:

    virtual StatusCode loadDataDeadPanels (EventIDRange &, std::unique_ptr<RpcCondDbData>&);
    virtual StatusCode loadDataOffPanels  (EventIDRange &, std::unique_ptr<RpcCondDbData>&);
    virtual StatusCode loadMcElementStatus(EventIDRange &, std::unique_ptr<RpcCondDbData>&);

    bool m_isOnline;
    bool m_isData;  
    bool m_isRun1;   

    ServiceHandle<ICondSvc> m_condSvc;
    ToolHandle<Muon::MuonIdHelperTool> m_idHelper;
 
    SG::WriteCondHandleKey<RpcCondDbData> m_writeKey{this, "WriteKey", "RpcCondDbData", "Key of output RPC condition data"};    

    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_deadPanels  {this, "ReadKey_DA_DP", "/RPC/DCS/DeadRopanels"   , "Key of input RPC condition data for data dead panels"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_da_offPanels   {this, "ReadKey_DA_OP", "/RPC/DCS/OffRopanels"    , "Key of input RPC condition data for data off panels"};
    SG::ReadCondHandleKey<CondAttrListCollection> m_readKey_folder_mc_deadElements{this, "ReadKey_MC_DE", "/RPC/DQMF/ELEMENT_STATUS", "Key of input RPC condition data for MC dead elements"};

	float m_panelEfficiency;

};


#endif
