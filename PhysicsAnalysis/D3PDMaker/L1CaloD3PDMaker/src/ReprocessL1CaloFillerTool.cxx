/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file L1CaloD3PDMaker/ReprocessL1CaloFillerTool.cxx
 * @author John Morris <john.morris@cern.ch>
 * @date May 2011
 * @brief Block filler tool for reprocessed L1Calo information.
 * @brief L1Calo settings can be changed and the entire system re-run
 * @brief from ADC values all the way to RoIs
 * @brief This D3PD algorithm records the reprocessed L1Calo output
 * @brief It is designed to be run many times per event (i.e. - test 20 different configs!)
 */

#include "AthenaKernel/errorcheck.h"
#include "ReprocessL1CaloFillerTool.h"

#include <iostream>

namespace D3PD{


  ReprocessL1CaloFillerTool::ReprocessL1CaloFillerTool (const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent)
  : BlockFillerTool<void> (type, name, parent),
    m_tools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_roiTool("LVL1::L1RoITools/L1RoITools"),
    m_JetElementTool("LVL1::L1JetElementTools/L1JetElementTools"),
    m_CpHitsTool("LVL1::L1CPHitsTools/L1CPHitsTools"),
    m_EmTauTool("LVL1::L1EmTauTools/L1EmTauTools"),
    m_JetTool("LVL1::L1JetTools/L1JetTools"),
    m_EtTool("LVL1::L1EtTools/L1EtTools"),
    m_triggerTowerLocation("TriggerTowers")
    {
      // Avoid coverity warnings.
      m_doPPM = true;
      m_doCPM = true;
      m_doJEM = true;
      m_doCMM = true;
      m_doROI = true;
      book().ignore();

  declareProperty("TriggerTowerLocation", m_triggerTowerLocation);
  declareProperty("ReprocessName",m_reprocessName);
  declareProperty("DoPPM",m_doPPM = true);
  declareProperty("DoCPM",m_doCPM = true);
  declareProperty("DoJEM",m_doJEM = false);
  declareProperty("DoCMM",m_doCMM = false);
  declareProperty("DoROI",m_doROI = true);
    }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode ReprocessL1CaloFillerTool::initialize()
  {
    CHECK( this->loadTools() );
    return StatusCode::SUCCESS;
  }

