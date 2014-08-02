/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigEgammaValidation/NavRingerValidation.h"
#include "TrigEgammaValidation/FEXValidationAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigCaloEvent/TrigEMCluster.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaContainer.h"
#include "GeneratorObjects/McEventCollection.h"
//Trigger Element
#include "GeneratorObjects/McEventCollection.h"
#include "TrigNavigation/NavigationCore.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"

#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTTriggerElement.h"
#include "TrigConfHLTData/HLTSignature.h"

#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMTrackMatch.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

#include "TrigSteering/Signature.h"
#include "TrigSteering/Sequence.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1F.h"
#include "TMath.h"

using namespace HLT;
using namespace std;

//Temporary!!
const double m_lowEtCut = 18;
const double m_maxEtaCut = 2.5;

NavRingerValidation::NavRingerValidation(const std::string & name, ISvcLocator * pSvcLocator) 
   : NavAuxValidation(name, pSvcLocator) , m_trigdec("Trig::TrigDecisionTool/TrigDecisionTool"),
   fHLTFrame(0)
{
	declareProperty("TrigDecisionTool", m_trigdec, "iTool to access the trigger decision");
	declareProperty("Chains", vec_chain);
	declareProperty("IsemMask", m_isemMask = 0xFFFFFFFF);
	declareProperty("CheckPhotons", m_checkPhotons = true);
	declareProperty("CheckElectrons", m_checkElectrons = true);
	error = 0.4;
}

NavRingerValidation::~NavRingerValidation() {
}

StatusCode NavRingerValidation::HLTValidation(const std::string &chain, float eta, float phi, float et, string lvl, int step, vector <TriggerElement *> &vec_TEs, vector < vector <ConfSeedTE> > &outputTEs) {
  int lvl_int;
  std::stringstream realStep;
  std::vector <TriggerElement *>::iterator TEs_itr;
  const TriggerElement * matchedTE = 0;
  std::vector <ConfSeedTE>::iterator itr;

  if (vec_TEs.size() == 0)
    throw "HLTValidation: vector of TEs is empty";
  if (outputTEs.size() == 0)
    throw "HLTValidation: vetor of outputTEs is empty";

  if (lvl == "L2_")
    lvl_int = 0;
  else
    lvl_int = 1;

  realStep << step + 1;

  for (TEs_itr = vec_TEs.begin(); TEs_itr != vec_TEs.end(); TEs_itr++) {
    for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3 + step).end(); itr++) {
      if (getTELabel(*TEs_itr) == (*itr).m_te->name()) {
        if ((*itr).m_found == false) {
          (*itr).m_found = true;
          matchedTE = const_cast <TriggerElement *>(*TEs_itr); 
          break;
        }
      }
    }
  }

  bool stepPassed = true;
  for (itr = outputTEs.at(lvl_int*3 + step).begin(); itr != outputTEs.at(lvl_int*3+ step).end(); itr++) {
    if ((*itr).m_found == false)
      stepPassed = false;
  }
  if (!(matchedTE->getActiveState())) 
    stepPassed = false;

  if (stepPassed) {
    cd(name() + "/" + chain + "/PreEfficiencies");
    hist1("TE_" + lvl + "Step" + realStep.str() + "_Et")->Fill(et);
    hist1("TE_" + lvl + "Step" + realStep.str() + "_Eta")->Fill(eta);
    hist1("TE_" + lvl + "Step" + realStep.str() + "_Phi")->Fill(phi);

    if (step < 2) {
      std::vector <TriggerElement *> nextTEs;
      nextTEs.clear();
      nextTEs = m_nav->getDirectSuccessors(matchedTE);

      StatusCode sc = HLTValidation(chain, eta, phi, et, lvl, step + 1, nextTEs, outputTEs);
      if (sc.isFailure())
        return sc;
    } else {
      std::vector <TriggerElement *> nextTEs;
      nextTEs.clear();
      nextTEs = m_nav->getDirectSuccessors(matchedTE);
      if (lvl == "L2_") {
        StatusCode sc = HLTValidation(chain, eta, phi, et, "EF_", 0, nextTEs, outputTEs);
        if (sc.isFailure())
          return sc;
      }
    }
  } //if stepPassed

  return StatusCode::SUCCESS;
}

