/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/**********************************************************************
 * AsgTool: TrigEgammaEventSelection
 * Authors:
 *      Ryan Mackenzie White <ryan.white@cern.ch>
 *      Denis Damazio <denis.damazio@cern.ch>
 * Contributors:
 *      Joao Victor da Fonseca Pinto <joao.victor.da.fonseca.pinto@cern.ch>
 * Date: Mar 2015
 * Description:
 *      Derived class for dumping the probe information into a ttree.
 *     
 **********************************************************************/

#include "TrigEgammaAnalysisTools/TrigEgammaEventSelection.h"

using namespace std;

TrigEgammaEventSelection::TrigEgammaEventSelection( const std::string& myname ): 
  TrigEgammaNavTPBaseTool(myname) , TrigEgammaPhysValBase()
{
  m_eventCounter = 0;
  declareProperty("CutLabels"                        ,      m_cutlabels               );
  declareProperty("Analysis"                         ,      m_anatype="EventSelection");
  declareProperty("SelectionZ"                       ,      m_selectionZ=true         );
  declareProperty("SelectionFakes"                   ,      m_selectionFakes=false    );
  declareProperty("SelectionMC"                      ,      m_selectionMC=false       );
  declareProperty("SelectionW"                       ,      m_selectionW=false        );
  declareProperty("SelectionJpsi"                    ,      m_selectionJpsi=false     );
  declareProperty("DumpTags"                         ,      m_dumpTags=false          );
  declareProperty("DetailedDataLevel"                ,      m_detailedDataLevel=0     );
  declareProperty("ElectronLHVLooseTool"             ,      m_electronLHVLooseTool    );

  
  declareProperty("HLTElectronLikelihoodSelector"    ,      m_HLTElectronLHSelectors  );
  declareProperty("EFCaloElectronLikelihoodSelector" , m_EFCaloElectronLHSelectors    );
  declareProperty("EFCaloElectronIsEMSelector"       , m_EFCaloElectronIsEMSelectors  );
  declareProperty("HLTElectronIsEMSelector"          , m_HLTElectronIsEMSelectors     );

  m_doCaloRings=false;
}

