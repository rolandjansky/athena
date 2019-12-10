/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 test MuonGeoModel from digits to pos. in space
 ----------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: MuonRpcCablingTest.h,v 1.15 2009/03/28 10:59:00 stefspa Exp $
//<version>	$Name:  $

#ifndef MUONGEOMODEL_MUONRPCCABLINGTEST_H
# define MUONGEOMODEL_MUONRPCCABLINGTEST_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

class StoreGateSvc;
class RpcIdHelper;
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
    ~MuonRpcCablingTest();
    
    StatusCode	initialize();
    StatusCode 	execute();
    StatusCode 	finalize();

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
    
    ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
      "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
};

#endif // MUONGEOMODEL_MUONRPCCABLINGTEST_H
