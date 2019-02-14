/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibTools/L1CaloLArTowerEnergy.h"

namespace LVL1{

  L1CaloLArTowerEnergy::L1CaloLArTowerEnergy( const std::string& name ) :
    asg::AsgTool( name ),
    m_condSvc(nullptr),
    m_lvl1Helper(nullptr),
    m_caloMgr(nullptr),
    m_LArOnlineHelper(nullptr),
    m_ttService(nullptr),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers"),
    m_badChannelTool("LArBadChanLegacyTool"),
    m_larCablingSvc("LArCablingLegacyService"),
    m_ttTool("LVL1::L1TriggerTowerTool/LVL1::L1TriggerTowerTool")
  {
  }

  StatusCode L1CaloLArTowerEnergy::initialize() {

    ATH_MSG_INFO("Initialize LVL1::L1CaloLArTowerEnergy");

    StatusCode sc;

    //get a pointer to L1CaloCond services
    sc = service("L1CaloCondSvc", m_condSvc);
    if(sc.isFailure()){ATH_MSG_ERROR( "Cannot access L1CaloCondSvc!" );return sc;}

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
    
    sc = m_cells2tt.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Cannot get L1CaloCells2TriggerTowers !");return sc;}

    sc=m_badChannelTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR( "Could not retrieve BadChannelTool" );return sc;}