  StatusCode ReprocessL1CaloFillerTool::book()
  {
    // PPM
    if( m_doPPM == true ){
      CHECK( addVariable ("PPM_emCoolId", m_ppm_em_coolId) );
      CHECK( addVariable ("PPM_emLUT", m_ppm_em_LUT) );
      CHECK( addVariable ("PPM_emEnergy", m_ppm_em_Energy) );

      CHECK( addVariable ("PPM_hadCoolId", m_ppm_had_coolId) );
      CHECK( addVariable ("PPM_hadLUT", m_ppm_had_LUT) );
      CHECK( addVariable ("PPM_hadEnergy", m_ppm_had_Energy) );
    }

    // *********************** CPM ***********************
    if( m_doCPM == true ){
      //CPM Tower
      CHECK( addVariable ("CPM_Tower_n",m_cpmtower_n) );
      CHECK( addVariable ("CPM_Tower_emEnergy",m_cpmtower_emEnergy) );
      CHECK( addVariable ("CPM_Tower_hadEnergy",m_cpmtower_hadEnergy) );
      CHECK( addVariable ("CPM_Tower_emEnergyVec",m_cpmtower_emEnergyVec) );
      CHECK( addVariable ("CPM_Tower_hadEnergyVec",m_cpmtower_hadEnergyVec) );
      CHECK( addVariable ("CPM_Tower_peak",m_cpmtower_peak) );
      CHECK( addVariable ("CPM_Tower_emError",m_cpmtower_emError) );
      CHECK( addVariable ("CPM_Tower_hadError",m_cpmtower_hadError) );
      CHECK( addVariable ("CPM_Tower_emErrorVec",m_cpmtower_emErrorVec) );
      CHECK( addVariable ("CPM_Tower_hadErrorVec",m_cpmtower_hadErrorVec) );
      CHECK( addVariable ("CPM_Tower_eta",m_cpmtower_eta) );
      CHECK( addVariable ("CPM_Tower_l1caloPhi",m_cpmtower_l1caloPhi) );
      CHECK( addVariable ("CPM_Tower_atlasPhi",m_cpmtower_atlasPhi) );

      // CPM RoI
      CHECK( addVariable ("CPM_RoI_n",m_cpmroi_n) );
      CHECK( addVariable ("CPM_RoI_crate",m_cpmroi_crate) );
      CHECK( addVariable ("CPM_RoI_cpm",m_cpmroi_cpm) );
      CHECK( addVariable ("CPM_RoI_chip",m_cpmroi_chip) );
      CHECK( addVariable ("CPM_RoI_location",m_cpmroi_location) );
      CHECK( addVariable ("CPM_RoI_hits",m_cpmroi_hits) );
      CHECK( addVariable ("CPM_RoI_error",m_cpmroi_error) );
      CHECK( addVariable ("CPM_RoI_roiWord",m_cpmroi_roiWord) );
      CHECK( addVariable ("CPM_RoI_eta",m_cpmroi_eta) );
      CHECK( addVariable ("CPM_RoI_l1caloPhi",m_cpmroi_l1caloPhi) );
      CHECK( addVariable ("CPM_RoI_atlasPhi",m_cpmroi_atlasPhi) );
      CHECK( addVariable ("CPM_RoI_core",m_cpmroi_core) );
      CHECK( addVariable ("CPM_RoI_emClus",m_cpmroi_emClus) );
      CHECK( addVariable ("CPM_RoI_tauClus",m_cpmroi_tauClus) );
      CHECK( addVariable ("CPM_RoI_emIsol",m_cpmroi_emIsol) );
      CHECK( addVariable ("CPM_RoI_hadIsol",m_cpmroi_hadIsol) );
      CHECK( addVariable ("CPM_RoI_hadVeto",m_cpmroi_hadVeto) );
      CHECK( addVariable ("CPM_RoI_isEtMax",m_cpmroi_isEtMax) );

      // CPM Hits
      CHECK( addVariable ("CPM_Hits_n",m_cpmhits_n) );
      CHECK( addVariable ("CPM_Hits_crate",m_cpmhits_crate) );
      CHECK( addVariable ("CPM_Hits_module",m_cpmhits_module) );
      CHECK( addVariable ("CPM_Hits_peak",m_cpmhits_peak) );
      CHECK( addVariable ("CPM_Hits_hitWord0",m_cpmhits_hitWord0) );
      CHECK( addVariable ("CPM_Hits_hitWord1",m_cpmhits_hitWord1) );
      CHECK( addVariable ("CPM_Hits_hitsVec0",m_cpmhits_hitsVec0) );
      CHECK( addVariable ("CPM_Hits_hitsVec1",m_cpmhits_hitsVec1) );
    }


    // *********************** JEM ***********************
    if( m_doJEM == true ){
      // Jet Element
      CHECK( addVariable ("JEM_JetEl_n",m_jem_jetel_n) );
      CHECK( addVariable ("JEM_JetEl_eta",m_jem_jetel_eta) );
      CHECK( addVariable ("JEM_JetEl_l1caloPhi",m_jem_jetel_l1caloPhi) );
      CHECK( addVariable ("JEM_JetEl_atlasPhi",m_jem_jetel_atlasPhi) );
      CHECK( addVariable ("JEM_JetEl_emEnergy",m_jem_jetel_emEnergy) );
      CHECK( addVariable ("JEM_JetEl_hadEnergy",m_jem_jetel_hadEnergy) );
      CHECK( addVariable ("JEM_JetEl_energy",m_jem_jetel_energy) );
      CHECK( addVariable ("JEM_JetEl_isSaturated",m_jem_jetel_isSaturated) );
      CHECK( addVariable ("JEM_JetEl_isEmSaturated",m_jem_jetel_isEmSaturated) );
      CHECK( addVariable ("JEM_JetEl_isHadSaturated",m_jem_jetel_isHadSaturated) );
      CHECK( addVariable ("JEM_JetEl_emError",m_jem_jetel_emError) );
      CHECK( addVariable ("JEM_JetEl_hadError",m_jem_jetel_hadError) );
      CHECK( addVariable ("JEM_JetEl_linkError",m_jem_jetel_linkError) );
      CHECK( addVariable ("JEM_JetEl_peak",m_jem_jetel_peak) );
      CHECK( addVariable ("JEM_JetEl_emEnergyVec",m_jem_jetel_emEnergyVec) );
      CHECK( addVariable ("JEM_JetEl_hadEnergyVec",m_jem_jetel_hadEnergyVec) );
      CHECK( addVariable ("JEM_JetEl_emErrorVec",m_jem_jetel_emErrorVec) );
      CHECK( addVariable ("JEM_JetEl_hadErrorVec",m_jem_jetel_hadErrorVec) );
      CHECK( addVariable ("JEM_JetEl_linkErrorVec",m_jem_jetel_linkErrorVec) );

      // JEM Et Sums -- todo

      // JEM Hits -- todo

      // JEM RoIs -- todo

    }

     // *********************** CMM ***********************
     if( m_doCMM == true ){
       // CMMCPHits -- todo

       // CMMEtSums -- todo

       // CMMJetHits -- todo

       // CMMRoI  -- todo
     }

    // *********************** RoIs ***********************
    if( m_doROI == true ){
      // EmTau RoI
      CHECK( addVariable ("emtau_n",m_roi_emtau_n) );
      CHECK( addVariable ("emtau_RoIWord",m_roi_emtau_roiWord) );
      CHECK( addVariable ("emtau_core",m_roi_emtau_core) );
      CHECK( addVariable ("emtau_EMClus",m_roi_emtau_emClus) );
      CHECK( addVariable ("emtau_tauClus",m_roi_emtau_tauClus) );
      CHECK( addVariable ("emtau_EMIsol",m_roi_emtau_emIsol) );
      CHECK( addVariable ("emtau_hadIsol",m_roi_emtau_hadIsol) );
      CHECK( addVariable ("emtau_hadCore",m_roi_emtau_hadCore) );
      CHECK( addVariable ("emtau_eta",m_roi_emtau_eta) );
      CHECK( addVariable ("emtau_phi",m_roi_emtau_phi) );

      // Jet RoI
      CHECK( addVariable ("jet_n",m_roi_jet_n) );
      CHECK( addVariable ("jet_RoIWord",m_roi_jet_roiWord) );
      CHECK( addVariable ("jet_et4x4",m_roi_jet_et4x4) );
      CHECK( addVariable ("jet_et6x6",m_roi_jet_et6x6) );
      CHECK( addVariable ("jet_et8x8",m_roi_jet_et8x8) );
      CHECK( addVariable ("jet_eta",m_roi_jet_eta) );
      CHECK( addVariable ("jet_phi",m_roi_jet_phi) );

      // Energy RoI
      
      CHECK( addVariable ("energySum_RoIWord0",m_roi_energy_roiWord0) );
      CHECK( addVariable ("energySum_RoIWord1",m_roi_energy_roiWord1) );
      CHECK( addVariable ("energySum_RoIWord2",m_roi_energy_roiWord2) );
      CHECK( addVariable ("energySum_ExMiss",m_roi_energy_eXmiss) );
      CHECK( addVariable ("energySum_EyMiss",m_roi_energy_eYmiss) );
      CHECK( addVariable ("energySum_energyT",m_roi_energy_energyT) );
      CHECK( addVariable ("energySum_overflowX",m_roi_energy_overflowX) );
      CHECK( addVariable ("energySum_overflowY",m_roi_energy_overflowY) );
      CHECK( addVariable ("energySum_overflowT",m_roi_energy_overflowT) );

    }
    

    return StatusCode::SUCCESS;

  }