StatusCode NavRingerValidation::NavigationMatch(const std::string &chain, float eta, float phi, float et) {
  float errorEta, errorPhi;
  bool foundTE;

  std::vector <SeedTE> chainTEs;
  std::vector <SeedTE>::iterator TEs_itr;
  std::vector < std::vector <ConfSeedTE> > outputTEs;
  std::vector <ConfSeedTE>::iterator itr;
  const TrigRoiDescriptor *matchedRoid = 0;
  const TriggerElement *matchedTE = 0;
  std::vector <const TrigRoiDescriptor*> trigRoiDescriptorVec;

  errorEta = errorPhi = error;

  const TriggerElement *inode = m_nav->getInitialNode();
  const std::vector<TriggerElement *> suc_initial = m_nav->getDirectSuccessors(inode); // Initial Node's successor is the Initial RoI
  
  foundTE = false;
  chainTEs.clear();

  // Finding TEs for each chain
  const TrigConf::HLTChainList& vec_chains = fHLTFrame->getHLTChainList();
  if (vec_chains.size() == 0)
    throw "NavigationMatch: vector of chains is empty";

  for (TrigConf::HLTChainList::const_iterator confChain = vec_chains.begin(); confChain != vec_chains.end(); ++confChain) {
    for (unsigned lvl = 0; lvl < 2; lvl++) {
      string compare_chain;
      if (lvl == 0)
        compare_chain = "L2_" + chain;
      else
        compare_chain = "EF_" + chain;

      if ((*confChain)->chain_name() == compare_chain) {
        std::vector <TrigConf::HLTSignature *> vec_sigs = (*confChain)->signatureList();
        if (vec_sigs.size() == 0)
          throw "NavigationMatch: Vector of signatures is empty";

        for (unsigned step = 0; step < 3; step++) {
          for (unsigned sigs = 0; sigs < vec_sigs.size(); sigs++) {
            outputTEs.push_back(std::vector<ConfSeedTE>());
            if (vec_sigs.at(sigs)->signature_counter() == (step + 1)) {
              std::vector <TrigConf::HLTTriggerElement *> sig_TEs = vec_sigs.at(sigs)->outputTEs();
              if (sig_TEs.size() == 0)
                throw ("NavigationMatch: vector of outputTEs from " + compare_chain + " is empty");

              for (unsigned i = 0; i < sig_TEs.size(); i++)
                outputTEs.at(lvl*3 + step).push_back(ConfSeedTE (sig_TEs.at(i), false));

            } // if found signature
          } //for vec_sigs
        } //for step
      } //if found chain
    } //for lvl
  } //for vec_chains

  for (int rois = 0; rois != (int) suc_initial.size(); rois++) {
    TriggerElement *roi = (suc_initial.at(rois)); // Initial Roi

    const vector<TriggerElement *> TEs_l1 = m_nav->getDirectSuccessors(roi);
    for (vector<TriggerElement *>::const_iterator l1_itr = TEs_l1.begin(); l1_itr != TEs_l1.end(); l1_itr++) {
      const vector<TriggerElement *> L2Step1 = m_nav->getDirectSuccessors(*l1_itr);

      for (vector<TriggerElement *>::const_iterator L2Step1_itr = L2Step1.begin(); L2Step1_itr != L2Step1.end(); L2Step1_itr++) {
        for (itr = outputTEs.at(0).begin(); itr != outputTEs.at(0).end(); itr++) {
          if ((*itr).m_te->name() == getTELabel(*L2Step1_itr)) {
            if ((*itr).m_found == false) {
              chainTEs.push_back(SeedTE (*l1_itr, *L2Step1_itr, rois));
              (*itr).m_found = true;
              foundTE = true;
              break;
            } 
          } //if Found matching TE
        } //for outputTEs
      } //for L2Step1
    } // for L1
  } //for rois

  if (foundTE) {
    const string label = "initialRoI";

    for (TEs_itr = chainTEs.begin(); TEs_itr != chainTEs.end(); TEs_itr++) {
      if (!const_cast<NavigationCore *>(m_nav)->getRecentFeatures((*TEs_itr).m_teL1, trigRoiDescriptorVec, label))
        throw ("An error ocurred while getting the features from the TriggerElement \"" + getTELabel((*TEs_itr).m_teL1) + "\"");

      if (trigRoiDescriptorVec.size() == 0)
        throw ("It was not possible to get the features from the TriggerElement \"" + getTELabel((*TEs_itr).m_teL1) + "\". Vector is empty.");

      for (unsigned i = 0; i < trigRoiDescriptorVec.size(); i++) {
        if (!trigRoiDescriptorVec.at(i)) {
          (*m_log) << MSG::DEBUG << "Null pointer for trigRoiDescriptorVec" << endreq;
          continue;
        }

        if ((fabsf(eta - (trigRoiDescriptorVec.at(i))->eta()) < errorEta) && (delta(phi - (trigRoiDescriptorVec.at(i))->phi()) < errorPhi)) {
          matchedRoid = (trigRoiDescriptorVec.at(i)); 
          matchedTE = (*TEs_itr).m_teL2;
          errorEta = fabsf(eta - (trigRoiDescriptorVec.at(i))->eta());
          errorPhi = delta(phi - (trigRoiDescriptorVec.at(i))->phi());
        } //if matches	
      } // for trigRoiDescriptorVec
    } // for chainTEs
  } // if foundTE

  if (matchedRoid) { 
    cd(name() + "/" + chain + "/PreEfficiencies"); 
    hist1("TE_L1_Et")->Fill(et);
    hist1("TE_L1_Eta")->Fill(eta);
    hist1("TE_L1_Phi")->Fill(phi);

    std::vector<const TrigEMCluster *> featureTrigEMCluster;
    if (!const_cast<NavigationCore *>(m_nav)->getRecentFeatures(matchedTE, featureTrigEMCluster, "TrigCaloRinger"))
      throw ("An error ocurred while getting the features from the TriggerElement \"" + getTELabel(matchedTE) + "\"");
    for(std::vector<const TrigEMCluster *>::const_iterator idx = featureTrigEMCluster.begin(); idx != featureTrigEMCluster.end(); ++idx) {
      cd(name() + "/" + chain + "/Dump/L2Calo");
      hist1("Rcore")->Fill((*idx)->e237()/(*idx)->e277());
      hist1("Rstrip")->Fill(((*idx)->emaxs1() - (*idx)->e2tsts1())/((*idx)->emaxs1() + (*idx)->e2tsts1()));
      hist1("eta")->Fill((*idx)->eta());
      hist1("phi")->Fill((*idx)->phi());
      hist1("Et")->Fill(((*idx)->energy()/cosh((*idx)->eta()))/1e3);
      hist1("HadEt")->Fill(((*idx)->ehad1()/cosh((*idx)->eta()))/1e3);

      if ((*idx)->rings()) {
        cd(name() + "/" + chain + "/Dump/L2Calo/Ring");
        
        for (unsigned int iRing = 0; iRing < (*idx)->rings()->size(); ++idx) {
          std::stringstream histName;
          histName << "R" << iRing;
          hist1(histName.str())->Fill((*idx)->rings()->at(iRing));
        }
      } else {
        (*m_log) << MSG::INFO << "No ring information available." << endreq;
      }
    }

    // LVL2 Step1
    if (matchedTE->getActiveState()) {
      cd(name() + "/" + chain + "/PreEfficiencies");
      hist1("TE_L2_Step1_Et")->Fill(et);
      hist1("TE_L2_Step1_Eta")->Fill(eta);
      hist1("TE_L2_Step1_Phi")->Fill(phi);
      
      vector <TriggerElement *> nextTEs = m_nav->getDirectSuccessors(matchedTE);
      try {
        StatusCode sc = HLTValidation(chain, eta, phi, et, "L2_", 1, nextTEs, outputTEs);
        if (sc.isFailure())
          return sc;
      } catch (const std::string &message) {
        throw message;
      }
    } // if TE is Active L2Step1
  } //if matchedRoi
  return StatusCode::SUCCESS;
}

