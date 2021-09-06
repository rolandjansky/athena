/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOMonitorAlgorithm.h"
#include "xAODPFlow/FEHelpers.h"

PFOMonitorAlgorithm::PFOMonitorAlgorithm( const std::string& name, ISvcLocator* pSvcLocator )
:AthMonitorAlgorithm(name,pSvcLocator)
{}


PFOMonitorAlgorithm::~PFOMonitorAlgorithm() {}


StatusCode PFOMonitorAlgorithm::initialize() {
    // initialize superclass
    ATH_CHECK( AthMonitorAlgorithm::initialize() );
    
    ATH_CHECK( m_ChargedPFOContainerKey.initialize() );
    ATH_CHECK( m_NeutralPFOContainerKey.initialize() );

    // release 21 containters
    //ATH_CHECK( m_oldChargedPFOContainerKey.initialize() );
    //ATH_CHECK( m_oldNeutralPFOContainerKey.initialize() );
    return StatusCode::SUCCESS;
}


StatusCode PFOMonitorAlgorithm::fillHistograms( const EventContext& ctx ) const {
    using namespace Monitored;

    // Declare the quantities which should be monitored
    auto nPFOs = Monitored::Scalar<int>("nPFOs",0.0);
    auto pT     = Monitored::Scalar<float>("pfopT",0.0);
    auto eta    = Monitored::Scalar<float>("pfoEta",0.0);
    auto phi    = Monitored::Scalar<float>("pfoPhi",0.0);
    auto mass   = Monitored::Scalar<float>("pfoMass",0.0);
    auto E      = Monitored::Scalar<float>("pfoE",0.0);
    auto y      = Monitored::Scalar<float>("pfoRapidity",0.0);

    auto nChPFOs = Monitored::Scalar<int>("nChPFOs",0.0);
    auto ChpT     = Monitored::Scalar<float>("ChpfopT",0.0);
    auto Cheta    = Monitored::Scalar<float>("ChpfoEta",0.0);
    auto Chphi    = Monitored::Scalar<float>("ChpfoPhi",0.0);
    auto Chmass   = Monitored::Scalar<float>("ChpfoMass",0.0);
    auto ChE      = Monitored::Scalar<float>("ChpfoE",0.0);
    auto Chy      = Monitored::Scalar<float>("ChpfoRapidity",0.0);
    auto ChExpE   = Monitored::Scalar<float>("ChpfoExpE",0.0);
    auto ChDenseEnv = Monitored::Scalar<int>("ChpfoDenseEnv",0.0);

// Declare cutmasks for dense environment
    auto DenseEnvFlag = Monitored::Scalar<bool>("DenseEnvFlag",false);
    
    auto nNuPFOs = Monitored::Scalar<int>("nNuPFOs",0.0);
    auto NupT     = Monitored::Scalar<float>("NupfopT",0.0);
    auto Nueta    = Monitored::Scalar<float>("NupfoEta",0.0);
    auto Nuphi    = Monitored::Scalar<float>("NupfoPhi",0.0);
    auto Numass   = Monitored::Scalar<float>("NupfoMass",0.0);
    auto NuE      = Monitored::Scalar<float>("NupfoE",0.0);
    auto Nuy      = Monitored::Scalar<float>("NupfoRapidity",0.0);
    auto NuSecondR = Monitored::Scalar<float>("NupfoSecondR",0.0);
    auto NuSecondLambda = Monitored::Scalar<float>("NupfoSecondLambda",0.0);
    auto NuCenterLambda   = Monitored::Scalar<float>("NupfoCenterLambda",0.0);
    auto NuNBadCells  = Monitored::Scalar<float>("NupfoNBadCells",0.0);
    auto NuEBadCells  = Monitored::Scalar<float>("NupfoEBadCells",0.0);
    auto NuBadLarQFrac= Monitored::Scalar<float>("NupfoBadLarQFrac",0.0);
    auto NuIso    = Monitored::Scalar<float>("NupfoIsolation",0.0);
    auto NuEPos   = Monitored::Scalar<float>("NupfoEPos",0.0);
    auto NuNBadC  = Monitored::Scalar<int>("NupfoNBadCells",0);
    auto NuAvgLarQ= Monitored::Scalar<float>("NupfoAvgLarQ",0.0);
    auto NuAvgTileQ= Monitored::Scalar<float>("NupfoAvgTileQ",0.0);
    auto NuEMProb= Monitored::Scalar<float>("NupfoEMProb",0.0);
    // Not currently filled in the xAOD
    //auto NuSignif = Monitored::Scalar<float>("NupfoSignificance",0.0);
    //auto NuEFracMX= Monitored::Scalar<float>("NupfoEFracMax",0.0);
    
    // Access the Particle Flow Objects via StoreGate
    SG::ReadHandle<xAOD::FlowElementContainer> chpfos(m_ChargedPFOContainerKey, ctx);
    if (! chpfos.isValid() ) {
        ATH_MSG_WARNING("evtStore() does not contain PFO Collection with name "<< m_ChargedPFOContainerKey);
        return StatusCode::SUCCESS; //Return SUCCESS so processing proceeds
    }

    SG::ReadHandle<xAOD::FlowElementContainer> nupfos(m_NeutralPFOContainerKey, ctx);
    if (! nupfos.isValid() ) {
        ATH_MSG_WARNING("evtStore() does not contain PFO Collection with name "<< m_NeutralPFOContainerKey);
        return StatusCode::SUCCESS; //Return SUCCESS so processing proceeds
    }

    // Set monitored variables for this event
    nPFOs = chpfos->size() + nupfos->size();
    nChPFOs = chpfos->size();
    nNuPFOs = nupfos->size();

    fill("PFOMonitorAllPFOs",nPFOs);
    fill("PFOMonitorChargedPFOs",nChPFOs);
    fill("PFOMonitorNeutralPFOs",nNuPFOs);    

    // Accessors for neutral PFO moments
    const static SG::AuxElement::ConstAccessor<float> acc_SECOND_R("SECOND_R");
    const static SG::AuxElement::ConstAccessor<float> acc_CENTER_LAMBDA("CENTER_LAMBDA");
    const static SG::AuxElement::ConstAccessor<float> acc_ISOLATION("ISOLATION");
    const static SG::AuxElement::ConstAccessor<float> acc_N_BAD_CELLS("N_BAD_CELLS");
    const static SG::AuxElement::ConstAccessor<float> acc_ENG_BAD_CELLS("ENG_BAD_CELLS");
    const static SG::AuxElement::ConstAccessor<float> acc_BADLARQ_FRAC("BADLARQ_FRAC");
    const static SG::AuxElement::ConstAccessor<float> acc_ENG_POS("ENG_POS");
    const static SG::AuxElement::ConstAccessor<float> acc_AVG_LAR_Q("AVG_LAR_Q");
    const static SG::AuxElement::ConstAccessor<float> acc_AVG_TILE_Q("AVG_TILE_Q");
    const static SG::AuxElement::ConstAccessor<float> acc_EM_PROBABILITY("EM_PROBABILITY");
    const static SG::AuxElement::ConstAccessor<float> acc_SECOND_LAMBDA("SECOND_LAMBDA");
    // Not currently filled in the xAOD
    //const static SG::AuxElement::ConstAccessor<float> acc_SIGNIFICANCE("SIGNIFICANCE");
    //const static SG::AuxElement::ConstAccessor<float> acc_ENG_FRAC_MAX("ENG_FRAC_MAX");

    // Accessors for charged PFO moments
    const static SG::AuxElement::ConstAccessor<int> acc_IsInDenseEnvironment("IsInDenseEnvironment");
    const static SG::AuxElement::ConstAccessor<float> acc_tracksExpectedEnergyDeposit("TracksExpectedEnergyDeposit");

    for (const auto& pfo : *chpfos) {
      pT  = pfo->pt();
      eta = pfo->eta();
      phi = pfo->phi();
      mass= pfo->m();
      E   = pfo->e();
      y   = pfo->rapidity();
      //float score = bdtPi0Score();

      int   denseEnv = 0;
      float expectedEnergy = -1.0;

      // Fill moments associated with tracks and track association
      if(acc_IsInDenseEnvironment.isAvailable(*pfo)) {denseEnv = acc_IsInDenseEnvironment(*pfo);}
      else {ATH_MSG_DEBUG("This charged PFO did not have IsInDenseEnvironment set");}

      if(acc_tracksExpectedEnergyDeposit.isAvailable(*pfo)) {expectedEnergy = acc_tracksExpectedEnergyDeposit(*pfo);}
      else {ATH_MSG_DEBUG("This charged PFO did not have TracksExpectedEnergyDeposit set");}

      ChpT  = pfo->pt();
      Cheta = pfo->eta();
      Chphi = pfo->phi();
      ChE   = pfo->e();
      Chy   = pfo->rapidity();
      ChDenseEnv = denseEnv;
      ChExpE= expectedEnergy;

      // Fill. First argument is the tool name, all others are the variables to be saved.
      fill("PFOMonitorAllPFOs",pT,eta,phi,mass,E,y);
      DenseEnvFlag = denseEnv;
      fill("PFOMonitorChargedPFOs",ChpT,Cheta,Chphi,Chmass,ChE,Chy,ChDenseEnv,ChExpE,DenseEnvFlag);
	      
    }

    for (const auto& pfo : *nupfos) {
      pT  = pfo->pt();
      eta = pfo->eta();
      phi = pfo->phi();
      mass= pfo->m();
      E   = pfo->e();
      y   = pfo->rapidity();
      
      NupT  = pfo->pt();
      Nueta = pfo->eta();
      Nuphi = pfo->phi();
      Numass= pfo->m();
      NuE   = pfo->e();
      Nuy   = pfo->rapidity();

      if (acc_ISOLATION.isAvailable(*pfo)) {NuIso = acc_ISOLATION(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have ISOLATION set");}

      if (acc_ENG_POS.isAvailable(*pfo)) {NuEPos= acc_ENG_POS(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have ENG_POS set");}

      if (acc_N_BAD_CELLS.isAvailable(*pfo)) {NuNBadCells = acc_N_BAD_CELLS(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have N_BAD_CELLS set");}

      if (acc_ENG_BAD_CELLS.isAvailable(*pfo)) {NuEBadCells = acc_ENG_BAD_CELLS(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have ENG_BAD_CELLS set");}

      if (acc_AVG_LAR_Q.isAvailable(*pfo)) {NuAvgLarQ = acc_AVG_LAR_Q(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have AVG_LAR_Q set");}

      if (acc_AVG_TILE_Q.isAvailable(*pfo)) {NuAvgTileQ = acc_AVG_TILE_Q(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have AVG_TILE_Q set");}

      if (acc_SECOND_R.isAvailable(*pfo)) {NuSecondR = acc_SECOND_R(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have SECOND_R set");}

      if (acc_SECOND_LAMBDA.isAvailable(*pfo)) {NuSecondLambda = acc_SECOND_LAMBDA(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have SECOND_LAMBDA set");}

      if (acc_CENTER_LAMBDA.isAvailable(*pfo)) {NuCenterLambda = acc_CENTER_LAMBDA(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have CENTER_LAMBDA set");}

      if (acc_BADLARQ_FRAC.isAvailable(*pfo)) {NuBadLarQFrac = acc_BADLARQ_FRAC(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have BAD_LARQ_FRAC set");}

      if (acc_EM_PROBABILITY.isAvailable(*pfo)) {NuEMProb = acc_EM_PROBABILITY(*pfo);}
      else {ATH_MSG_DEBUG("This neutral PFO did not have BAD_LARQ_FRAC set");}

      // These moments do not appear to be filled currently in the xAOD, but leave commented out
      // in case they are turned on later.
      //if (acc_SIGNIFICANCE.isAvailable(*pfo)) {NuSignif = acc_SIGNIFICANCE(*pfo);}
      //else {ATH_MSG_DEBUG("This charged PFO did not have SIGNIFICANCE set");}
      //if (acc_ENG_FRAC_MAX.isAvailable(*pfo)) {NuEFracMAX = acc_ENG_FRAC_MAX(*pfo);}
      //else {ATH_MSG_DEBUG("This neutral PFO did not have ENG_FRAC_MAX set");}

      // Fill. First argument is the tool name, all others are the variables to be saved.
      fill("PFOMonitorAllPFOs",pT,eta,phi,mass,E,y);
      fill("PFOMonitorNeutralPFOs",NupT,Nueta,Nuphi,Numass,NuE,Nuy,NuIso,NuEPos,NuNBadC,NuEBadCells,NuAvgLarQ,NuSecondR,NuCenterLambda,NuSecondLambda,NuBadLarQFrac,NuEMProb);
    }

    return StatusCode::SUCCESS;
}