  StatusCode ReprocessL1CaloFillerTool::fill(){
    StatusCode sc;

    const TriggerTowerCollection* tts = 0;
    std::string location = m_triggerTowerLocation + "_" + m_reprocessName;

    sc = evtStore()->retrieve(tts,location);
    if(sc.isFailure()){ATH_MSG_INFO("Failed to load Trigger Towers");return sc;}


    LVL1_ROI rois;
    m_roiTool->findROIs(tts,rois);
    std::vector<EmTau_ROI> emtau_rois = rois.getEmTauROIs();
    std::vector<Jet_ROI> jet_rois = rois.getJetROIs();
    std::vector<EnergySum_ROI> energy_rois = rois.getEnergySumROIs();

    if( m_doPPM == true ){
      this->ppm(tts);
    }

    if( m_doCPM == true ){
      this->cpm(tts,emtau_rois);
    }

    if( m_doJEM == true ){
      this->jem(tts);
    }

    if( m_doROI == true ){
      this->rois_emTau(emtau_rois);
      this->rois_jet(jet_rois);
      this->rois_energy(energy_rois);
    }


    return sc;
  }

  void ReprocessL1CaloFillerTool::ppm(const TriggerTowerCollection* tts){
    typedef TriggerTowerCollection::const_iterator Itr;
    for(Itr i=tts->begin();i!=tts->end();++i){
      m_ppm_em_coolId->push_back(m_tools->emCoolChannelId(*i));
      m_ppm_em_LUT->push_back((*i)->emLUT());
      m_ppm_em_Energy->push_back((*i)->emEnergy());

      m_ppm_had_coolId->push_back(m_tools->hadCoolChannelId(*i));
      m_ppm_had_LUT->push_back((*i)->hadLUT());
      m_ppm_had_Energy->push_back((*i)->hadEnergy());
    }    
  }

