/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloLArTowerEnergy.h"
#include "LArRecConditions/LArBadChannelCont.h"
#include "StoreGate/ReadCondHandle.h"

namespace LVL1{

  L1CaloLArTowerEnergy::L1CaloLArTowerEnergy( const std::string& name ) :
    asg::AsgTool( name ),
    m_lvl1Helper(nullptr),
    m_caloMgr(nullptr),
    m_LArOnlineHelper(nullptr),
    m_ttService(nullptr),
    m_badFebMasker("LArBadFebMasker")
  {
  }

  StatusCode L1CaloLArTowerEnergy::initialize() {

    ATH_MSG_INFO("Initialize LVL1::L1CaloLArTowerEnergy");

    StatusCode sc;

    sc = detStore()->retrieve(m_caloMgr) ;
    if(sc.isFailure()){ATH_MSG_ERROR( "Cannot access caloMgr");return sc;}

    sc = detStore()->retrieve(m_LArOnlineHelper,"LArOnlineID") ;
    if(sc.isFailure()){ATH_MSG_ERROR( "Cannot access LArOnlineID");return sc;}
    
    //Use the CaloIdManager to get a pointer to an instance of the CaloLVL1_ID helper
    m_lvl1Helper = m_caloMgr->getLVL1_ID();
    if(!m_lvl1Helper) {
      ATH_MSG_ERROR( "Cannot access CaloLVL1_ID helper." );
      return StatusCode::FAILURE;
    }
    
    ATH_CHECK(m_badFebMasker.retrieve());

    //Retrieve cabling & tt services
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    IToolSvc* toolSvc;

    sc = svcLoc->service( "ToolSvc",toolSvc  );
    if(sc.isSuccess()) {
      sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttService);
      if(sc.isFailure()){ATH_MSG_ERROR("Could not retrieve CaloTriggerTowerService Tool");return sc;}

    }

    ATH_CHECK( m_cablingKey.initialize() );

    return sc;
  }

  StatusCode L1CaloLArTowerEnergy::finalize() {
      return StatusCode::SUCCESS;
  }



  double L1CaloLArTowerEnergy::IDeta(const Identifier& TTid) const {
    int region = m_lvl1Helper->region(TTid);
    int ieta = m_lvl1Helper->eta(TTid);
    int sign = m_lvl1Helper->pos_neg_z(TTid);

    double gran[4] = {0.1, 0.2, 0.1, 0.425};
    double offset[4] = {0., 2.5, 3.1, 3.2};
    double eta;

    if (region>=0 && region<=3) {
      eta = sign* ( ( (ieta+0.5) * gran[region] ) + offset[region] );
    }
    else {
      eta = 0.;
    }
    return eta;
  }


  bool L1CaloLArTowerEnergy::hasMissingFEB(const Identifier& TTid) const
  {
    SG::ReadCondHandle<LArOnOffIdMapping> cabling (m_cablingKey);
    return hasMissingFEB (**cabling, TTid);
  }
  bool L1CaloLArTowerEnergy::hasMissingFEB(const LArOnOffIdMapping& cabling,
                                           const Identifier& TTid) const
  {
    bool result = false;

    std::vector<Identifier> TT_cells_vec = m_ttService->createCellIDvecTT(TTid) ;

    if(TT_cells_vec.size()!=0)
      {
        std::vector<Identifier>::const_iterator it  = TT_cells_vec.begin();
        std::vector<Identifier>::const_iterator it_e  = TT_cells_vec.end();

        for (; it!=it_e; ++it)
    {
      HWIdentifier chid = cabling.createSignalChannelID(*it);
      HWIdentifier febId = m_LArOnlineHelper->feb_Id(chid);
      LArBadFeb febstatus = m_badFebMasker->febStatus(febId);
      bool deadReadout = febstatus.deadReadout();
      bool desactivatedInOKS = febstatus.deactivatedInOKS();

      if(deadReadout || desactivatedInOKS)  result = true;
    }
      }
    else{
      ATH_MSG_ERROR( "Cannot retreive feb status for trigger Tower " << TTid );
    }

    return result;
  }
} // end of namespace
