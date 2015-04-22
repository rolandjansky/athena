/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigEgammaAnalysisTools/TrigEgammaNavNtuple.h"

using namespace std;

TrigEgammaNavNtuple::TrigEgammaNavNtuple( const std::string& myname ): TrigEgammaNavBaseTool(myname) 
{
  declareProperty("DirectoryPath",m_dir="NavNtuple");
  declareProperty("doRinger", m_doRinger=false);
}


StatusCode TrigEgammaNavNtuple::childInitialize(){

  addDirectory(m_dir);
  for (int i = 0; i < (int) m_trigList.size(); i++) {
    
    std::string trigItem = m_trigList[i];
    TTree *t = new TTree( (trigItem).c_str(), "tree of rings associated to the chain");

    t->Branch("RunNumber",        &m_runNumber);
    t->Branch("EventNumber",      &m_eventNumber);

    // likelihood
    t->Branch("el_pt",            &m_el_pt);
    t->Branch("el_eta",           &m_el_eta);
    t->Branch("el_phi",           &m_el_phi);
    t->Branch("el_nSi",                &m_el_nSi             );
    t->Branch("el_nSiDeadSensors",     &m_el_nSiDeadSensors  );
    t->Branch("el_nPix",               &m_el_nPix            );
    t->Branch("el_nSCT",               &m_el_nSCT            );
    t->Branch("el_nPixDeadSensors",    &m_el_nPixDeadSensors );
    t->Branch("el_nSCTDeadSensors",    &m_el_nSCTDeadSensors );
    t->Branch("el_expectBlayer",       &m_el_expectBlayer    );
    t->Branch("el_nBlayerHits",        &m_el_nBlayerHits     );
    t->Branch("el_nBlayerOutliers",    &m_el_nBlayerOutliers );
    t->Branch("el_d0",                 &m_el_d0              );
    t->Branch("el_deltaEta",           &m_el_deltaEta        );
    t->Branch("el_deltaPhiRescaled2",  &m_el_deltaPhiRescaled2);
    t->Branch("el_convBi",             &m_el_convBit ); // this nno longer works
    // offline answer
    t->Branch("el_loose",         &m_el_loose );
    t->Branch("el_medium",        &m_el_medium );
    t->Branch("el_tight",         &m_el_tight );
    t->Branch("el_lhLoose",       &m_el_lhLoose );
    t->Branch("el_lhMedium",      &m_el_lhMedium ); 
    t->Branch("el_lhTight",       &m_el_lhTight ); 
    t->Branch("el_multiLepton",   &m_el_multiLepton);
    t->Branch("trk_nGoodVtx",           &m_trk_nGoodVtx);
    t->Branch("trk_nPileupPrimaryVtx",  &m_trk_nPileupPrimaryVtx);
    t->Branch("calo_et",          &m_calo_et);
    t->Branch("calo_eta",         &m_calo_eta);
    t->Branch("calo_phi",         &m_calo_phi);
    // Level L1 cluster
    t->Branch( "trig_L1_emClus",       &m_trig_L1_emClus);
    t->Branch( "trig_L1_tauClus",      &m_trig_L1_tauClus);
    t->Branch( "trig_L1_emIsol",       &m_trig_L1_emIsol);
    t->Branch( "trig_L1_hadIsol",      &m_trig_L1_hadIsol);
    t->Branch( "trig_L1_thrNames",     &m_trig_L1_thrNames);
    t->Branch( "trig_L1_accept",       &m_trig_L1_accept);
    // Level L2 calo
    t->Branch( "trig_L2_calo_et",      &m_trig_L2_calo_et);
    t->Branch( "trig_L2_calo_eta",     &m_trig_L2_calo_eta);
    t->Branch( "trig_L2_calo_phi",     &m_trig_L2_calo_phi);
    t->Branch( "trig_L2_calo_e237",    &m_trig_L2_calo_e237 );
    t->Branch( "trig_L2_calo_e277",    &m_trig_L2_calo_e277 );
    t->Branch( "trig_L2_calo_fracs1",  &m_trig_L2_calo_fracs1);
    t->Branch( "trig_L2_calo_weta2",   &m_trig_L2_calo_weta2);
    t->Branch( "trig_L2_calo_ehad1",   &m_trig_L2_calo_ehad1);
    t->Branch( "trig_L2_calo_emaxs1",  &m_trig_L2_calo_emaxs1);
    t->Branch( "trig_L2_calo_e2tsts1", &m_trig_L2_calo_e2tsts1);
    t->Branch( "trig_L2_calo_wstot",   &m_trig_L2_calo_wstot);
    t->Branch( "trig_L2_calo_rings",   &m_trig_L2_calo_rings ); 
    t->Branch( "trig_L2_calo_accept",  &m_trig_L2_calo_accept);
    // Level L2 calo+ID
    t->Branch( "trig_L2_el_pt" ,          &m_trig_L2_el_pt  );
    t->Branch( "trig_L2_el_eta",          &m_trig_L2_el_eta );
    t->Branch( "trig_L2_el_phi",          &m_trig_L2_el_phi );
    t->Branch( "trig_L2_el_charge",       &m_trig_L2_el_charge );
    t->Branch( "trig_L2_el_nTRTHits",     &m_trig_L2_el_nTRTHits);
    t->Branch( "trig_L2_el_rcore" ,       &m_trig_L2_el_rcore );
    t->Branch( "trig_L2_el_eratio" ,      &m_trig_L2_el_eratio );
    t->Branch( "trig_L2_el_ethad" ,       &m_trig_L2_el_ethad );
    t->Branch( "trig_L2_el_f0" ,          &m_trig_L2_el_f0 );
    t->Branch( "trig_L2_el_f1" ,          &m_trig_L2_el_f1 );
    t->Branch( "trig_L2_el_f2" ,          &m_trig_L2_el_f2 );
    t->Branch( "trig_L2_el_f3" ,          &m_trig_L2_el_f3 );
    t->Branch( "trig_L2_el_etOverPt" ,    &m_trig_L2_el_etOverPt );
    t->Branch( "trig_L2_el_trkClusDeta" , &m_trig_L2_el_trkClusDeta );
    t->Branch( "trig_L2_el_trkClusDphi" , &m_trig_L2_el_trkClusDphi );
    t->Branch( "trig_L2_el_accept",       &m_trig_L2_el_accept );
    // level EF    
    t->Branch("trig_EF_calo_accept", &m_trig_EF_calo_accept);
    t->Branch("trig_EF_el_accept", &m_trig_EF_el_accept);
    // Monte Carlo
    t->Branch("mc_hasMC",       &m_mc_hasMC);
    t->Branch("mc_pt",          &m_mc_pt);
    t->Branch("mc_eta",         &m_mc_eta);
    t->Branch("mc_phi",         &m_mc_phi);
    t->Branch("mc_isTop",       &m_mc_isTop);
    t->Branch("mc_isParton",    &m_mc_isParton);
    t->Branch("mc_isMeson",     &m_mc_isMeson);
    t->Branch("mc_isTau",       &m_mc_isTau);
    t->Branch("mc_isMuon",      &m_mc_isMuon);
    t->Branch("mc_isPhoton",    &m_mc_isPhoton);
    t->Branch("mc_isElectron",  &m_mc_isElectron);
    t->Branch("mc_hasZMother",  &m_mc_hasZMother);
    t->Branch("mc_hasWMother",  &m_mc_hasWMother);

    addTree(t, m_dir);
  }

  // alloc memory to vector branchs
  alloc_space();
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaNavNtuple::childExecute(){

  m_eventCounter++;
  m_eventInfo = 0;

  if ( (m_storeGate->retrieve(m_eventInfo, "EventInfo")).isFailure() ){
    ATH_MSG_ERROR("Failed to retrieve eventInfo ");
    return StatusCode::FAILURE;
  }
 
  // pileup calculation
  int nGoodVtx = 0; int nPileupPrimaryVtx = 0;
  if( m_storeGate->contains<xAOD::VertexContainer>("PrimaryVertices")) {
     const xAOD::VertexContainer* vxContainer(0);
     if ( m_storeGate->retrieve(vxContainer, "PrimaryVertices").isFailure() ) {
       ATH_MSG_WARNING ("Could not retrieve xAOD::VertexContainer 'PrimaryVertices'.");
     }else{
       for(unsigned ivx = 0; ivx < vxContainer->size(); ++ivx){
         int nTrackParticles = vxContainer->at(ivx)->nTrackParticles();
         if (nTrackParticles >= 4) nGoodVtx++;
         if ( (nTrackParticles >= 4 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PriVtx) ||
            (nTrackParticles >= 2 && vxContainer->at(ivx)->vertexType() == xAOD::VxType::PileUp) )
           nPileupPrimaryVtx++;
       }// loop over vertexs
     } 
   }// protection


  // Monte Carlo information;
  m_truthContainer = 0;
  if(m_storeGate->contains<xAOD::TruthParticleContainer>("egammaTruthParticles")){
    if(m_storeGate->retrieve(m_truthContainer,"egammaTruthParticles").isFailure()){
      ATH_MSG_WARNING("Could not retrieve xAOD::TruthParticleContainer 'egammaTruthParticles'");
    }
  }// protection


  for(unsigned int ilist = 0; ilist != m_trigList.size(); ilist++) {
    std::string trigItem = m_trigList.at(ilist);
    
    if ( executeNavigation(trigItem).isFailure() )
      return StatusCode::FAILURE;

    TTree *t = tree( trigItem, m_dir);
    conect_branchs(t);
    
    for(unsigned int i = 0; i != m_electronTriggerElementList.size(); ++i){
      clear();
      const xAOD::Electron *el = m_electronTriggerElementList[i].first;
      const HLT::TriggerElement *feat = m_electronTriggerElementList[i].second;

      if(feat == NULL){
        ATH_MSG_INFO("TriggerElement is NULL");
        continue;
      }

      m_runNumber               = m_eventInfo->runNumber();
      m_eventNumber             = m_eventInfo->eventNumber();
      m_el_pt                   = el->pt()/1e3;
      m_el_eta                  = el->eta(); 
      m_el_phi                  = el->phi();
      m_el_loose                = el->passSelection("Loose");
      m_el_medium               = el->passSelection("Medium");
      m_el_tight                = el->passSelection("Tight");
      m_el_lhLoose              = el->passSelection("LHLoose");
      m_el_lhMedium             = el->passSelection("LHMedium");
      m_el_lhTight              = el->passSelection("LHTight"); 
      m_el_multiLepton          = el->passSelection("MultiLepton");
      m_calo_et                 = el->caloCluster()->et()/1e3;
      m_calo_eta                = el->caloCluster()->eta();
      m_calo_phi                = el->caloCluster()->phi();
      m_trk_nGoodVtx            = nGoodVtx;
      m_trk_nPileupPrimaryVtx   = nPileupPrimaryVtx;

      const xAOD::EmTauRoI *emTauRoI = getFeature<xAOD::EmTauRoI>(feat);
      m_trig_L1_emClus  = emTauRoI->emClus();
      m_trig_L1_tauClus = emTauRoI->tauClus();
      m_trig_L1_emIsol  = emTauRoI->emIsol();
      m_trig_L1_hadIsol = emTauRoI->hadIsol();
      for(unsigned i=0; i < emTauRoI->thrNames().size();++i)  m_trig_L1_thrNames->push_back(emTauRoI->thrNames().at(i));
      m_trig_L1_accept = ancestorPassed<xAOD::EmTauRoI>(feat);


      if(m_trig_L1_accept){   
        const xAOD::TrigEMCluster *emCluster = getFeature<xAOD::TrigEMCluster>(feat);
        if(emCluster){
    
          m_trig_L2_calo_et         = emCluster->et()/1e3;
          m_trig_L2_calo_eta        = emCluster->eta();
          m_trig_L2_calo_phi        = emCluster->phi();
          m_trig_L2_calo_e237       = emCluster->e237();
          m_trig_L2_calo_e277       = emCluster->e277();
          m_trig_L2_calo_fracs1     = emCluster->fracs1();
          m_trig_L2_calo_weta2      = emCluster->weta2();
          m_trig_L2_calo_ehad1      = emCluster->ehad1();
          m_trig_L2_calo_emaxs1     = emCluster->emaxs1();
          m_trig_L2_calo_e2tsts1    = emCluster->e2tsts1();
          m_trig_L2_calo_wstot      = emCluster->wstot();
          m_trig_L2_calo_accept     = ancestorPassed<xAOD::TrigEMCluster>(feat);
         
          if(!attach_ringer( feat )){
            ATH_MSG_WARNING("Cound not attach the ringer information into the tree.");
            if(m_doRinger)  continue;
          }
   
          // Level 2 ID+Calo
          if(m_trig_L2_calo_accept){
            const xAOD::TrigElectronContainer *trigElCont = getFeature<xAOD::TrigElectronContainer>(feat);
            if(trigElCont){
              for(unsigned i = 0; i < trigElCont->size(); ++i ){
                const xAOD::TrigElectron *trigEl = trigElCont->at(i);
                m_trig_L2_el_pt          ->push_back(trigEl->pt()); 
                m_trig_L2_el_eta         ->push_back(trigEl->eta());  
                m_trig_L2_el_phi         ->push_back(trigEl->phi());  
                m_trig_L2_el_charge      ->push_back(trigEl->charge());      
                m_trig_L2_el_nTRTHits    ->push_back(trigEl->nTRTHits());        
                m_trig_L2_el_rcore       ->push_back(trigEl->rcore());      
                m_trig_L2_el_eratio      ->push_back(trigEl->eratio());      
                m_trig_L2_el_ethad       ->push_back(trigEl->etHad());      
                m_trig_L2_el_f0          ->push_back(trigEl->f0());      
                m_trig_L2_el_f1          ->push_back(trigEl->f1());        
                m_trig_L2_el_f2          ->push_back(trigEl->f2());      
                m_trig_L2_el_f3          ->push_back(trigEl->f3());            
                m_trig_L2_el_etOverPt    ->push_back(trigEl->etOverPt());          
                m_trig_L2_el_trkClusDeta ->push_back(trigEl->trkClusDeta());
                m_trig_L2_el_trkClusDphi ->push_back(trigEl->trkClusDphi());
              }// loop over all trigElectrons for this feat
            }// protection
    
            m_trig_L2_el_accept    = ancestorPassed<xAOD::TrigElectronContainer>(feat); 
            // Level EF
            if(m_trig_L2_el_accept){
              m_trig_EF_calo_accept    = ancestorPassed<xAOD::CaloClusterContainer>(feat);
              m_trig_EF_el_accept      = ancestorPassed<xAOD::ElectronContainer>(feat);
            }// EF
          }// protection L2 Track
        }// protection L2 Calo
      }// protection L1


      if(!attach_monteCarlo( el ) ){
        ATH_MSG_WARNING("Cound not found any TruthParticle for this Electron");
      }

      if(!attach_likelihood( el )){
        ATH_MSG_WARNING("Could not attach the likelihood information.");
      }

      t->Fill();// save information
    }// loop
    //t->Write();
  } // End loop over trigger list

  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaNavNtuple::childFinalize(){

  //release_space();
  return StatusCode::SUCCESS;
}


bool TrigEgammaNavNtuple::attach_monteCarlo(const xAOD::Electron *eg){

  // find MC particle
  if(m_truthContainer){
    TLorentzVector elp; elp.SetPtEtaPhiE(eg->pt(),eg->eta(),eg->phi(),eg->e());
    for(const auto& mc : *m_truthContainer ){
      bool Zfound = false;
      bool Wfound = false;
      if(mc->isElectron()){
        size_t nParents = mc->nParents();
        for(size_t iparent = 0; iparent < nParents; ++iparent){
          if((mc->parent(iparent))->isZ()){
            Zfound = true;     
          }
          if((mc->parent(iparent))->isW()){
            Wfound = true;     
          }
        }
      }
      TLorentzVector mcp;
      mcp.SetPtEtaPhiE(mc->pt(), mc->eta(), mc->phi(), mc->e() );
      if(mcp.DeltaR(elp) < 0.07){
        m_mc_hasMC = true;
        m_mc_pt = mc->pt();
        m_mc_eta = mc->eta();
        m_mc_phi = mc->phi();
        m_mc_isTop = mc->isTop();
        m_mc_isQuark = mc->isQuark();
        m_mc_isParton = mc->isParton();
        m_mc_isMeson = mc->isMeson();
        m_mc_isTau = mc->isTau();
        m_mc_isMuon = mc->isMuon();
        m_mc_isPhoton = mc->isPhoton();
        m_mc_isElectron = mc->isElectron();
        m_mc_hasZMother = Zfound;
        m_mc_hasWMother = Wfound;
        return true;
       }// has match
    }// loop over MC
  }// has truth?
  return false;
}



bool TrigEgammaNavNtuple::attach_likelihood( const xAOD::Electron *eg ){

  if ( !eg ){
    ATH_MSG_DEBUG ("Failed, no egamma object.");
    return false;
  }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster ){
    ATH_MSG_DEBUG ("Failed, no cluster.");
    return false;
  }  
  
  const float eta = (cluster->etaBE(2)); 
  if ( fabs(eta) > 300.0 ){
    ATH_MSG_DEBUG ("Failed, eta range.");
    //return m_acceptDummy;
  }
   
  // retrieve associated track
  const xAOD::TrackParticle* t  = eg->trackParticle();    
  if (t){// protection
    m_el_d0 = t->d0();
    t->summaryValue(m_el_nPix, xAOD::numberOfPixelHits);
    t->summaryValue(m_el_nSCT, xAOD::numberOfSCTHits);
    m_el_nSi = m_el_nPix + m_el_nSCT;
    t->summaryValue(m_el_nPixDeadSensors, xAOD::numberOfPixelDeadSensors);
    t->summaryValue(m_el_nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
    m_el_nSiDeadSensors = m_el_nPixDeadSensors + m_el_nSCTDeadSensors;

    t->summaryValue(m_el_expectBlayer, xAOD::expectBLayerHit);
    t->summaryValue(m_el_nBlayerHits, xAOD::numberOfBLayerHits);
    t->summaryValue(m_el_nBlayerOutliers, xAOD::numberOfBLayerOutliers);

  }else{
    ATH_MSG_WARNING ( "Failed, no track particle: et= " << m_el_pt << "eta= " << m_el_eta );
    return false;
  }
  //if ( nSi < 4 ){ et = cluster->et(); }

  eg->trackCaloMatchValue(m_el_deltaEta, xAOD::EgammaParameters::deltaEta1);
  eg->trackCaloMatchValue(m_el_deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);

  return true;
}

bool TrigEgammaNavNtuple::attach_ringer( const HLT::TriggerElement *te ){
 
  
  const xAOD::TrigEMCluster *emCluster = getFeature<xAOD::TrigEMCluster>(te);
  if(!emCluster)  return false;

  Trig::FeatureContainer fc = (m_trigdec->features("HLT_.*",TrigDefs::alsoDeactivateTEs));
  const std::vector< Trig::Feature<xAOD::TrigRingerRings > > vec_featRinger = fc.get< xAOD::TrigRingerRings >("",TrigDefs::alsoDeactivateTEs);
  for( Trig::Feature<xAOD::TrigRingerRings > featRinger : vec_featRinger ){
    const xAOD::TrigRingerRings *ringer = featRinger.cptr();
    if(emCluster->RoIword() ==  (getFeature<xAOD::TrigEMCluster>(featRinger.te()))->RoIword() ){
      for(unsigned i = 0; i < ringer->size();++i){
        m_trig_L2_calo_rings->push_back(ringer->rings()[i]);
      } // loop over rings
      return true;
    }
  }
  return false;
}




template <class T> 
void TrigEgammaNavNtuple::InitBranch(TTree* fChain, std::string branch_name, T* param, bool message){

  std::string bname = branch_name;
  if (fChain->GetAlias(bname.c_str()))
     bname = std::string(fChain->GetAlias(bname.c_str()));

  if (!fChain->FindBranch(bname.c_str()) && message) {
    ATH_MSG_WARNING("unknown branch " << bname);
    return;  
  }
  fChain->SetBranchStatus(bname.c_str(), 1.);
  fChain->SetBranchAddress(bname.c_str(), param);
}


void TrigEgammaNavNtuple::conect_branchs(TTree *t){
 
  InitBranch(t, "RunNumber", &m_runNumber );
  InitBranch(t, "EventNumber", &m_eventNumber );
  // likelihood
  InitBranch(t, "el_pt", &m_el_pt);
  InitBranch(t, "el_eta", &m_el_eta );
  InitBranch(t, "el_phi", &m_el_phi );
  InitBranch(t, "el_nSi",                &m_el_nSi             );
  InitBranch(t, "el_nSiDeadSensors",     &m_el_nSiDeadSensors  );
  InitBranch(t, "el_nPix",               &m_el_nPix            );
  InitBranch(t, "el_nSCT",               &m_el_nSCT            );
  InitBranch(t, "el_nPixDeadSensors",    &m_el_nPixDeadSensors );
  InitBranch(t, "el_nSCTDeadSensors",    &m_el_nSCTDeadSensors );
  InitBranch(t, "el_expectBlayer",       &m_el_expectBlayer    );
  InitBranch(t, "el_nBlayerHits",        &m_el_nBlayerHits     );
  InitBranch(t, "el_nBlayerOutliers",    &m_el_nBlayerOutliers );
  InitBranch(t, "el_d0",                 &m_el_d0              );
  InitBranch(t, "el_deltaEta",           &m_el_deltaEta        );
  InitBranch(t, "el_deltaPhiRescaled2",  &m_el_deltaPhiRescaled2);
  InitBranch(t, "el_convBi",             &m_el_convBit ); // this nno longer works
  // offline 
  InitBranch(t, "el_loose", &m_el_loose );
  InitBranch(t, "el_medium", &m_el_medium );
  InitBranch(t, "el_tight", &m_el_tight );
  InitBranch(t, "el_lhLoose", &m_el_lhLoose );
  InitBranch(t, "el_lhMedium", &m_el_lhMedium );
  InitBranch(t, "el_lhTight", &m_el_lhTight );  
  InitBranch(t, "el_multiLepton", &m_el_multiLepton);
  InitBranch(t, "trk_nGoodVtx", &m_trk_nGoodVtx);
  InitBranch(t, "trk_nPileupPrimaryVtx", &m_trk_nPileupPrimaryVtx); 
  InitBranch(t, "calo_et", &m_calo_et);
  InitBranch(t, "calo_eta", &m_calo_eta);
  InitBranch(t, "calo_phi", &m_calo_phi);
  // level 1
  InitBranch(t, "trig_L1_emClus",  &m_trig_L1_emClus);
  InitBranch(t, "trig_L1_tauClus", &m_trig_L1_tauClus);
  InitBranch(t, "trig_L1_emIsol",  &m_trig_L1_emIsol);
  InitBranch(t, "trig_L1_hadIsol", &m_trig_L1_hadIsol);
  InitBranch(t, "trig_L1_thrNames",&m_trig_L1_thrNames);
  InitBranch(t, "trig_L1_accept",  &m_trig_L1_accept);
  // level 2 calo
  InitBranch(t, "trig_L2_calo_et",      &m_trig_L2_calo_et);
  InitBranch(t, "trig_L2_calo_eta",     &m_trig_L2_calo_eta);
  InitBranch(t, "trig_L2_calo_phi",     &m_trig_L2_calo_phi);
  InitBranch(t, "trig_L2_calo_e237",    &m_trig_L2_calo_e237 );
  InitBranch(t, "trig_L2_calo_e277",    &m_trig_L2_calo_e277 );
  InitBranch(t, "trig_L2_calo_fracs1",  &m_trig_L2_calo_fracs1);
  InitBranch(t, "trig_L2_calo_weta2",   &m_trig_L2_calo_weta2);
  InitBranch(t, "trig_L2_calo_ehad1",   &m_trig_L2_calo_ehad1);
  InitBranch(t, "trig_L2_calo_emaxs1",  &m_trig_L2_calo_emaxs1);
  InitBranch(t, "trig_L2_calo_e2tsts1", &m_trig_L2_calo_e2tsts1);
  InitBranch(t, "trig_L2_calo_wstot",   &m_trig_L2_calo_wstot);
  InitBranch(t, "trig_L2_calo_rings",   &m_trig_L2_calo_rings ); 
  InitBranch(t, "trig_L2_calo_accept",  &m_trig_L2_calo_accept);
  // level 2 ID
  InitBranch(t, "trig_L2_el_pt" ,          &m_trig_L2_el_pt );
  InitBranch(t, "trig_L2_el_eta",          &m_trig_L2_el_eta);
  InitBranch(t, "trig_L2_el_phi",          &m_trig_L2_el_phi );
  InitBranch(t, "trig_L2_el_charge",       &m_trig_L2_el_charge);
  InitBranch(t, "trig_L2_el_nTRTHits",     &m_trig_L2_el_nTRTHits);
  InitBranch(t, "trig_L2_el_rcore" ,       &m_trig_L2_el_rcore );
  InitBranch(t, "trig_L2_el_eratio" ,      &m_trig_L2_el_eratio );
  InitBranch(t, "trig_L2_el_ethad" ,       &m_trig_L2_el_ethad );
  InitBranch(t, "trig_L2_el_f0" ,          &m_trig_L2_el_f0 );
  InitBranch(t, "trig_L2_el_f1" ,          &m_trig_L2_el_f1 );
  InitBranch(t, "trig_L2_el_f2" ,          &m_trig_L2_el_f2 );
  InitBranch(t, "trig_L2_el_f3" ,          &m_trig_L2_el_f3 );
  InitBranch(t, "trig_L2_el_etOverPt" ,    &m_trig_L2_el_etOverPt );
  InitBranch(t, "trig_L2_el_trkClusDeta" , &m_trig_L2_el_trkClusDeta );
  InitBranch(t, "trig_L2_el_trkClusDphi" , &m_trig_L2_el_trkClusDphi );
  InitBranch(t, "trig_L2_el_accept",       &m_trig_L2_el_accept );
  // level EF
  InitBranch(t, "trig_EF_el_accept", &m_trig_EF_el_accept );
  InitBranch(t, "trig_EF_calo_accept", &m_trig_EF_calo_accept );
  // Monte Carlo
  InitBranch(t, "mc_hasMC",       &m_mc_hasMC);
  InitBranch(t, "mc_pt",          &m_mc_pt);
  InitBranch(t, "mc_eta",         &m_mc_eta);
  InitBranch(t, "mc_phi",         &m_mc_phi);
  InitBranch(t, "mc_isTop",       &m_mc_isTop);
  InitBranch(t, "mc_isParton",    &m_mc_isParton);
  InitBranch(t, "mc_isMeson",     &m_mc_isMeson);
  InitBranch(t, "mc_isTau",       &m_mc_isTau);
  InitBranch(t, "mc_isMuon",      &m_mc_isMuon);
  InitBranch(t, "mc_isPhoton",    &m_mc_isPhoton);
  InitBranch(t, "mc_isElectron",  &m_mc_isElectron);
  InitBranch(t, "mc_hasZMother",  &m_mc_hasZMother);
  InitBranch(t, "mc_hasWMother",  &m_mc_hasWMother);
 
}

void TrigEgammaNavNtuple::clear(){

  m_runNumber             =  0; 
  m_eventNumber           =  0; 
  m_el_pt                 = -1; 
  m_el_eta                = -1; 
  m_el_phi                = -1; 

  // likelihood
  m_el_nSi                = 0;
  m_el_nSiDeadSensors     = 0;
  m_el_nPix               = 0;
  m_el_nSCT               = 0;
  m_el_nPixDeadSensors    = 0;
  m_el_nSCTDeadSensors    = 0;
  m_el_expectBlayer       = true;
  m_el_nBlayerHits        = 0;
  m_el_nBlayerOutliers    = 0;
  m_el_d0                 = 0;
  m_el_deltaEta           = 0;
  m_el_deltaPhiRescaled2  = 0;
  m_el_convBit            = 0; // this no longer works
  m_el_loose              = false; 
  m_el_medium             = false; 
  m_el_tight              = false; 
  m_el_lhLoose            = false; 
  m_el_lhMedium           = false; 
  m_el_lhTight            = false;  
  m_el_multiLepton        = false;

  m_trk_nGoodVtx          = -1;
  m_trk_nPileupPrimaryVtx = -1;
  m_calo_et               = -1; 
  m_calo_eta              = -1; 
  m_calo_phi              = -1; 
  m_trig_L1_emClus        = -1;
  m_trig_L1_tauClus       = -1;
  m_trig_L1_emIsol        = -1;
  m_trig_L1_hadIsol       = -1;
  m_trig_L1_accept        = false;
  m_trig_L2_calo_et       = -1;
  m_trig_L2_calo_eta      = -1;
  m_trig_L2_calo_phi      = -1;
  m_trig_L2_calo_e237     = -1;
  m_trig_L2_calo_e277     = -1;
  m_trig_L2_calo_fracs1   = -1;
  m_trig_L2_calo_weta2    = -1;
  m_trig_L2_calo_ehad1    = -1;
  m_trig_L2_calo_emaxs1   = -1;
  m_trig_L2_calo_e2tsts1  = -1;
  m_trig_L2_calo_wstot    = -1; 
  m_trig_L2_calo_accept   = false;
  m_trig_L2_el_accept     = false; 
  m_trig_EF_calo_accept   = false;
  m_trig_EF_el_accept     = false;
  // Monte Carlo
  m_mc_hasMC              = false;
  m_mc_pt                 = -1;
  m_mc_eta                = -1;
  m_mc_phi                = -1;
  m_mc_isTop              = false;
  m_mc_isParton           = false;
  m_mc_isMeson            = false;
  m_mc_isTau              = false;
  m_mc_isMuon             = false;
  m_mc_isPhoton           = false;
  m_mc_isElectron         = false;
  m_mc_hasZMother         = false;
  m_mc_hasWMother         = false;
  // pointers
  m_trig_L1_thrNames      ->clear(); 
  m_trig_L2_calo_rings    ->clear(); 
  m_trig_L2_el_pt         ->clear();   
  m_trig_L2_el_eta        ->clear();    
  m_trig_L2_el_phi        ->clear();    
  m_trig_L2_el_charge     ->clear();        
  m_trig_L2_el_nTRTHits   ->clear();          
  m_trig_L2_el_rcore      ->clear();        
  m_trig_L2_el_eratio     ->clear();        
  m_trig_L2_el_ethad      ->clear();        
  m_trig_L2_el_f0         ->clear();        
  m_trig_L2_el_f1         ->clear();          
  m_trig_L2_el_f2         ->clear();        
  m_trig_L2_el_f3         ->clear();              
  m_trig_L2_el_etOverPt   ->clear();            
  m_trig_L2_el_trkClusDeta->clear(); 
  m_trig_L2_el_trkClusDphi->clear(); 
}

void TrigEgammaNavNtuple::alloc_space(){

  m_trig_L1_thrNames       = new std::vector<std::string>();
  m_trig_L2_calo_rings     = new std::vector<float>();
  m_trig_L2_el_pt          = new std::vector<float>();  
  m_trig_L2_el_eta         = new std::vector<float>();   
  m_trig_L2_el_phi         = new std::vector<float>();   
  m_trig_L2_el_charge      = new std::vector<float>();       
  m_trig_L2_el_nTRTHits    = new std::vector<float>();         
  m_trig_L2_el_rcore       = new std::vector<float>();       
  m_trig_L2_el_eratio      = new std::vector<float>();       
  m_trig_L2_el_ethad       = new std::vector<float>();       
  m_trig_L2_el_f0          = new std::vector<float>();       
  m_trig_L2_el_f1          = new std::vector<float>();         
  m_trig_L2_el_f2          = new std::vector<float>();       
  m_trig_L2_el_f3          = new std::vector<float>();             
  m_trig_L2_el_etOverPt    = new std::vector<float>();           
  m_trig_L2_el_trkClusDeta = new std::vector<float>();
  m_trig_L2_el_trkClusDphi = new std::vector<float>();
 
}

void TrigEgammaNavNtuple::release_space(){

  delete m_trig_L1_thrNames       ;
  delete m_trig_L2_calo_rings     ;
  delete m_trig_L2_el_pt          ;  
  delete m_trig_L2_el_eta         ;   
  delete m_trig_L2_el_phi         ;   
  delete m_trig_L2_el_charge      ;       
  delete m_trig_L2_el_nTRTHits    ;         
  delete m_trig_L2_el_rcore       ;       
  delete m_trig_L2_el_eratio      ;       
  delete m_trig_L2_el_ethad       ;       
  delete m_trig_L2_el_f0          ;       
  delete m_trig_L2_el_f1          ;         
  delete m_trig_L2_el_f2          ;       
  delete m_trig_L2_el_f3          ;             
  delete m_trig_L2_el_etOverPt    ;           
  delete m_trig_L2_el_trkClusDeta ;
  delete m_trig_L2_el_trkClusDphi ;
}