  void ReprocessL1CaloFillerTool::cpm(const TriggerTowerCollection* tts,std::vector<EmTau_ROI> emtau_rois){

    // CPM Towers
    // Need a map of CPMTowers as input 
    std::map<int, LVL1::CPMTower*>* cpmtowers = new std::map<int, LVL1::CPMTower*>;
    m_EmTauTool->mapTowers(tts, cpmtowers);

    int count_towers(0);
    for(std::map<int, LVL1::CPMTower*>::iterator it = cpmtowers->begin(); it != cpmtowers->end(); ++it){
      count_towers++;

      m_cpmtower_emEnergy->push_back( (*it).second->emEnergy() );
      m_cpmtower_hadEnergy->push_back( (*it).second->hadEnergy() );
      m_cpmtower_emEnergyVec->push_back( (*it).second->emEnergyVec() );
      m_cpmtower_hadEnergyVec->push_back( (*it).second->hadEnergyVec() );
      m_cpmtower_peak->push_back( (*it).second->peak() );
      m_cpmtower_emError->push_back( (*it).second->emError() );
      m_cpmtower_hadError->push_back( (*it).second->hadError() );
      m_cpmtower_emErrorVec->push_back( (*it).second->emErrorVec() );
      m_cpmtower_hadErrorVec->push_back( (*it).second->hadErrorVec() );

      m_cpmtower_eta->push_back( (*it).second->eta() );
      m_cpmtower_l1caloPhi->push_back( (*it).second->phi() );
      m_cpmtower_atlasPhi->push_back( this->AtlasPhi( (*it).second->phi() ) );
    }
    *m_cpmtower_n = count_towers;


    
    // CPM RoIs
    typedef std::vector<EmTau_ROI>::const_iterator Itr_roi;
//     typedef std::vector<LVL1::CPMRoI>::const_iterator Itr_cpmrois;
//     std::vector<LVL1::CPMRoI>* cpmrois = new std::vector<LVL1::CPMRoI>;
//     for(Itr_roi i=emtau_rois.begin();i!=emtau_rois.end();++i){
//       LVL1::CPMRoI* roi = new LVL1::CPMRoI( i->getROIWord() );
//       cpmrois->push_back(*roi);
//     }
    typedef CPMRoICollection::const_iterator Itr_cpmrois;
    CPMRoICollection* cpmrois = new CPMRoICollection;
    for(Itr_roi i=emtau_rois.begin();i!=emtau_rois.end();++i){
      LVL1::CPMRoI* roi = new LVL1::CPMRoI( i->getROIWord() );
      cpmrois->push_back(roi);
    }

    int count_rois(0);
    for(Itr_cpmrois i=cpmrois->begin();i!=cpmrois->end();++i){
      count_rois++;
      m_cpmroi_crate->push_back( (*i)->crate() );
      m_cpmroi_cpm->push_back( (*i)->cpm() );
      m_cpmroi_chip->push_back( (*i)->chip() );
      m_cpmroi_location->push_back( (*i)->location() );
      m_cpmroi_hits->push_back( (*i)->hits() );
      m_cpmroi_error->push_back( (*i)->error() );
      m_cpmroi_roiWord->push_back( (*i)->roiWord() );

      LVL1::CPRoIDecoder decoder;
      const LVL1::CoordinateRange coord( decoder.coordinate( (*i)->roiWord() ) );

      m_cpmroi_eta->push_back( coord.eta() );
      m_cpmroi_l1caloPhi->push_back( coord.phi() );
      m_cpmroi_atlasPhi->push_back( this->AtlasPhi( coord.phi() ) );

      LVL1::CPAlgorithm simRoI = m_EmTauTool->findRoI(coord.eta(), coord.phi(), &(*cpmtowers));

      m_cpmroi_core->push_back( simRoI.Core() );
      m_cpmroi_emClus->push_back( simRoI.EMClus() );
      m_cpmroi_tauClus->push_back( simRoI.TauClus() );
      m_cpmroi_emIsol->push_back( simRoI.EMIsol() );
      m_cpmroi_hadIsol->push_back( simRoI.HadIsol() );
      m_cpmroi_hadVeto->push_back( simRoI.HadVeto() );

      if( simRoI.isEtMax() ){
        m_cpmroi_isEtMax->push_back( 1 );
      }
      else{
        m_cpmroi_isEtMax->push_back( 0 );
      }
    }
    *m_cpmroi_n = count_rois;

    this->cpmhits(cpmrois);

    // Clean up 
    delete cpmrois;
    for (std::map<int, CPMTower*>::iterator it = cpmtowers->begin(); it != cpmtowers->end(); ++it) {
      delete (*it).second;
    }
    delete cpmtowers;     

  }