StatusCode TrigEgammaEventSelection::childInitialize(){

  if ( (m_electronLHVLooseTool.retrieve()).isFailure() ){
    ATH_MSG_ERROR( "Could not retrieve VeryLoose LH Selector Tool! Can't work");
    return StatusCode::FAILURE;
  }

  if ( (m_EFCaloElectronLHSelectors.retrieve()).isFailure() ){
    ATH_MSG_ERROR( "Could not retrieve EFCalo LH Selector Tool! Can't work");
    return StatusCode::FAILURE;
  }
 
  if ( (m_EFCaloElectronIsEMSelectors.retrieve()).isFailure() ){
    ATH_MSG_ERROR( "Could not retrieve EFCalo IsEM Selector Tool! Can't work");
    return StatusCode::FAILURE;
  }
  if ( (m_HLTElectronLHSelectors.retrieve()).isFailure() ){
    ATH_MSG_ERROR( "Could not retrieve HLT LH Selector Tool! Can't work");
    return StatusCode::FAILURE;
  }
  if ( (m_HLTElectronIsEMSelectors.retrieve()).isFailure() ){
    ATH_MSG_ERROR( "Could not retrieve HLT IsEM Selector Tool! Can't work");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


StatusCode TrigEgammaEventSelection::childBook(){

  ATH_MSG_DEBUG("Now configuring chains for analysis");
  //Set the base directory from the plot()
  m_dir=plot()->getBasePath();
  std::vector<std::string> chains  = tdt()->getListOfTriggers("HLT_e.*, L1_EM.*, HLT_g.*");

  TrigEgammaAnalysisBaseTool::write_trigger_list(chains);
  
  if(!getEmulation()){
    ATH_MSG_ERROR("Emulation tool was not set. This tool use the emulation to retrievea all support objects. Please, enable this!");
    return StatusCode::FAILURE;
  }


  for(const auto trigName:m_trigInputList){ 
    if (std::find(chains.begin(), chains.end(), trigName) != chains.end()){ 
      if(plot()->getTrigInfoMap().count(trigName) != 0)
        ATH_MSG_WARNING("Trigger already booked, removing from trigger list " << trigName);
      else{ 
        m_trigList.push_back(trigName);
        setTrigInfo(trigName);
      }
    }else if(getEmulation() && m_forceTrigAttachment){// 
      ATH_MSG_DEBUG("Trigger doesn't exist in menu. Attach to emulate.");
      if(plot()->getTrigInfoMap().count(trigName) == 0){
        std::string parent;
        m_trigList.push_back(trigName);
        setTrigEmulation();
        setTrigInfo(trigName);
      }
    }// emulation 
  }
 
  // set the correct schema level to save
  /* 0 is very slim schema
   * 1 is slim
   * 2 is full */
  if(m_detailedDataLevel > 0 ){
    m_doL2Detailed=true;
    m_doL2CaloDetailed=true;
  }
  if(m_detailedDataLevel > 1){
    m_doRecoDetailed=true;
    m_doMCDetailed=true;
  }


  // Book histograms for average efficiencies and counters
  //const int nTrigger = (int) m_trigList.size();
  addDirectory(m_dir+"/Expert/Event");
  std::string histname=m_anatype+"_ProbeCutCounter";
  addHistogram(new TH1F(histname.c_str(), "Number of Probes; Cut ; Count", 12, 0., 12));
  histname=m_anatype+"_TagCutCounter";
  addHistogram(new TH1F(histname.c_str(), "Number of Tags; Cut ; Count", 10, 0., 10));
  histname=m_anatype+"_Mee";
  addHistogram(new TH1F(histname.c_str(), "Offline M(ee); m_ee [GeV] ; Count", 50, m_ZeeMassMin, m_ZeeMassMax));
  histname=m_anatype+"_CutCounter";
  addHistogram(new TH1I(histname.c_str(), "Event Selection; Cut ; Count", 6, 0., 6));
  setLabels(hist1(histname),m_cutlabels);


  addDirectory(m_dir+"/Egamma");

  // create the TDT metadata for future access
  TTree *tdtMetadata = new TTree("tdt", "Trigger  metadata navigation");
  createTDTMetadata( tdtMetadata, m_trigList );
  addTree(tdtMetadata, m_dir+"/Egamma");

  if(m_selectionZ){
      if(m_dumpTags){
        TTree *tag = new TTree( "tags", "tree of Offline Zee tags");
        bookEventBranches( tag );
        bookElectronBranches( tag );
        bookTriggerBranches( tag );
        bookTDTBranches( tag );
        bookMonteCarloBranches( tag );
        addTree(tag, m_dir+"/Egamma");
      }
      TTree *probe = new TTree( "probes", "tree of Zee probes");
      bookEventBranches( probe );
      bookElectronBranches( probe );
      bookTriggerBranches( probe );
      bookTDTBranches( probe );
      bookMonteCarloBranches( probe );

      addTree(probe, m_dir+"/Egamma");
  }

  if(m_selectionFakes){
      TTree *fakes = new TTree( "fakes", "tree of Fakes");
      bookEventBranches( fakes );
      bookElectronBranches( fakes );
      bookTriggerBranches( fakes );
      bookTDTBranches( fakes );
      bookMonteCarloBranches( fakes );
      addTree(fakes, m_dir+"/Egamma");
  }



  // Check for Rnn container in SG
  if(m_storeGate->contains<xAOD::TrigRNNOutputContainer>("HLT_xAOD__TrigRNNOutputContainer_TrigRingerNeuralFex")){
    ATH_MSG_DEBUG("Rnn container in SG "); 
    setSGContainsRnn(true);
  }


  if(m_storeGate->contains<xAOD::CaloRingsContainer>("ElectronCaloRings")){
    ATH_MSG_DEBUG("Offline electron calo rings in SG");
    m_doCaloRings=true;
  }

  // This will needed to emulate objects without all features attached
  //if(emulation())
  //  emulation()->ExperimentalAndExpertMethods();

  ///Alloc pointers
  alloc_space();
  return StatusCode::SUCCESS;
}

StatusCode TrigEgammaEventSelection::childExecute(){

  m_eventCounter++;

  cd(m_dir+"/Expert/Event");
  
  // Event Wise Selection (independent of the required signatures)
  if ( !TrigEgammaNavTPBaseTool::EventWiseSelection() ) return StatusCode::SUCCESS;

  // Pass by errors in EventInfo
  if( !TrigEgammaNavTPBaseTool::passesEventCleaning() ) return StatusCode::SUCCESS;

  // nvtx > 0
  if( getNPVtx() <= 0 )  return StatusCode::SUCCESS;
  
  if(m_selectionZ)  
    EventSelectionZ();

  if(m_selectionFakes)
    EventSelectionFakes();

  //if(m_selectionMC)
  //  EventSelectionMC();
  

  return StatusCode::SUCCESS;
}

bool TrigEgammaEventSelection::EventSelectionZ(){

  ATH_MSG_INFO("In EventSelectonZ...");
  // execute the standard tag and probe method
  TrigEgammaNavTPBaseTool::executeTandP();
  cd(m_dir+"/Egamma");
 
  for(auto Z : m_Z){
    const xAOD::Electron *elTag   = m_offElectrons->at(Z.first);
    const xAOD::Electron *elProbe = m_offElectrons->at(Z.second); 
    if(m_dumpTags)
      fill( tree("tags"  , m_dir+"/Egamma")  , elTag   );
    fill( tree("probes", m_dir+"/Egamma")  , elProbe );

  }// Loop over pairs

  return true;
}

bool TrigEgammaEventSelection::EventSelectionFakes(){

  
  ATH_MSG_INFO("In EventSelectonFakes...");
  double GeV = 1000.;

  for(const auto& elTag : *m_offElectrons){ 

    // Remove electrons from Z or W if this is Monte Carlo
    if(m_selectionMC && !isTruthElectronAny( elTag ))  continue; 

    if (!elTag->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) continue;

    if(!elTag->trackParticle()) continue;

    // Default is 4 GeV
    if( !(elTag->e()/cosh(elTag->trackParticle()->eta())  > m_probeMinEt*GeV) ) continue;
    
    //fiducial detector acceptance region
    float absEta = fabs(elTag->caloCluster()->etaBE(2));
    if (absEta > 2.47) continue;
     
    if( !(getWTransverseMass(elTag) < 40000) )  continue;

    bool passesZveto=true;

    // If we can find any pair that falls in the Zmass window we fail the passesZveto cut
    for(const auto& elProbe : *m_offElectrons){

      if(elTag == elProbe)  continue;

      if(!elProbe->isGoodOQ(xAOD::EgammaParameters::BADCLUSELECTRON)) continue;
      
      if(!elProbe->trackParticle()) continue;
      
      // Must be higher than 4 GeV
      if( (getEt(elProbe)) < 4*GeV)  continue;

      if(!TrigEgammaNavTPBaseTool::ApplyElectronPid(elProbe,"LHMedium")) continue;

      //Must be an easy way with IParticle
      TLorentzVector el1;
      TLorentzVector el2;
      el1.SetPtEtaPhiE(elTag->pt(), elTag->trackParticle()->eta(), elTag->trackParticle()->phi(), elTag->e());
      el2.SetPtEtaPhiE(elProbe->pt(), elProbe->trackParticle()->eta(), elProbe->trackParticle()->phi(), elProbe->e());
      float tpPairMass = (el1 + el2).M();
      if( !((tpPairMass > m_ZeeMassMin*1.e3) && (tpPairMass < m_ZeeMassMax*1.e3))){
          ATH_MSG_DEBUG("tag and probe pair not in Z mass window");
          continue;
      }else{
        ATH_MSG_INFO("tag and probe pain in Z mass window. Regect the electron...");
        passesZveto=false;
        break;
      }
    }// Loop over probes electrons

    if(!passesZveto)  continue;

    ATH_MSG_INFO("Appling Fire trigger...");
    if(!ApplyFireTriggers( elTag ))  continue;

    if(TrigEgammaNavTPBaseTool::ApplyElectronPid(elTag,"LHTight")) continue;
    
    ATH_MSG_INFO("Fill electron...");
    fill( tree("fakes"  , m_dir+"/Egamma")  , elTag   );


  }// Loop over electrons

  return true;
}



bool TrigEgammaEventSelection::fill( TTree *t, const xAOD::Electron *el){
 

  linkEventBranches(t); 
  linkElectronBranches(t); 
  linkTriggerBranches(t); 
  linkMonteCarloBranches(t); 

  const HLT::TriggerElement* feat=nullptr;

  //bool doAccept=false;
  //if(trigItem.empty()){
  // Use the support match method from the emulation tool
  if(getEmulation())
    emulation()->match( el,  feat );
  else{
    ATH_MSG_ERROR("Emulation tool was not configurated. Impossible to match! Please, see your python config.");
    return false;
  }
  //}else{
  //  match()->match(el, trigItem, feat);
  //  doAccept=true;
  //}

  if(feat){
    clear();
    fillEvent();
    fillElectron( el );  

    if(!fillMonteCarlo( el ) ){
      ATH_MSG_WARNING("Cound not found any TruthParticle for this Electron");
    }
 
    ///Start trigger analisys...
    const xAOD::EmTauRoI *emTauRoI = getFeature<xAOD::EmTauRoI>(feat);
    if(emTauRoI){
      fillEmTauRoI( emTauRoI ); 
    }else{ 
      ATH_MSG_WARNING("Cound not found EmTauRoI in this TriggerElement..");
    }
  
    const xAOD::TrigEMCluster *emCluster = getFeature<xAOD::TrigEMCluster>(feat);
    if(emCluster){
      if(!fillTrigEMCluster( emCluster )){
        ATH_MSG_WARNING("Cound not attach the trigEMCluster information into the tree.");
      }

      if(!fillTrigCaloRings( feat )){
        ATH_MSG_WARNING("Cound not attach the TrigRingerRings information into the tree.");
      }
    }///cluster protection
  
  
    const xAOD::TrigElectronContainer *trigElCont = getFeature<xAOD::TrigElectronContainer>(feat);
    // Level 2 ID+Calo
    if(trigElCont){
      for(const auto& trigEl : *trigElCont){
        if(!fillTrigElectron(trigEl)) {
          ATH_MSG_WARNING("Cound not attach the trigElectron information into the tree.");
        }    
      }// loop over all trigElectrons for this feat
    }
     
    const xAOD::CaloClusterContainer *caloCont = getFeature<xAOD::CaloClusterContainer>(feat);
    if(caloCont){
      for(const auto& cl : *caloCont){
        if(!fillCaloCluster(cl)){
          ATH_MSG_WARNING("Cound not attach the CaloCluster information into the tree.");
        }
      }// loop over calo cluster
    }


    const xAOD::ElectronContainer *elCont = getFeature<xAOD::ElectronContainer>(feat);

    if(elCont){
      for(const auto& hlt_el : *elCont){
        if(!fillHLTElectron(hlt_el))
          ATH_MSG_WARNING("Cound not attach the HLT Electron information into the tree.");
      }
    }


    fillTDT( el, feat );


    ATH_MSG_INFO("recording trigger information into the file.");
    t->Fill();
  }//loop over trigger objects

  return true;
}


bool TrigEgammaEventSelection::fillTDT(const xAOD::Electron *el , const HLT::TriggerElement *te){

  ATH_MSG_DEBUG("FillTDT...");
  for(auto& trigItem : m_trigList){

    const HLT::TriggerElement* feat=nullptr;
    match()->match(el, trigItem, feat);
    if(feat){
      asg::AcceptData acceptData = setAccept(feat, getTrigInfo(trigItem));
      bool passedL1Calo  = acceptData.getCutResult("L1Calo");
      bool passedL2Calo  = acceptData.getCutResult("L2Calo");
      bool passedL2      = acceptData.getCutResult("L2");
      bool passedEFCalo  = acceptData.getCutResult("EFCalo");
      bool passedHLT     = acceptData.getCutResult("HLT");
      m_trig_tdt_L1_calo_accept->push_back( int(passedL1Calo) );
      m_trig_tdt_L2_calo_accept->push_back( int(passedL2Calo) );
      m_trig_tdt_L2_el_accept->push_back( int(passedL2) );
      m_trig_tdt_EF_calo_accept->push_back( int(passedEFCalo) );
      m_trig_tdt_EF_el_accept->push_back( int(passedHLT) );
      //m_trig_tdt_emu_decision_mask
      count("tdt_"+trigItem+"_total");
      if(passedL1Calo)  count("tdt_"+trigItem+"_L1Calo");
      if(passedL2Calo)  count("tdt_"+trigItem+"_L2Calo");
      if(passedL2)      count("tdt_"+trigItem+"_L2"    );
      if(passedEFCalo)  count("tdt_"+trigItem+"_EFCalo");
      if(passedHLT)     count("tdt_"+trigItem+"_HLT"   );
 
    }else{
      m_trig_tdt_L1_calo_accept->push_back( -1 );
      m_trig_tdt_L2_calo_accept->push_back( -1 );
      m_trig_tdt_L2_el_accept->push_back( -1 );
      m_trig_tdt_EF_calo_accept->push_back( -1 );
      m_trig_tdt_EF_el_accept->push_back( -1 );
      //ATH_MSG_DEBUG("Trigger bitmask was converted to " << int(GetByteFromBools(mask)));
      //m_trig_tdt_decision_mask->push_back(GetByteFromBools(mask));
    }

    if(te && getEmulation()){
      asg::AcceptData acceptData  = emulation()->executeTool(te, trigItem);
      bool passedL1Calo  = acceptData.getCutResult("L1Calo");
      bool passedL2Calo  = acceptData.getCutResult("L2Calo");
      bool passedL2      = acceptData.getCutResult("L2");
      bool passedEFCalo  = acceptData.getCutResult("EFCalo");
      bool passedHLT     = acceptData.getCutResult("HLT");
      m_trig_tdt_emu_L1_calo_accept->push_back( int(passedL1Calo) );
      m_trig_tdt_emu_L2_calo_accept->push_back( int(passedL2Calo) );
      m_trig_tdt_emu_L2_el_accept->push_back(   int(passedL2) );
      m_trig_tdt_emu_EF_calo_accept->push_back( int(passedEFCalo) );
      m_trig_tdt_emu_EF_el_accept->push_back(   int(passedHLT) );
      //m_trig_tdt_emu_decision_mask
      count("emu_"+trigItem+"_total");
      if(passedL1Calo)  count("emu_"+trigItem+"_L1Calo");
      if(passedL2Calo)  count("emu_"+trigItem+"_L2Calo");
      if(passedL2)      count("emu_"+trigItem+"_L2"    );
      if(passedEFCalo)  count("emu_"+trigItem+"_EFCalo");
      if(passedHLT)     count("emu_"+trigItem+"_HLT"   );
    }else{
      m_trig_tdt_emu_L1_calo_accept->push_back( -1 );
      m_trig_tdt_emu_L2_calo_accept->push_back( -1 );
      m_trig_tdt_emu_L2_el_accept->push_back( -1 );
      m_trig_tdt_emu_EF_calo_accept->push_back( -1 );
      m_trig_tdt_emu_EF_el_accept->push_back( -1 );
      //ATH_MSG_DEBUG("Trigger bitmask was converted to " << int(GetByteFromBools(mask)));
      //m_trig_tdt_decision_mask->push_back(GetByteFromBools(mask));
    }


  }// Loop over triggers


  return true;

}



bool TrigEgammaEventSelection::fillTrigCaloRings( const HLT::TriggerElement *feat ){
  
  ATH_MSG_DEBUG("FillTrigCaloRings...");
  /* try to get the calo rings energy for this TE!*/
  const xAOD::TrigRingerRings *ringer = getFeature<xAOD::TrigRingerRings>(feat);
  if(ringer){// from this TE
    for(unsigned r=0; r<ringer->size(); ++r){
      m_trig_L2_calo_rings->push_back(ringer->rings()[r]);
    }
  }else{// from others TEs
    const xAOD::TrigEMCluster *emCluster = getFeature<xAOD::TrigEMCluster>(feat);
    if(emCluster){
      // try to match without any other cluster.
      TrigEgammaAnalysisBaseTool::getTrigCaloRings(emCluster, *m_trig_L2_calo_rings);
    }else{
      return false;
    }
  }
 
  if(getSGContainsRnn()){
    /* try to retrieve the NN output from this TE*/
    const xAOD::TrigRNNOutput *rnn = getFeature<xAOD::TrigRNNOutput>(feat);
    if(rnn){
      for(unsigned o=0; o<rnn->rnnDecision().size(); ++o)
        m_trig_L2_calo_rnnOutput->push_back(rnn->rnnDecision()[o]);
    }else{
      ATH_MSG_WARNING("Cound not attach the TrigRNNOutput information into the tree.");
    }
  }
  return true;
}




bool TrigEgammaEventSelection::ApplyFireTriggers( const xAOD::Electron *el){

  float GeV = 1000.0;
  float et = getEt(el);

  std::string trigger="";
  
  if(et < 5*GeV){
    return false;
  }else if(et >= 5*GeV && et < 10*GeV){
    trigger = "HLT_e5_etcut";
  }else if(et >= 10*GeV && et < 15*GeV){
    trigger = "HLT_e10_etcut_L1EM7";
  }else if(et >= 15*GeV && et < 20*GeV){
    trigger = "HLT_e15_etcut_L1EM7";
  }else if(et >= 20*GeV && et < 25*GeV){
    trigger = "HLT_e20_etcut_L1EM12";
  }else if(et >= 25*GeV && et < 30*GeV){
    trigger = "HLT_e25_etcut_L1EM15";
  }else if(et >= 30*GeV && et < 40*GeV){
    trigger = "HLT_e30_etcut_L1EM15";
  }else if(et >= 40*GeV && et < 50*GeV){
    trigger = "HLT_e40_etcut_L1EM15";
  }else if(et >= 50*GeV && et < 60*GeV){
    trigger = "HLT_e50_etcut_L1EM15";
  }else if(et >= 50*GeV && et < 60*GeV){
    trigger = "HLT_e60_etcut";
  }else if(et >= 60*GeV && et < 70*GeV){
    trigger = "HLT_e70_etcut";
  }else if(et >= 70*GeV && et < 80*GeV){
    trigger = "HLT_e80_etcut";
  }else if(et >= 80*GeV && et < 100*GeV){
    trigger = "HLT_e100_etcut";
  }else if(et >= 100*GeV && et < 120*GeV){
    trigger = "HLT_e120_etcut";
  }else{
    return true;
  }
 
  const HLT::TriggerElement *te;
  match()->match(el,trigger,te);
  bool passedHLT = ancestorPassed<xAOD::ElectronContainer>(te);
  
  if(!passedHLT)  return false;

  return true;
}



StatusCode TrigEgammaEventSelection::childFinalize(){
  //release_space();
  //ATH_MSG_DEBUG( "Number of probes collected is: " <<tree("probes"  , m_dir+"/Egamma")->GetEntries());
  
  for(auto& trigItem : m_trigList){
    ATH_MSG_DEBUG("From TDT tool:");
    ATH_MSG_DEBUG(trigItem << " total  = " << m_countMap["tdt_"+trigItem+"_total"] );
    ATH_MSG_DEBUG(trigItem << " L1Calo = " << m_countMap["tdt_"+trigItem+"_L1Calo"]);
    ATH_MSG_DEBUG(trigItem << " L2Calo = " << m_countMap["tdt_"+trigItem+"_L2Calo"]);
    ATH_MSG_DEBUG(trigItem << " L2     = " << m_countMap["tdt_"+trigItem+"_L2"]    );
    ATH_MSG_DEBUG(trigItem << " EFCalo = " << m_countMap["tdt_"+trigItem+"_EFCalo"]);
    ATH_MSG_DEBUG(trigItem << " HLT    = " << m_countMap["tdt_"+trigItem+"_HLT"]   );
    ATH_MSG_DEBUG("From emulation tool:");
    ATH_MSG_DEBUG(trigItem << " total  = " << m_countMap["emu_"+trigItem+"_total"] );
    ATH_MSG_DEBUG(trigItem << " L1Calo = " << m_countMap["emu_"+trigItem+"_L1Calo"]);
    ATH_MSG_DEBUG(trigItem << " L2Calo = " << m_countMap["emu_"+trigItem+"_L2Calo"]);
    ATH_MSG_DEBUG(trigItem << " L2     = " << m_countMap["emu_"+trigItem+"_L2"]    );
    ATH_MSG_DEBUG(trigItem << " EFCalo = " << m_countMap["emu_"+trigItem+"_EFCalo"]);
    ATH_MSG_DEBUG(trigItem << " HLT    = " << m_countMap["emu_"+trigItem+"_HLT"]   );
  }
  return StatusCode::SUCCESS;
}


bool TrigEgammaEventSelection::fillEmTauRoI( const xAOD::EmTauRoI *emTauRoI ){
  
  ATH_MSG_DEBUG("FillEmTauRoI...");
  m_trig_L1_eta     = emTauRoI->eta();
  m_trig_L1_phi     = emTauRoI->phi();
  m_trig_L1_emClus  = emTauRoI->emClus();
  m_trig_L1_tauClus = emTauRoI->tauClus();
  m_trig_L1_emIsol  = emTauRoI->emIsol();
  m_trig_L1_hadIsol = emTauRoI->hadIsol();
  m_trig_L1_hadCore = emTauRoI->hadCore();
  //for(unsigned i=0; i < emTauRoI->thrNames().size();++i){
  //  m_trig_L1_thrNames->push_back(emTauRoI->thrNames().at(i));
  //}
  return true;
} 

bool TrigEgammaEventSelection::fillTrigEMCluster( const xAOD::TrigEMCluster *emCluster ){
  
  ATH_MSG_DEBUG("FillTrigEMCluster...");
  m_trig_L2_calo_et         = emCluster->et();
  m_trig_L2_calo_eta        = emCluster->eta();
  m_trig_L2_calo_phi        = emCluster->phi();
  m_trig_L2_calo_emaxs1     = emCluster->emaxs1();
  m_trig_L2_calo_e2tsts1    = emCluster->e2tsts1();
  m_trig_L2_calo_e237       = emCluster->e237();
  m_trig_L2_calo_e277       = emCluster->e277();
  m_trig_L2_calo_ehad1      = emCluster->ehad1();
  m_trig_L2_calo_weta2      = emCluster->weta2();
  m_trig_L2_calo_wstot      = emCluster->wstot();
  m_trig_L2_calo_fracs1     = emCluster->fracs1();
  for(unsigned i=0; i<emCluster->energySample().size(); ++i){
    m_trig_L2_calo_energySample->push_back( emCluster->energySample().at(i));
  }

  return true;
}


bool TrigEgammaEventSelection::fillTrigElectron( const xAOD::TrigElectron *trigEl ){
   ATH_MSG_DEBUG("FillTrigElectron...");
   //const xAOD::TrackParticle* trkIter = trigEl->trackParticle();
   //if (trkIter==nullptr) return false; // disconsider candidates without track
   //int algoId = 0;
   //if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::FastTrackFinderSeed] ) algoId=9;
   //if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyA] )           algoId=5;
   //if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyB] )           algoId=6;
   //if ( trkIter->patternRecoInfo()[xAOD::TrackPatternRecoInfo::strategyC] )           algoId=7;

   //m_trig_L2_el_trackAlgID  ->push_back(algoId);
   m_trig_L2_el_caloEta     ->push_back(trigEl->caloEta()); 
   m_trig_L2_el_trkClusDphi ->push_back(trigEl->trkClusDphi());  
   m_trig_L2_el_trkClusDeta ->push_back(trigEl->trkClusDeta());  
   m_trig_L2_el_pt          ->push_back(trigEl->pt());
   m_trig_L2_el_phi         ->push_back(trigEl->phi());  
   m_trig_L2_el_eta         ->push_back(trigEl->eta());  
   m_trig_L2_el_etOverPt    ->push_back(trigEl->etOverPt());          
   m_trig_L2_el_nTRTHits    ->push_back(trigEl->nTRTHits());        
   m_trig_L2_el_nTRTHiThresholdHits->push_back(trigEl->nTRTHiThresholdHits());
   m_trig_L2_el_charge      ->push_back(trigEl->charge());      
   return true;
}


bool TrigEgammaEventSelection::fillCaloCluster( const xAOD::CaloCluster *cluster){

  ATH_MSG_DEBUG("FillCaloCluster...");
  m_trig_EF_calo_et->push_back(cluster->et());
  m_trig_EF_calo_eta->push_back(cluster->eta());
  m_trig_EF_calo_phi->push_back(cluster->phi());
  m_trig_EF_calo_etaBE2->push_back(cluster->etaBE(2));
  m_trig_EF_calo_e->push_back(cluster->e());

  return true;
}

bool TrigEgammaEventSelection::fillEvent(){
  ///Event information
  ATH_MSG_DEBUG("FillEvent...");
  m_runNumber               = m_eventInfo->runNumber();
  m_eventNumber             = m_eventInfo->eventNumber();
  m_lumiBlock               = m_eventInfo->lumiBlock();
  m_avgmu=getAvgMu();
  return true;
}

bool TrigEgammaEventSelection::fillElectron( const xAOD::Electron *el ){

  ATH_MSG_DEBUG("FillElectron...");
  ///Cluster information
  uint8_t nblayerhits(0);
  uint8_t nblayerolhits(0);
  uint8_t npixhits(0);
  uint8_t npixolhits(0);
  uint8_t nscthits(0);
  uint8_t nsctolhits(0);
  uint8_t ntrthighthreshits(0);
  uint8_t ntrthighthresolhits(0);
  uint8_t ntrthits(0);
  uint8_t ntrtolhits(0);
  uint8_t ntrtxenonhits(0);
  uint8_t npixdeadsensors(0);
  uint8_t nsctdeadsensors(0);
  uint8_t expectblayerhit(true);
  uint8_t expectNextToInnerMostLayer(true);
  uint8_t nNextToInnerMostLayerHits(0);
  uint8_t nNextToInnerMostLayerOutliers(0);
  float val(0);
  bool hasCalo(false);
  bool hasTrack(false);


  
  m_el_e                    = el->e();
  m_el_et                   = el->pt();
  m_el_eta                  = el->eta();
  m_el_phi                  = el->phi();

  el->showerShapeValue( m_el_ethad1   , xAOD::EgammaParameters::ShowerShapeType::ethad1   ); 
  el->showerShapeValue( m_el_ehad1    , xAOD::EgammaParameters::ShowerShapeType::ehad1    );
  el->showerShapeValue( m_el_f1       , xAOD::EgammaParameters::ShowerShapeType::f1       ); // LH
  el->showerShapeValue( m_el_f3       , xAOD::EgammaParameters::ShowerShapeType::f3       ); // LH
  el->showerShapeValue( m_el_f1core   , xAOD::EgammaParameters::ShowerShapeType::f1core   );
  el->showerShapeValue( m_el_f3core   , xAOD::EgammaParameters::ShowerShapeType::f3core   );
  el->showerShapeValue( m_el_weta1    , xAOD::EgammaParameters::ShowerShapeType::weta1    ); // LH (new)
  el->showerShapeValue( m_el_weta2    , xAOD::EgammaParameters::ShowerShapeType::weta2    ); // LH
  el->showerShapeValue( m_el_wtots1   , xAOD::EgammaParameters::ShowerShapeType::wtots1   );
  el->showerShapeValue( m_el_fracs1   , xAOD::EgammaParameters::ShowerShapeType::fracs1   );
  el->showerShapeValue( m_el_Reta     , xAOD::EgammaParameters::ShowerShapeType::Reta     ); // LH
  el->showerShapeValue( m_el_Rphi     , xAOD::EgammaParameters::ShowerShapeType::Rphi     ); // LH
  el->showerShapeValue( m_el_Eratio   , xAOD::EgammaParameters::ShowerShapeType::Eratio   ); // LH
  el->showerShapeValue( m_el_Rhad     , xAOD::EgammaParameters::ShowerShapeType::Rhad     ); // LH
  el->showerShapeValue( m_el_Rhad1    , xAOD::EgammaParameters::ShowerShapeType::Rhad1    ); // LH
  el->showerShapeValue( m_el_deltaE   , xAOD::EgammaParameters::ShowerShapeType::DeltaE   ); // LH
  el->showerShapeValue( m_el_e277     , xAOD::EgammaParameters::ShowerShapeType::e277     ); // LH
  ///Combined track/Cluter information
  el->trackCaloMatchValue( m_el_deta2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaEta2   );
  el->trackCaloMatchValue( m_el_dphi2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhi2   );
  el->trackCaloMatchValue( m_el_dphiresc  , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhiRescaled0 );
  m_el_deltaPhiRescaled2  =  get_el_deltaPhiRescaled2(el); // LH
  m_el_deltaEta1          =  get_el_deltaEta1(el); // LH
 
  if(el->caloCluster()){
    hasCalo=true;
    m_el_calo_et                 = getCluster_et( el );
    m_el_calo_eta                = getCluster_eta( el );
    m_el_calo_phi                = getCluster_phi( el );
    m_el_calo_etaBE2             = el->caloCluster()->etaBE(2); // LH
    m_el_calo_e                  = el->caloCluster()->e(); // LH
  }
 
  ///Track particle information
  const xAOD::TrackParticle *track = el->trackParticle();
  if(track){
    hasTrack=true;
    track->summaryValue( nblayerhits                  , xAOD::SummaryType::numberOfBLayerHits );
    track->summaryValue( nblayerolhits                , xAOD::SummaryType::numberOfBLayerOutliers );
    track->summaryValue( npixhits                     , xAOD::SummaryType::numberOfPixelHits );
    track->summaryValue( npixolhits                   , xAOD::SummaryType::numberOfPixelOutliers );
    track->summaryValue( npixdeadsensors              , xAOD::SummaryType::numberOfPixelDeadSensors);
    track->summaryValue( nscthits                     , xAOD::SummaryType::numberOfSCTHits );
    track->summaryValue( nsctolhits                   , xAOD::SummaryType::numberOfSCTOutliers );
    track->summaryValue( nsctdeadsensors              , xAOD::SummaryType::numberOfSCTDeadSensors);
    track->summaryValue( ntrthits                     , xAOD::SummaryType::numberOfTRTHits);
    track->summaryValue( ntrtolhits                   , xAOD::SummaryType::numberOfTRTOutliers );
    track->summaryValue( ntrthighthreshits            , xAOD::SummaryType::numberOfTRTHighThresholdHits );
    track->summaryValue( ntrthighthresolhits          , xAOD::SummaryType::numberOfTRTHighThresholdOutliers );
    track->summaryValue( ntrtxenonhits                , xAOD::SummaryType::numberOfTRTXenonHits );
    track->summaryValue( expectblayerhit              , xAOD::SummaryType::expectBLayerHit );
    track->summaryValue( expectNextToInnerMostLayer   , xAOD::expectNextToInnermostPixelLayerHit);
    track->summaryValue( nNextToInnerMostLayerHits    , xAOD::numberOfNextToInnermostPixelLayerHits);
    track->summaryValue( nNextToInnerMostLayerOutliers, xAOD::numberOfNextToInnermostPixelLayerOutliers);
    
    m_el_trk_pt                 =  track->pt();
    m_el_trk_eta                =  track->eta();
    m_el_trk_charge             =  track->charge(); // LH 
    m_el_trk_qOverP             =  track->qOverP(); // LH
    m_el_trk_sigd0              =  get_el_sigd0(el); // LH
    m_el_trk_d0                 =  get_el_d0(el); // LH
    m_el_trk_eProbabilityHT     =  get_el_eProbabilityHT(el); // LH
    m_el_trk_transformed_eProbabilityHT=get_el_transformed_eProbabilityHT(el); // LH
    m_el_trk_d0significance     =  get_el_d0significance(el);
    m_el_trk_deltaPOverP        =  get_el_DeltaPOverP(el); // LH

  }

  ///Extra information about pileup
  m_el_nGoodVtx             = getNGoodVertex();
  m_el_nPileupPrimaryVtx    = getNPVtx();

  auto ctx = Gaudi::Hive::currentContext() ;
  // rerun Likelihood Veryloose selector tool
  bool accept=(bool)m_electronLHVLooseTool->accept(ctx,el, getAvgMu());
  m_el_lhVLoose             = accept;
  m_el_loose                = ApplyElectronPid(el, "Loose");
  m_el_medium               = ApplyElectronPid(el, "Medium");
  m_el_tight                = ApplyElectronPid(el, "Tight");
  m_el_lhLoose              = ApplyElectronPid(el, "LHLoose");
  m_el_lhMedium             = ApplyElectronPid(el, "LHMedium");
  m_el_lhTight              = ApplyElectronPid(el, "LHTight"); 
 

  // Do not change this order!
  m_el_trk_summaryValues->push_back( nblayerhits                     ); 
  m_el_trk_summaryValues->push_back( nblayerolhits                   ); 
  m_el_trk_summaryValues->push_back( npixhits                        ); 
  m_el_trk_summaryValues->push_back( npixolhits                      ); 
  m_el_trk_summaryValues->push_back( npixdeadsensors                 ); 
  m_el_trk_summaryValues->push_back( nscthits                        ); 
  m_el_trk_summaryValues->push_back( nsctolhits                      ); 
  m_el_trk_summaryValues->push_back( nsctdeadsensors                 ); 
  m_el_trk_summaryValues->push_back( ntrthits                        ); 
  m_el_trk_summaryValues->push_back( ntrtolhits                      ); 
  m_el_trk_summaryValues->push_back( ntrthighthreshits               ); 
  m_el_trk_summaryValues->push_back( ntrthighthresolhits             ); 
  m_el_trk_summaryValues->push_back( ntrtxenonhits                   ); 
  m_el_trk_summaryValues->push_back( expectblayerhit                 ); 
  m_el_trk_summaryValues->push_back( expectNextToInnerMostLayer      ); 
  m_el_trk_summaryValues->push_back( nNextToInnerMostLayerHits       ); 
  m_el_trk_summaryValues->push_back( nNextToInnerMostLayerOutliers   );

  //Add isolations values
  el->isolationValue(val,xAOD::Iso::etcone20);
  m_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone30);
  m_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone40);
  m_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone20);
  m_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone30);
  m_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone40);
  m_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone20);
  m_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone30);
  m_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone40);
  m_el_ptCone->push_back(val);

  m_el_hasCalo=hasCalo;
  m_el_hasTrack=hasTrack;


  if(m_doCaloRings){
    ATH_MSG_DEBUG("Fill Electron calo rings...");
    if(!TrigEgammaAnalysisBaseTool::getCaloRings( el , *m_el_ringsE)){
      ATH_MSG_WARNING("Could not attach the calorRings information.");
    }
  }



  return true;
}


