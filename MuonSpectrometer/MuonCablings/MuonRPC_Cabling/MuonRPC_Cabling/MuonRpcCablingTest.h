/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONGEOMODEL_MUONRPCCABLINGTEST_H
# define MUONGEOMODEL_MUONRPCCABLINGTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class IRPCcablingSvc;
class RpcPadIdHash;

namespace MuonGM
{    
    class MuonReadoutElement;
    class MuonDetectorManager;
}

class MuonRpcCablingTest: public AthAlgorithm
{
public:
    
    MuonRpcCablingTest(const std::string& name, ISvcLocator* pSvcLocator);
    ~MuonRpcCablingTest() {};
    
    StatusCode	initialize();
    StatusCode 	execute();

private:

    bool m_first;
    const IRPCcablingSvc*         m_cablingSvc;
    // RpcPadIdHash helper 	
    RpcPadIdHash* m_padHashIdHelper;
    

    /** configurable flags */
    std::string m_selSideStr;
    std::vector<int> m_selSectorVec;
    std::vector<int> m_selRoiVec;
    std::vector<int> m_selPadVec;
    bool m_firstLastChannel;
    
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
};

#endif // MUONGEOMODEL_MUONRPCCABLINGTEST_H