  void ReprocessL1CaloFillerTool::cpmhits(const CPMRoICollection* cpmrois){

    typedef CPMHitsCollection::const_iterator Itr;
    CPMHitsCollection* cpmhits = new CPMHitsCollection;
    m_CpHitsTool->formCPMHits(cpmrois,cpmhits);

    int count_hits(0);
    for(Itr i=cpmhits->begin();i!=cpmhits->end();++i){
      count_hits++;
      m_cpmhits_crate->push_back( (*i)->crate() );
      m_cpmhits_module->push_back( (*i)->module() );
      m_cpmhits_peak->push_back( (*i)->peak() );
      m_cpmhits_hitWord0->push_back( (*i)->HitWord0() );
      m_cpmhits_hitWord1->push_back( (*i)->HitWord1() );
      m_cpmhits_hitsVec0->push_back( (*i)->HitsVec0() );
      m_cpmhits_hitsVec1->push_back( (*i)->HitsVec1() );
    }
    *m_cpmhits_n = count_hits;

    delete cpmhits;
  }


  void ReprocessL1CaloFillerTool::jem(const TriggerTowerCollection* tts){

    typedef JetElementCollection::const_iterator Itr;
    JetElementCollection* jetel = new JetElementCollection;
    m_JetElementTool->makeJetElements(tts,jetel);

    int count(0);
    for(Itr i=jetel->begin();i!=jetel->end();++i){
      count++;
      m_jem_jetel_eta->push_back( (*i)->eta() );
      m_jem_jetel_l1caloPhi->push_back( (*i)->phi() );
      m_jem_jetel_atlasPhi->push_back( this->AtlasPhi( (*i)->phi() ) );
      m_jem_jetel_emEnergy->push_back( (*i)->emEnergy() );
      m_jem_jetel_hadEnergy->push_back( (*i)->hadEnergy() );
      m_jem_jetel_energy->push_back( (*i)->energy() );
      m_jem_jetel_emError->push_back( (*i)->emError() );
      m_jem_jetel_hadError->push_back( (*i)->hadError() );
      m_jem_jetel_linkError->push_back( (*i)->linkError() );
      m_jem_jetel_peak->push_back( (*i)->peak() );
      m_jem_jetel_emEnergyVec->push_back( (*i)->emEnergyVec() );
      m_jem_jetel_hadEnergyVec->push_back( (*i)->hadEnergyVec() );
      m_jem_jetel_emErrorVec->push_back( (*i)->emErrorVec() );
      m_jem_jetel_hadErrorVec->push_back( (*i)->hadErrorVec() );
      m_jem_jetel_linkErrorVec->push_back( (*i)->linkErrorVec() );

      unsigned int sat(0),emsat(0),hadsat(0);
      if( (*i)->isSaturated() == true ){sat = 1;}
      if( (*i)->isEmSaturated() == true ){emsat = 1;}
      if( (*i)->isHadSaturated() == true ){hadsat = 1;}

      m_jem_jetel_isSaturated->push_back( sat );
      m_jem_jetel_isEmSaturated->push_back( emsat );
      m_jem_jetel_isHadSaturated->push_back( hadsat );

    }
    *m_jem_jetel_n = count;
  }