bool TrigEgammaEventSelection::fillHLTElectron( const xAOD::Electron *el ){

  ATH_MSG_DEBUG("FillHLTElectron...");
  float   calo_e(0);
  float   calo_et(0);
  float   calo_eta(0);
  float   calo_phi(0);
  float   calo_etaBE2(0);
  float   eta(0);
  float   et(0);
  //float   eta2(0);
  float   phi(0);
  float   pt(0);
  float   trk_eta(0);
  float   ethad1(0);
  float   ehad1(0);
  float   f1(0);
  float   f3(0);
  float   f1core(0);
  float   f3core(0);
  float   weta1(0);
  float   weta2(0);
  float   wtots1(0);
  float   fracs1(0);
  float   Reta(0);
  float   Rphi(0);
  float   Eratio(0);
  float   Rhad(0);
  float   Rhad1(0);
  float   deta1(0);
  float   deta2(0);
  float   dphi2(0);
  float   dphiresc(0);
  float   charge(0);
  float   qOverP(0);
  float   sigd0(0);
  float   d0(0);
  //float   eprobht(0);
  float   eProbabilityHT(0);
  float   transformed_eProbabilityHT(0);
  float   d0significance(0);
  float   deltaPhiRescaled2(0);
  float   deltaPOverP(0);
  float   e(0);
  float   e277(0);
  float   deltaE(0);
  uint8_t nblayerhits(0);
  uint8_t nblayerolhits(0);
  uint8_t npixhits(0);
  uint8_t npixolhits(0);
  uint8_t nscthits(0);
  uint8_t nsctolhits(0);
  uint8_t ntrthighthreshits(0);
  uint8_t ntrthighthresolhits(0);
  uint8_t ntrthits(0);
  uint8_t ntrtolhits(0);
  uint8_t ntrtxenonhits(0);
  uint8_t npixdeadsensors(0);
  uint8_t nsctdeadsensors(0);
  uint8_t expectblayerhit(true);
  uint8_t expectNextToInnerMostLayer(true);
  uint8_t nNextToInnerMostLayerHits(0);
  uint8_t nNextToInnerMostLayerOutliers(0);
  float   val(0) ; 

  bool hasCalo(false);
  bool hasTrack(false);


  ATH_MSG_DEBUG("FillElectron...");
  
  ///Cluster information
  e                    = el->e();
  et                   = el->pt();
  eta                  = el->eta();
  phi                  = el->phi();

  el->showerShapeValue( ethad1   , xAOD::EgammaParameters::ShowerShapeType::ethad1   ); 
  el->showerShapeValue( ehad1    , xAOD::EgammaParameters::ShowerShapeType::ehad1    );
  el->showerShapeValue( f1       , xAOD::EgammaParameters::ShowerShapeType::f1       ); // LH, isEM
  el->showerShapeValue( f3       , xAOD::EgammaParameters::ShowerShapeType::f3       ); // LH, isEM
  el->showerShapeValue( f1core   , xAOD::EgammaParameters::ShowerShapeType::f1core   );
  el->showerShapeValue( f3core   , xAOD::EgammaParameters::ShowerShapeType::f3core   ); // isEM
  el->showerShapeValue( weta1    , xAOD::EgammaParameters::ShowerShapeType::weta1    ); // LH, isEM (new)
  el->showerShapeValue( weta2    , xAOD::EgammaParameters::ShowerShapeType::weta2    ); // LH, isEM
  el->showerShapeValue( wtots1   , xAOD::EgammaParameters::ShowerShapeType::wtots1   ); // LH, isEM
  el->showerShapeValue( fracs1   , xAOD::EgammaParameters::ShowerShapeType::fracs1   ); // isEM
  el->showerShapeValue( Reta     , xAOD::EgammaParameters::ShowerShapeType::Reta     ); // LH, isEM
  el->showerShapeValue( Rphi     , xAOD::EgammaParameters::ShowerShapeType::Rphi     ); // LH, isEM
  el->showerShapeValue( Eratio   , xAOD::EgammaParameters::ShowerShapeType::Eratio   ); // LH, isEM
  el->showerShapeValue( Rhad     , xAOD::EgammaParameters::ShowerShapeType::Rhad     ); // LH, isEM
  el->showerShapeValue( Rhad1    , xAOD::EgammaParameters::ShowerShapeType::Rhad1    ); // LH, isEM
  el->showerShapeValue( e277     , xAOD::EgammaParameters::e277                      ); // isEM (new)
  el->showerShapeValue( deltaE   , xAOD::EgammaParameters::DeltaE                    ); // isEM (new)

  ///Combined track/Cluter information
  el->trackCaloMatchValue( deta2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaEta2          );
  el->trackCaloMatchValue( dphi2     , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhi2          );
  el->trackCaloMatchValue( dphiresc  , xAOD::EgammaParameters::TrackCaloMatchType::deltaPhiRescaled0  );
  deltaPhiRescaled2  =  get_el_deltaPhiRescaled2(el); // LH
  deta1              =  get_el_deltaEta1(el); // LH
 
  if(el->caloCluster()){
    hasCalo=true;
    calo_et                 = getCluster_et( el );
    calo_eta                = getCluster_eta( el );
    calo_phi                = getCluster_phi( el );
    calo_etaBE2             = el->caloCluster()->etaBE(2); // LH
    calo_e                  = el->caloCluster()->e(); // LH
  }
 


  ///Track particle information
  const xAOD::TrackParticle *track = el->trackParticle();
  if(track){
    hasTrack=true;
    // track->summaryValue( eprobht                      , xAOD::SummaryType::eProbabilityHT ); // LH
    track->summaryValue( nblayerhits                  , xAOD::SummaryType::numberOfBLayerHits ); // LH, isEM
    track->summaryValue( nblayerolhits                , xAOD::SummaryType::numberOfBLayerOutliers ); // LH, isEM
    track->summaryValue( npixhits                     , xAOD::SummaryType::numberOfPixelHits ); // LH, isEM
    track->summaryValue( npixolhits                   , xAOD::SummaryType::numberOfPixelOutliers ); // isEM
    track->summaryValue( npixdeadsensors              , xAOD::SummaryType::numberOfPixelDeadSensors); // LH, isEM
    track->summaryValue( nscthits                     , xAOD::SummaryType::numberOfSCTHits ); // LH, isEM
    track->summaryValue( nsctolhits                   , xAOD::SummaryType::numberOfSCTOutliers ); // isEM
    track->summaryValue( nsctdeadsensors              , xAOD::SummaryType::numberOfSCTDeadSensors); // isEM
    track->summaryValue( ntrthits                     , xAOD::SummaryType::numberOfTRTHits); // isEM
    track->summaryValue( ntrtolhits                   , xAOD::SummaryType::numberOfTRTOutliers ); // isEM
    track->summaryValue( ntrthighthreshits            , xAOD::SummaryType::numberOfTRTHighThresholdHits ); // isEM
    track->summaryValue( ntrthighthresolhits          , xAOD::SummaryType::numberOfTRTHighThresholdOutliers ); // isEM
    track->summaryValue( ntrtxenonhits                , xAOD::SummaryType::numberOfTRTXenonHits ); // isEM
    track->summaryValue( expectblayerhit              , xAOD::SummaryType::expectBLayerHit ); // LH, isEM
    track->summaryValue( expectNextToInnerMostLayer   , xAOD::expectNextToInnermostPixelLayerHit); // LH, isEM
    track->summaryValue( nNextToInnerMostLayerHits    , xAOD::numberOfNextToInnermostPixelLayerHits); // LH, isEM
    track->summaryValue( nNextToInnerMostLayerOutliers, xAOD::numberOfNextToInnermostPixelLayerOutliers); // LH, isEM
   
    pt                 =  track->pt();
    trk_eta            =  track->eta(); // LH
    charge             =  track->charge(); // LH
    qOverP             =  track->qOverP();  // LH, isEM
    sigd0              =  get_el_sigd0(el); // LH
    d0                 =  get_el_d0(el); // LH, isEM
    eProbabilityHT     =  get_el_eProbabilityHT(el); // LH, isEM
    transformed_eProbabilityHT=get_el_transformed_eProbabilityHT(el); // LH
    d0significance     =  get_el_d0significance(el); 
    deltaPOverP        =  get_el_DeltaPOverP(el); // LH  
  }

  auto avgmu=getAvgMu();

  m_trig_EF_el_hasCalo                      ->push_back( hasCalo ); 
  m_trig_EF_el_hasTrack                     ->push_back( hasTrack );

  auto ctx  = Gaudi::Hive::currentContext();
  m_trig_EF_calo_tight                      ->push_back((bool)(m_EFCaloElectronIsEMSelectors[0]->accept(ctx,el)));
  m_trig_EF_calo_medium                     ->push_back((bool)(m_EFCaloElectronIsEMSelectors[1]->accept(ctx,el)));
  m_trig_EF_calo_loose                      ->push_back((bool)(m_EFCaloElectronIsEMSelectors[2]->accept(ctx,el)));
  m_trig_EF_calo_lhtight                    ->push_back((bool)(m_EFCaloElectronLHSelectors[0]->accept(ctx,el,avgmu)));
  m_trig_EF_calo_lhmedium                   ->push_back((bool)(m_EFCaloElectronLHSelectors[1]->accept(ctx,el,avgmu)));
  m_trig_EF_calo_lhloose                    ->push_back((bool)(m_EFCaloElectronLHSelectors[2]->accept(ctx,el,avgmu)));
  m_trig_EF_calo_lhvloose                   ->push_back((bool)(m_EFCaloElectronLHSelectors[3]->accept(ctx,el,avgmu)));
  
  m_trig_EF_el_tight                        ->push_back((bool)(m_HLTElectronIsEMSelectors[0]->accept(ctx,el)));
  m_trig_EF_el_medium                       ->push_back((bool)(m_HLTElectronIsEMSelectors[1]->accept(ctx,el)));
  m_trig_EF_el_loose                        ->push_back((bool)(m_HLTElectronIsEMSelectors[2]->accept(ctx,el)));
  m_trig_EF_el_lhtight                      ->push_back((bool)(m_HLTElectronLHSelectors[0]->accept(ctx,el,avgmu)));
  m_trig_EF_el_lhmedium                     ->push_back((bool)(m_HLTElectronLHSelectors[1]->accept(ctx,el,avgmu)));
  m_trig_EF_el_lhloose                      ->push_back((bool)(m_HLTElectronLHSelectors[2]->accept(ctx,el,avgmu)));
  m_trig_EF_el_lhvloose                     ->push_back((bool)(m_HLTElectronLHSelectors[3]->accept(ctx,el,avgmu)));
  
  m_trig_EF_el_calo_et                      ->push_back( calo_et); 
  m_trig_EF_el_calo_eta                     ->push_back( calo_eta ); 
  m_trig_EF_el_calo_phi                     ->push_back( calo_phi ); 
  m_trig_EF_el_calo_etaBE2                  ->push_back( calo_etaBE2 ); 
  m_trig_EF_el_calo_e                       ->push_back( calo_e ); 
  
  m_trig_EF_el_et                           ->push_back( et );
  m_trig_EF_el_eta                          ->push_back( eta );
  m_trig_EF_el_phi                          ->push_back( phi );
  m_trig_EF_el_e                            ->push_back( e );
  m_trig_EF_el_e277                         ->push_back( e277 );
  m_trig_EF_el_deltaE                       ->push_back( deltaE );
  m_trig_EF_el_ethad1                       ->push_back( ethad1 );
  m_trig_EF_el_ehad1                        ->push_back( ehad1 );
  m_trig_EF_el_f1                           ->push_back( f1 );
  m_trig_EF_el_f3                           ->push_back( f3 );
  m_trig_EF_el_f1core                       ->push_back( f1core );
  m_trig_EF_el_f3core                       ->push_back( f3core );
  m_trig_EF_el_weta1                        ->push_back( weta1 );
  m_trig_EF_el_weta2                        ->push_back( weta2 );
  m_trig_EF_el_wtots1                       ->push_back( wtots1 );
  m_trig_EF_el_fracs1                       ->push_back( fracs1 );
  m_trig_EF_el_Reta                         ->push_back( Reta );
  m_trig_EF_el_Rphi                         ->push_back( Rphi );
  m_trig_EF_el_Eratio                       ->push_back( Eratio );
  m_trig_EF_el_Rhad                         ->push_back( Rhad );
  m_trig_EF_el_Rhad1                        ->push_back( Rhad1 );
  m_trig_EF_el_deta2                        ->push_back( deta2 );
  m_trig_EF_el_dphi2                        ->push_back( dphi2 );
  m_trig_EF_el_dphiresc                     ->push_back( dphiresc );
  m_trig_EF_el_deltaEta1                    ->push_back( deta1 );
  m_trig_EF_el_deltaPhiRescaled2            ->push_back( deltaPhiRescaled2 );
  
  m_trig_EF_el_trk_pt                           ->push_back( pt );
  m_trig_EF_el_trk_eta                          ->push_back( trk_eta );
  m_trig_EF_el_trk_charge                       ->push_back( charge );
  m_trig_EF_el_trk_qOverP                       ->push_back( qOverP );
  m_trig_EF_el_trk_sigd0                        ->push_back( sigd0 );
  m_trig_EF_el_trk_d0                           ->push_back( d0 );
  m_trig_EF_el_trk_eProbabilityHT               ->push_back( eProbabilityHT );
  m_trig_EF_el_trk_transformed_eProbabilityHT   ->push_back( transformed_eProbabilityHT );
  m_trig_EF_el_trk_d0significance               ->push_back( d0significance );
  m_trig_EF_el_trk_deltaPOverP                  ->push_back( deltaPOverP );
    

  m_trig_EF_el_trk_summaryValues->push_back( nblayerhits                     ); 
  m_trig_EF_el_trk_summaryValues->push_back( nblayerolhits                   ); 
  m_trig_EF_el_trk_summaryValues->push_back( npixhits                        ); 
  m_trig_EF_el_trk_summaryValues->push_back( npixolhits                      ); 
  m_trig_EF_el_trk_summaryValues->push_back( npixdeadsensors                 ); 
  m_trig_EF_el_trk_summaryValues->push_back( nscthits                        ); 
  m_trig_EF_el_trk_summaryValues->push_back( nsctolhits                      ); 
  m_trig_EF_el_trk_summaryValues->push_back( nsctdeadsensors                 ); 
  m_trig_EF_el_trk_summaryValues->push_back( ntrthits                        ); 
  m_trig_EF_el_trk_summaryValues->push_back( ntrtolhits                      ); 
  m_trig_EF_el_trk_summaryValues->push_back( ntrthighthreshits               ); 
  m_trig_EF_el_trk_summaryValues->push_back( ntrthighthresolhits             ); 
  m_trig_EF_el_trk_summaryValues->push_back( ntrtxenonhits                   ); 
  m_trig_EF_el_trk_summaryValues->push_back( expectblayerhit                 ); 
  m_trig_EF_el_trk_summaryValues->push_back( expectNextToInnerMostLayer      ); 
  m_trig_EF_el_trk_summaryValues->push_back( nNextToInnerMostLayerHits       ); 
  m_trig_EF_el_trk_summaryValues->push_back( nNextToInnerMostLayerOutliers   );

   //Add isolations values
  el->isolationValue(val,xAOD::Iso::etcone20);
  m_trig_EF_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone30);
  m_trig_EF_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::etcone40);
  m_trig_EF_el_etCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone20);
  m_trig_EF_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone30);
  m_trig_EF_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptcone40);
  m_trig_EF_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone20);
  m_trig_EF_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone30);
  m_trig_EF_el_ptCone->push_back(val);
  el->isolationValue(val,xAOD::Iso::ptvarcone40);
  m_trig_EF_el_ptCone->push_back(val);

 
  
    
  /*
  m_trig_EF_el_nblayerhits                  ->push_back( nblayerhits ); 
  m_trig_EF_el_nblayerolhits                ->push_back( nblayerolhits ); 
  m_trig_EF_el_npixhits                     ->push_back( npixhits ); 
  m_trig_EF_el_npixolhits                   ->push_back( npixolhits ); 
  m_trig_EF_el_npixdeadsensors              ->push_back( npixdeadsensors ); 
  m_trig_EF_el_nscthits                     ->push_back( nscthits ); 
  m_trig_EF_el_nsctolhits                   ->push_back( nsctolhits ); 
  m_trig_EF_el_nsctdeadsensors              ->push_back( nsctdeadsensors ); 
  m_trig_EF_el_ntrthits                     ->push_back( ntrthits ); 
  m_trig_EF_el_ntrtolhits                   ->push_back( ntrtolhits ); 
  m_trig_EF_el_ntrthightreshits             ->push_back( ntrthighthreshits ); 
  m_trig_EF_el_ntrthighthresolhits          ->push_back( ntrthighthresolhits ); 
  m_trig_EF_el_ntrtxenonhits                ->push_back( ntrtxenonhits ); 
  m_trig_EF_el_expectblayerhit              ->push_back( expectblayerhit ); 
  m_trig_EF_el_expectNextToInnerMostLayer   ->push_back( expectNextToInnerMostLayer); 
  m_trig_EF_el_nNextToInnerMostLayerHits    ->push_back( nNextToInnerMostLayerHits ); 
  m_trig_EF_el_nNextToInnerMostLayerOutliers->push_back( nNextToInnerMostLayerOutliers );
  */




  return true;
}