StatusCode NavRingerValidation::childInitialize() {
  StatusCode sc;

  sc = m_trigdec.retrieve();
  if (sc.isFailure()) {
    (*m_log) << MSG::ERROR << "Could not retrieve TrigDecisionTool!" << endreq;
    return sc;
  }

  // Look for chain in configuration
  const TrigConf::HLTChainList& conf_chains = fHLTFrame->getHLTChainList(); 
  std::vector<std::string> newVecChain; // vector after removing useless chains
  for (std::vector<std::string>::const_iterator i = vec_chain.begin(); i != vec_chain.end(); ++i){
    bool found = false;
    std::string conf_chain_name;
    for (TrigConf::HLTChainList::const_iterator j = conf_chains.begin(); j != conf_chains.end(); ++j) {
      conf_chain_name = (*j)->chain_name().substr(3);
      if ((*i).compare(conf_chain_name) == 0) {
        found = true;
        break;
      }
    }
    if (found) {
      addDirectory(name() + "/" + (*i) + "/PreEfficiencies");
      addHistogram(new TH1F("TruthEtHist", "Truth E_{T} for single electron ; E_{T} [GeV] ; Count", 80, 0., 80.));
      addHistogram(new TH1F("TruthEtaHist", "Truth \\eta for single electron ; \\eta ; Count", 128, -3.2, 3.2));
      addHistogram(new TH1F("TruthPhiHist", "Truth \\phi for single electron ; \\phi [rad] ; Count", 128, -3.2, 3.2));

      addHistogram(new TH1F("TE_L1_Et", "Level 1 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L1_Eta", "Level 1 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L1_Phi", "Level 1 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_L2_Step1_Et", "Level 2 Step1 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step1_Eta", "Level 2 Step1 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step1_Phi", "Level 2 Step1 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_L2_Step2_Et", "Level 2 Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step2_Eta", "Level 2 Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step2_Phi", "Level 2 Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_L2_Step3_Et", "Level 2 Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_L2_Step3_Eta", "Level 2 Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_L2_Step3_Phi", "Level 2 Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_EF_Step1_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step1_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step1_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_EF_Step2_Et", "EF Step2 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step2_Eta", "EF Step2 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step2_Phi", "EF Step2 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));
      
      addHistogram(new TH1F("TE_EF_Step3_Et", "EF Step3 Truth E_{T} ; E_{T} [GeV]" , 80 , 0. , 80.));
      addHistogram(new TH1F("TE_EF_Step3_Eta", "EF Step3 Truth \\eta ; \\eta" , 128 , -3.2, 3.2));
      addHistogram(new TH1F("TE_EF_Step3_Phi", "EF Step3 Truth \\phi ; \\phi" , 128, -3.2 , 3.2));

      //addDirectory (name() + "/" + (*i) + "/Dump/L1");
      //addHistogram(new TH1F("EMClus", "Level 1 EMClus ; EMClus [GeV] ; Probability" , 80 , 0. , 80.));
      //addHistogram(new TH1F("EMIsol", "Level 1 EMIsol ; EMIsol [GeV]; Probability" , 80 , 0., 80.));
      //addHistogram(new TH1F("HadCore", "Level 1 HadCore ; HadCore [GeV]; Probability" , 80 , 0., 80.));
      //addHistogram(new TH1F("HadIsol", "Level 1 HadIsol ; HadIsol [GeV]; Probability" , 80 , 0., 80.));

      addDirectory (name() + "/" + (*i) + "/Dump/L2Calo");
      addHistogram(new TH1F("Rcore", "Level 2 Calo Rcore ; Rcore ; Probability", 100 , 0. , 1.));
      addHistogram(new TH1F("Rstrip", "Level 2 Calo Rstrip ; Rstrip ; Probability" , 100 , 0., 1.));
      addHistogram(new TH1F("eta", "Level 2 Calo \\eta ; \\eta ; Probability" , 200 , -2.5, 2.5));
      addHistogram(new TH1F("phi", "Level 2 Calo \\phi ; \\phi [rd] ; Probability" , 256, -3.2, 3.2));
      addHistogram(new TH1F("Et", "Level 2 Calo EM E_{T} ; E_{T} [GeV] ; Probability" , 160 , 0., 80.));
      addHistogram(new TH1F("HadEt", "Level 2 Calo Had E_{T} ; E_{T} [GeV]; Probability" , 20 , 0., 2.));

      addDirectory (name() + "/" + (*i) + "/Dump/L2Calo/Ring");
      for (unsigned int iRing = 0; iRing < 100; ++iRing) {
        std::stringstream histName,histTitle;
        histName << "R" << iRing;
        histTitle << "Level 2 Calo Ring " << iRing << "; Ring " << iRing << "; Probability";
        addHistogram(new TH1F(histName.str().c_str(), histTitle.str().c_str() , 100 , 0., 1.));
      }

      newVecChain.push_back(*i);
    } else {
      (*m_log) << MSG::WARNING << "The chains \"L2_" << (*i) << "\" and \"EF_" << (*i) << "\" do not exist. These chains will be skipped." << endreq;
    }
  }
  vec_chain.clear();
  for (std::vector<std::string>::const_iterator i = newVecChain.begin(); i != newVecChain.end(); ++i) {
    vec_chain.push_back(*i);
  }

  return StatusCode::SUCCESS;
}

StatusCode NavRingerValidation::childExecute()
{
  StatusCode sc;
  
  m_nav = m_trigdec->ExperimentalAndExpertMethods()->getNavigation();
  if (!m_nav) { 
    (*m_log) << MSG::ERROR << "Navigation is pointer to NULL!" << endreq;;
    return StatusCode::SUCCESS;
  }

  if (vec_chain.size() == 0) {
    (*m_log) << MSG::ERROR << "Chain vector is empty, there is nothing to be done" << endreq;
    return StatusCode::FAILURE;
  }

  for (std::vector<std::string>::const_iterator i = vec_chain.begin(); i != vec_chain.end(); ++i) {
    sc = chainExecute(*i);
    if (sc.isFailure()) {
      (*m_log) << MSG::ERROR << "An Error ocurred during the processing of chain: \"" << *i << "\"" << endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode NavRingerValidation::chainExecute(const std::string &chain) {
  StatusCode sc;
  
  const DataHandle<McEventCollection> themcpartTES,mcpartTESend;
  McEventCollection::const_iterator Itr, ItrE;

  sc = m_storeGate->retrieve(themcpartTES, mcpartTESend);
  if (sc.isFailure()) {
    (*m_log) << MSG::WARNING << "No MC truth particle container found in StoreGate. Aborting!!!" << endreq;
    return StatusCode::SUCCESS; // Don't stop proccessing events
  }

  for (; themcpartTES != mcpartTESend; ++themcpartTES) { // For each event collection
    if (themcpartTES.key() == "TruthEvent") {
      for (Itr = themcpartTES->begin(), ItrE = themcpartTES->end(); Itr != ItrE; ++Itr) { // For each event
        if (!(*Itr)) {
          (*m_log) << MSG::DEBUG << "*Itr is zero!" << endreq;
          continue;
        }

        HepMC::GenEvent::particle_const_iterator itrParticle, itrParticleE;  
        for (itrParticle = (*Itr)->particles_begin(), itrParticleE = (*Itr)->particles_end(); itrParticle != itrParticleE; ++itrParticle) { // For each particle in the event
          // *Itr is a GenEvent
          // *itrParticle is a GenParticle
          if (!(*itrParticle))
            continue;

          // Finds an e-
          if ( fabsf((*itrParticle)->pdg_id()) == 11)
            break;
        } // for each particle

        if (itrParticle == itrParticleE) 
          continue;

        // Fill the Truth Histograms for these particles
        float truthEt = (*itrParticle)->momentum().e()/cosh((*itrParticle)->momentum().eta());
        if ( (truthEt > m_lowEtCut) && (fabsf((*itrParticle)->momentum().eta()) <= m_maxEtaCut) ) {
          cd(name() + "/" + chain + "/PreEfficiencies");
          hist1("TruthEtHist")->Fill(truthEt/1.e3);
          hist1("TruthEtaHist")->Fill((*itrParticle)->momentum().eta());
          hist1("TruthPhiHist")->Fill((*itrParticle)->momentum().phi());
          try {
            sc = NavigationMatch(chain, (*itrParticle)->momentum().eta(), (*itrParticle)->momentum().phi(), truthEt/1.e3);
            if (sc.isFailure())
              return StatusCode::FAILURE;
          } catch (std::string message) {
            (*m_log) << MSG::WARNING << "Exception thrown: " << message << endreq;
          } catch (...) {
            (*m_log) << MSG::WARNING << "Unknown Exception Thrown" << endreq;
          }
        } // Truth matched
      } // for event
    } // if it is a Truth Event 
  } // for every collection
  return StatusCode::SUCCESS;
}

StatusCode NavRingerValidation::childFinalize()
{
  for (std::vector<std::string>::const_iterator chain = vec_chain.begin(); chain != vec_chain.end(); ++chain) {
    addDirectory(name() + "/" + (*chain) + "/Efficiencies");
    std::vector<std::string> PreEffPattern;
    std::vector<std::string> PreEffNotPattern;
    PreEffPattern.push_back(name() + "/" + (*chain) + "/PreEfficiencies");
    PreEffNotPattern.push_back("Hist");
    hist1("TruthEtHist", name() + "/" + (*chain) + "/PreEfficiencies")->Sumw2();
    hist1("TruthEtaHist", name() + "/" + (*chain) + "/PreEfficiencies")->Sumw2();
    hist1("TruthPhiHist", name() + "/" + (*chain) + "/PreEfficiencies")->Sumw2();
    
    std::map<std::string, TH1 *> PreHists;
    getHistsFromPath(PreEffPattern, PreEffNotPattern, PreHists);
    for (std::map<std::string, TH1 *>::iterator i = PreHists.begin(); i != PreHists.end(); i++) {
      TH1F *newhist = new TH1F(*( (TH1F *) i->second));
      addHistogram(newhist);
      newhist->GetYaxis()->SetTitle("Efficiency");
      newhist->Sumw2();
      if (i->first.find_last_of("Et") == (i->first.size() - 1)) 
        newhist->Divide(hist1("TruthEtHist", name() + "/" + (*chain) + "/PreEfficiencies"));
      else if (i->first.find_last_of("Eta") == (i->first.size() - 1)) 	
        newhist->Divide(hist1("TruthEtaHist", name() + "/" + (*chain) + "/PreEfficiencies"));
      else if (i->first.find_last_of("Phi") == (i->first.size() - 1)) 	
        newhist->Divide(hist1("TruthPhiHist", name() + "/" + (*chain) + "/PreEfficiencies"));
    }
  }
  return StatusCode::SUCCESS;
}