  void ReprocessL1CaloFillerTool::rois_emTau(const std::vector< EmTau_ROI > rois){
    typedef std::vector< EmTau_ROI >::const_iterator Itr;
    int count(0);
    for(Itr i=rois.begin();i!=rois.end();++i){
      count++;
      m_roi_emtau_roiWord->push_back( i->getROIWord() );
      m_roi_emtau_core->push_back( i->getCore() );
      m_roi_emtau_emClus->push_back( i->getEMClus() );
      m_roi_emtau_tauClus->push_back( i->getTauClus() );
      m_roi_emtau_emIsol->push_back( i->getEMIsol() );
      m_roi_emtau_hadIsol->push_back( i->getHadIsol() );
      m_roi_emtau_hadCore->push_back( i->getHadCore() );
      m_roi_emtau_eta->push_back( i->eta() );
      m_roi_emtau_phi->push_back( i->phi() );
    }
    *m_roi_emtau_n = count;
  }

  void ReprocessL1CaloFillerTool::rois_jet(const std::vector< Jet_ROI > rois){
    typedef std::vector< Jet_ROI >::const_iterator Itr;
    int count(0);
    for(Itr i=rois.begin();i!=rois.end();++i){
      count++;
      m_roi_jet_roiWord->push_back( i->getROIWord() );
      m_roi_jet_et4x4->push_back( i->getET4x4() );
      m_roi_jet_et6x6->push_back( i->getET6x6() );
      m_roi_jet_et8x8->push_back( i->getET8x8() );
      m_roi_jet_eta->push_back( i->eta() );
      m_roi_jet_phi->push_back( i->phi() );

    }
    *m_roi_jet_n = count;
  }

  void ReprocessL1CaloFillerTool::rois_energy(const std::vector< EnergySum_ROI > rois){
    if( rois.size() == 1){
      EnergySum_ROI roi = rois.at(0);

      *m_roi_energy_roiWord0 = roi.getROIWord0();
      *m_roi_energy_roiWord1 = roi.getROIWord1();
      *m_roi_energy_roiWord2 = roi.getROIWord2();
      *m_roi_energy_eXmiss = roi.getExMiss();
      *m_roi_energy_eYmiss = roi.getEyMiss();
      *m_roi_energy_energyT = roi.getEnergyT();

      bool overX = roi.getOverflowX();
      bool overY = roi.getOverflowY();
      bool overT = roi.getOverflowT();

      if( overX == true ){*m_roi_energy_overflowX = 1;}
      if( overX == false ){*m_roi_energy_overflowX = 0;}

      if( overY == true ){*m_roi_energy_overflowY = 1;}
      if( overY == false ){*m_roi_energy_overflowY = 0;}

      if( overT == true ){*m_roi_energy_overflowT = 1;}
      if( overT == false ){*m_roi_energy_overflowT = 0;}

    }
  }



  StatusCode ReprocessL1CaloFillerTool::loadTools(){
    StatusCode sc;

    sc = m_tools.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load L1CaloOfflineTriggerTowerTools");return sc;}

    sc = m_roiTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load ROI Tool");return sc;}

    sc = m_JetElementTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load JetElementTool");return sc;}

    sc = m_CpHitsTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load CpHitsTool");return sc;}

    sc = m_EmTauTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load EmTauTool");return sc;}

    sc = m_JetTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load JetTool");return sc;}

    sc = m_EtTool.retrieve();
    if(sc.isFailure()){ATH_MSG_ERROR("Failed to load EtTool");return sc;}

    return sc;
  }

  float ReprocessL1CaloFillerTool::AtlasPhi(const float& phi) const {
    float pi(3.14159265);
    if(phi >= 0.0 && phi < pi ){
      return phi;
    }
    if(phi >= pi){
      return phi - (2.0 * pi);
    }
    return phi;
  }  
    

} // end of namespace


      