// if true: is Z or W Or not MC   if false: not electron or not Z||W mother
bool TrigEgammaEventSelection::isTruthElectronAny(const xAOD::Egamma *eg ){
  if(m_truthContainer){
    const xAOD::TruthParticle *mc=nullptr;
    MonteCarlo::PDGID pdg = pdgid(eg, m_truthContainer,mc);
    if(mc){
      bool Z,W,e;
      Z=W=e=false;
      if(pdg == MonteCarlo::PDGID::ZMother)  Z = true;
      if(pdg == MonteCarlo::PDGID::WMother)  W = true;
      if(mc->isElectron()) e=true;
      return !(e && (Z||W));
    }
  }
  return true;
}


bool TrigEgammaEventSelection::fillMonteCarlo(const xAOD::Egamma *eg){

  if(m_truthContainer){
    const xAOD::TruthParticle *mc=nullptr;
    MonteCarlo::PDGID pdg = pdgid(eg, m_truthContainer,mc);
    if(mc){
      m_mc_hasMC        = true;
      m_mc_pt           = mc->pt();
      m_mc_eta          = mc->eta();
      m_mc_phi          = mc->phi();
      m_mc_isTop        = mc->isTop();
      m_mc_isQuark      = mc->isQuark();
      m_mc_isParton     = mc->isParton();
      m_mc_isMeson      = mc->isMeson();
      m_mc_isTau        = mc->isTau();
      m_mc_isMuon       = mc->isMuon();
      m_mc_isPhoton     = mc->isPhoton();
      m_mc_isElectron   = mc->isElectron();
      if(pdg == MonteCarlo::PDGID::ZMother)  m_mc_hasZMother   = true;
      if(pdg == MonteCarlo::PDGID::WMother)  m_mc_hasWMother   = true;
      return true;
    }//has match
  }//has truth container
  return false;
}