    sc=m_larCablingSvc.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR( "Could not retrieve LArCablingService" );return sc;}    


    //Retrieve cabling & tt services
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    IToolSvc* toolSvc;

    sc = svcLoc->service( "ToolSvc",toolSvc  );
    if(sc.isSuccess()) {
      sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttService);
      if(sc.isFailure()){ATH_MSG_ERROR("Could not retrieve CaloTriggerTowerService Tool");return sc;}

    }

    return sc;
  }

  StatusCode L1CaloLArTowerEnergy::finalize() {
      return StatusCode::SUCCESS;
  }



  bool L1CaloLArTowerEnergy::initL1CaloLArTowerEnergy(const CaloCellContainer &caloCellContainer, const TriggerTowerCollection &triggerTowerCollection) {

  StatusCode sc;

    this->reset();

    // access database - each event conditions may have changed ...
    sc = m_ttTool->retrieveConditions();
    if(!sc.isSuccess()) {
      ATH_MSG_ERROR( "Cannot retrieve Conditions in L1TriggerTowerTool." );
      return false;
    }

    // init trigger tower to cell mapping - needed each event?
    if(!m_cells2tt->initCaloCellsTriggerTowers(caloCellContainer)) {
      ATH_MSG_ERROR( "Can not initialize L1CaloCells2TriggerTowers with CaloCellContainer "<< m_caloCellContainerName );
      return false;
    }

    Identifier TTid;
    unsigned int coolId;

    double caloEnergy, sinTheta;

    mapSum::iterator it_map;

    const LVL1::TriggerTower *tt;
    TriggerTowerCollection::const_iterator p_itTT = triggerTowerCollection.begin();
    TriggerTowerCollection::const_iterator p_itTTEnd = triggerTowerCollection.end();

    //Loop to fill the maps
    for(;p_itTT != p_itTTEnd; ++p_itTT) {

      tt = *p_itTT;

      // electromagnetic sampling
      TTid = m_ttTool->identifier(tt->eta(), tt->phi(), 0);
      coolId = m_ttTool->channelID(TTid).id();

      if(!this->hasMissingFEB(TTid) && !m_ttTool->disabledChannel(coolId)) {

        sinTheta = 1. / std::cosh(tt->eta());
        caloEnergy = 1e-3 * m_cells2tt->energy(TTid) * sinTheta;

        m_map_Etcells_em.insert( std::pair<Identifier,double> (TTid,caloEnergy) );

        it_map = m_map_sumEtcells_phi_em.find(tt->eta());
        if(it_map == m_map_sumEtcells_phi_em.end()) {
    std::pair<double,int> Et (caloEnergy,1);
    m_map_sumEtcells_phi_em.insert ( std::pair<double,std::pair<double,int> >(tt->eta(),Et) );
        }
        else  {
    std::pair<double,int> Et = (*it_map).second;
    Et.first += caloEnergy;
    Et.second += 1;
    m_map_sumEtcells_phi_em[tt->eta()] = Et;
        }
      }

      // hadronic sampling
      TTid = m_ttTool->identifier(tt->eta(), tt->phi(), 1);
      coolId = m_ttTool->channelID(TTid).id();

      if(m_ttTool->disabledChannel(coolId)) continue;

      sinTheta = 1. / std::cosh(tt->eta());
      caloEnergy = 1e-3 * m_cells2tt->energy(TTid) * sinTheta;

      m_map_Etcells_had.insert( std::pair<Identifier,double> (TTid,caloEnergy) );

      bool bIsTile   = m_lvl1Helper->is_tile(TTid);
      if(bIsTile) continue;
      if(hasMissingFEB(TTid)) continue;

      it_map = m_map_sumEtcells_phi_had.find(tt->eta());
      if(it_map == m_map_sumEtcells_phi_had.end()) {
        std::pair<double,int> Et (caloEnergy,1);
        m_map_sumEtcells_phi_had.insert ( std::pair<double,std::pair<double,int> >(tt->eta(),Et) );
      }
      else {
        std::pair<double,int> Et = (*it_map).second;
        Et.first += caloEnergy;
        Et.second += 1;
        m_map_sumEtcells_phi_had[tt->eta()] = Et;
      }
    }
    return true;
  }

  float L1CaloLArTowerEnergy::EtLArg(const Identifier& TTid) const {

    float energy = 0;

    mapTT::const_iterator it_mapTT;
    mapSum::const_iterator it_mapSum;

    int sampling = m_lvl1Helper->sampling(TTid);
    double eta = this->IDeta(TTid);

    //electromagnetic sampling
    if(sampling==0) {
      if(this->hasMissingFEB(TTid)) {
        it_mapSum = m_map_sumEtcells_phi_em.find(eta);
        std::pair<double,int> Et = (*it_mapSum).second;
        energy =  Et.first/Et.second;
      }
      else {
        it_mapTT = m_map_Etcells_em.find(TTid);
        energy = (*it_mapTT).second;
      }
    }
    // hadronic sampling
    if(sampling==1) {

      bool bIsTile   = m_lvl1Helper->is_tile(TTid);
      if(bIsTile)
        {
    it_mapTT = m_map_Etcells_had.find(TTid);
    energy = (*it_mapTT).second;
        }
      else {
        if(this->hasMissingFEB(TTid)) {
    it_mapSum = m_map_sumEtcells_phi_had.find(eta);
    std::pair<double,int> Et = (*it_mapSum).second;
    energy =  Et.first/Et.second;
      }
        else {
    it_mapTT = m_map_Etcells_had.find(TTid);
    energy = (*it_mapTT).second;
        }
      }
    }

    return energy;
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


  bool L1CaloLArTowerEnergy::hasMissingFEB(const Identifier& TTid) const {

    bool result = false;

    std::vector<Identifier> TT_cells_vec = m_ttService->createCellIDvecTT(TTid) ;

    if(TT_cells_vec.size()!=0)
      {
        std::vector<Identifier>::const_iterator it  = TT_cells_vec.begin();
        std::vector<Identifier>::const_iterator it_e  = TT_cells_vec.end();

        for (; it!=it_e; ++it)
    {
      HWIdentifier chid = m_larCablingSvc->createSignalChannelID(*it);
      HWIdentifier febId = m_LArOnlineHelper->feb_Id(chid);
      LArBadFeb febstatus = m_badChannelTool->febStatus(febId);
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

  void L1CaloLArTowerEnergy::reset() {

    m_map_sumEtcells_phi_em.clear();
    m_map_sumEtcells_phi_had.clear();
    m_map_Etcells_em.clear();
    m_map_Etcells_had.clear();
  }
} // end of namespace