bool TrigEgammaEventSelection::fillCaloRings( const xAOD::Electron *el ){
  return TrigEgammaAnalysisBaseTool::getCaloRings(el, *m_el_ringsE );
}


//! ============================================================================================================

  
double TrigEgammaEventSelection::get_el_sigd0(const xAOD::Electron *el){
  float vard0 = el->trackParticle()->definingParametersCovMatrix()(0,0); // Cov matrix in vector form
  if(vard0>0.0)
    return sqrt(vard0); // variance of d0
  return 0.0; 
}

double TrigEgammaEventSelection::get_el_d0(const xAOD::Electron *el){
 return el->trackParticle()->d0();
}


double TrigEgammaEventSelection::get_el_eProbabilityHT(const xAOD::Electron *el){
 float TRT_PID = 0;
 el->trackParticle()->summaryValue(TRT_PID, xAOD::eProbabilityHT);
 return TRT_PID;
}


double TrigEgammaEventSelection::get_el_transformed_eProbabilityHT(const xAOD::Electron *el){
 double TRT_PID = get_el_eProbabilityHT(el);
 double epsilon = 1e-30;
 if(TRT_PID >= 1.0) TRT_PID = 1.0 - 1.e-15;
 else if(TRT_PID <= epsilon) TRT_PID = epsilon;
 double tau = 15.0;
 TRT_PID = -(1/tau) * log( (1.0/TRT_PID) - 1.0);
 return TRT_PID;
}


double TrigEgammaEventSelection::get_el_d0significance(const xAOD::Electron *el){
 double d0 = get_el_d0(el);
 double sigd0 = get_el_sigd0(el);
 return (sigd0 != 0) ? fabs(d0/sigd0) : 0;
}

double TrigEgammaEventSelection::get_el_deltaPhiRescaled2(const xAOD::Electron *el){
 float deltaPhiRescaled2 = 0;
 el->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2);
 return deltaPhiRescaled2;
}


double TrigEgammaEventSelection::get_el_deltaEta1(const xAOD::Electron *el){
 float deltaEta1 = 0;
 el->trackCaloMatchValue(deltaEta1, xAOD::EgammaParameters::deltaEta1);
 return deltaEta1;
}


double TrigEgammaEventSelection::get_el_DeltaPOverP(const xAOD::Electron *el){
 unsigned int index;
 if(el->trackParticle()->indexOfParameterAtPosition(index,xAOD::LastMeasurement)){
   double LMPX = el->trackParticle()->parameterPX(index);
   double LMPY = el->trackParticle()->parameterPY(index);
   double LMPZ = el->trackParticle()->parameterPZ(index);
   double refittedTrack_LMP = sqrt(LMPX*LMPX + LMPY*LMPY + LMPZ*LMPZ);
   double refittedTrack_LMqOverP = el->trackParticle()->charge() / refittedTrack_LMP;
   double unrefittedTrack_qOverP = el->trackParticle()->qOverP();
   return 1 - (unrefittedTrack_qOverP / refittedTrack_LMqOverP);
 }
 else
   return 0.0;
}


