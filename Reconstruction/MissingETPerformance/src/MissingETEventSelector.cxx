/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/MissingETEventSelector.h"

//#include "PrimaryDPDMaker/CaloTimeFilterTool.h"
//#include "PrimaryDPDMaker/MBTSTimeFilterTool.h"

#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

#include "muonEvent/MuonContainer.h"

#include "egammaEvent/ElectronContainer.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/egammaParamDefs.h"

#include "TruthHelper/IsGenStable.h"
#include "TruthHelper/IsGenNonInteracting.h"
#include "TruthHelper/GenAccessIO.h"
#include "AthenaKernel/Units.h"

using namespace TruthHelper; 
using Athena::Units::GeV;

#include <algorithm>

MissingETEventSelector::MissingETEventSelector( const std::string& type,
    const std::string& name,
    const IInterface* parent ) : AthAlgTool( type, name, parent ),
				 //				 m_mbtsfilter("MBTSTimeFilterTool"),
				 //				 m_calofilter("CaloTimeFilterTool"),
				 m_jetVariables ("JetVariablesTool"),
				 m_analysisTools( "AnalysisTools", this ){
  declareInterface<MissingETEventSelector>( this );

  declareProperty("ElectronContainer",            m_electronContainerName   = "ElectronAODCollection");
  declareProperty("MuonContainerKey",             m_muonContainerKey        = "StacoMuonCollection");
  declareProperty("JetKey",                       m_jetKey                  = "Cone4H1TopoJets");

  declareProperty("JetPtCut",                     m_jetPtCut                = 20.*GeV);
  declareProperty("JetElecOverlapDeltaR",         m_jetElDeltaR             = 0.2);
  declareProperty("MinNJets",                     m_minNjets                = 0);
  declareProperty("MaxNJets",                     m_maxNjets                = 100);
  declareProperty("ElecPtCut",                    m_elecPtCut               = 20.*GeV);
  declareProperty("MaxElecIsolCone",              m_maxelecIsolCone         = 10.*GeV);
  declareProperty("MaxMuonIsolCone",              m_maxmuonIsolCone         = 10.*GeV);
  declareProperty("isEM",                         m_isEM                    = 0);
  declareProperty("MuonPtCut",                    m_muonPtCut               = 20.*GeV);
  declareProperty("minRefFinalMET",               m_minRefFinalMET          = 20.*GeV);
  declareProperty("minNonIntMET",                 m_minNonIntMET            = 20.*GeV);
  declareProperty("minInvMass",                   m_minInvMass              = 0.);
  declareProperty("maxInvMass",                   m_maxInvMass              = 999.*GeV);
  declareProperty("selectionList",                m_selectionList);
  declareProperty("minEventEMfraction",           m_minEventEMfraction      = 0.);
  declareProperty("minPerJetEMfraction",          m_minPerJetEMfraction= 0.);
  declareProperty("N90CellsCut",                  m_N90CellsCut             = 5);

  declareProperty("SelectionType",                m_selectionType           = "none");
  declareProperty("selectedMuonKey",              m_selectedMuonKey         = "SelectedMuons");
  declareProperty("selectedElectronKey",          m_selectedElectronKey     = "SelectedElectrons");
  declareProperty("GoodnessConfig",               m_goodnessConfig          = "All");
  
  declareProperty("SelectionZtautau",             m_selectionZtautau        = "PassedInvMassAnalysis");    
  declareProperty("PassflagZtautau",              m_passflagZtautau         = "ZToTwoTausPassedInvMassAnalysis");    
  declareProperty("tau_container",                m_tau_container           = "ZToTwoTausChosenTauJets" );
  declareProperty("lepton_container",             m_lepton_container        = "ZToTwoTausChosenLepton" );

  declareProperty("FolderName",           m_folderName="");

  declareProperty( "AnalysisTools", m_analysisTools );
  m_METdata = 0;
}

MissingETEventSelector::~MissingETEventSelector() {}

StatusCode MissingETEventSelector::initialize() {
  /// get a handle on the analysis tools                                                                                    
  StatusCode sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on analysis tools" << endmsg;
    return sc;
  }

  sc = service("THistSvc", m_thistSvc);

//   if (
//       (m_selectionType == "passesMBTSCut") ||
//       (m_selectionType == "ANDedSelection" && find(m_selectionList.begin(), m_selectionList.end(), "passesMBTSCut") != m_selectionList.end())
//       ) {
//     sc = m_mbtsfilter.retrieve();
//     if ( sc.isFailure() ) {
//       msg() << MSG::ERROR << "Can't get handle on MBTSTimeFilterTool in MissingETEventSelector" << endmsg;
//       return sc;
//     }
//   }//retrieve MBTS tool if necessary

//   if (
//       (m_selectionType == "passesCALOCut") ||
//       (m_selectionType == "ANDedSelection" &&  find(m_selectionList.begin(), m_selectionList.end(), "passesCALOCut") != m_selectionList.end())
//       ) {
//     sc = m_calofilter.retrieve();
//     if ( sc.isFailure() ) {
//       msg() << MSG::ERROR << "Can't get handle on CaloTimeFilterTool in MissingETEventSelector" << endmsg;
//       return sc;
//     }
//   }//retrieve Calo tool if necessary

  sc = m_jetVariables.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on JetVariablesTool in MissingETEventSelector" << endmsg;
    return sc;
  }

  if (m_selectionType == "hasZboson_mumu") {
    m_hasZboson_mumu_nmuons = new TH1D("hasZboson_mumu_nmuons","hasZboson_mumu_nmuons",10,0,10);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_nmuons",m_hasZboson_mumu_nmuons);

    m_hasZboson_mumu_leadpt = new TH1D("hasZboson_mumu_leadpt","hasZboson_mumu_leadpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_leadpt",m_hasZboson_mumu_leadpt);

    m_hasZboson_mumu_secondpt = new TH1D("hasZboson_mumu_secondpt","hasZboson_mumu_secondpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_secondpt",m_hasZboson_mumu_secondpt);

    m_hasZboson_mumu_InvMass = new TH1D("hasZboson_mumu_InvMass","hasZboson_mumu_InvMass",2000,0.,200.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_InvMass",m_hasZboson_mumu_InvMass);
  }

  if (m_selectionType == "hasZboson_ee") {
    m_hasZboson_ee_nelecs = new TH1D("hasZboson_ee_nelecs","hasZboson_ee_nelecs",10,0,10);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_nelecs",m_hasZboson_ee_nelecs);

    m_hasZboson_ee_leadpt = new TH1D("hasZboson_ee_leadpt","hasZboson_ee_leadpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_leadpt",m_hasZboson_ee_leadpt);

    m_hasZboson_ee_secondpt = new TH1D("hasZboson_ee_secondpt","hasZboson_ee_secondpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_secondpt",m_hasZboson_ee_secondpt);

    m_hasZboson_ee_InvMass = new TH1D("hasZboson_ee_InvMass","hasZboson_ee_InvMass",2000,0.,200.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_InvMass",m_hasZboson_ee_InvMass);
  }

  return sc;
}

StatusCode MissingETEventSelector::finalize() {
  msg() << MSG::DEBUG << "MissingETEventSelector Tool finalize() has been called" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode MissingETEventSelector::retrieveContainers() {
  msg() << MSG::DEBUG << "in MissingETEventSelector::retrieveContainers() " << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  m_electrons = 0;
  if (evtStore()->contains<ElectronContainer>(m_electronContainerName)) {
    sc=evtStore()->retrieve( m_electrons, m_electronContainerName);
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No ElectronContainer found in StoreGate, key:" << m_electronContainerName << endmsg; 
    return StatusCode::FAILURE;
  }

  m_jet = 0;
  if (evtStore()->contains<JetCollection>(m_jetKey)) {
    sc=evtStore()->retrieve( m_jet, m_jetKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No JetCollection found in StoreGate, key:" << m_jetKey << endmsg; 
    return StatusCode::FAILURE;
  }

  m_muons = 0;
  if (evtStore()->contains<Analysis::MuonContainer>(m_muonContainerKey)) {
    sc = evtStore()->retrieve( m_muons, m_muonContainerKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No MuonContainer found in StoreGate key: " << m_muonContainerKey << endmsg;
    return StatusCode::FAILURE;
  }

  m_jetVariables->retrieveContainers();

  return sc;
}//end retrieveContainers()

bool MissingETEventSelector::isSelectedEvent() {
  bool isSelected = true;

  if (m_selectionType == "ANDedSelection") {
    isSelected = ANDedSelection();
  }
  if (m_selectionType == "hasOneElectron") {
    isSelected = hasOneElectron();
  }
  if (m_selectionType == "hasOneMuon") {
    isSelected = hasOneMuon();
  }
  if (m_selectionType == "hasOneJet") {
    isSelected = hasOneJet();
  }
  if (m_selectionType == "hasNJets") {
    isSelected = hasNJets();
  }
  if (m_selectionType == "hasMinMETRefFinal") {
    isSelected = hasMinMissingETRefFinal();
  }
  if (m_selectionType == "hasMinMETNonInt") {
    isSelected = hasMinMissingETNonInt();
  }
  if (m_selectionType == "hasZboson_mumu") {
    isSelected = hasZboson_mumu();
  }
  if (m_selectionType == "hasZboson_ee") {
    isSelected = hasZboson_ee();
  }
//   if (m_selectionType == "passesMBTSCut") {
//     isSelected = passesMBTSCut();
//   }
//   if (m_selectionType == "passesCALOCut") {
//     isSelected = passesCALOCut();
//   }
  if (m_selectionType == "passesMETGoodness") {
    isSelected = passesMETGoodness();
  }
  if (m_selectionType == "hasMinEventEMfraction") {
    isSelected = hasMinEventEMfraction();
  }
  if (m_selectionType == "hasMinPerJetEMfraction") {
    isSelected = hasMinPerJetEMfraction();
  }
  if (m_selectionType == "passesN90CellsCut") {
    isSelected = passesN90CellsCut();
  }
  if (m_selectionType == "hasgoodZ_tautau") {
    isSelected = hasgoodZ_tautau();
  }

  return isSelected;
}


bool MissingETEventSelector::ANDedSelection() {
  for (std::vector<std::string>::iterator it = m_selectionList.begin(); it != m_selectionList.end(); ++it) {
    if ((*it) == "hasOneElectron") {if (!hasOneElectron()) return false;}
    else if ((*it) == "hasOneMuon") {if (!hasOneMuon()) return false;}
    else if ((*it) == "hasOneJet") {if (!hasOneJet()) return false;}
    else if ((*it) == "hasNJets") {if (!hasNJets()) return false;}
    else if ((*it) == "hasMinEventEMfraction") {if (!hasMinEventEMfraction()) return false;}
    else if ((*it) == "hasMinPerJetEMfraction") {if (!hasMinPerJetEMfraction()) return false;}
    else if ((*it) == "hasMinMETRefFinal") {if (!hasMinMissingETRefFinal()) return false;}
    else if ((*it) == "hasMinMETNonInt") {if (!hasMinMissingETNonInt()) return false;}
//     else if ((*it) == "passesMBTSCut") {if (!passesMBTSCut()) return false;}
//     else if ((*it) == "passesCALOCut") {if (!passesCALOCut()) return false;}
    else if ((*it) == "passesN90CellsCut") {if (!passesN90CellsCut()) return false;}
    else if ((*it) == "passesMETGoodness") {if (!passesMETGoodness()) return false;}
    else {
     msg() << MSG::WARNING << "ANDed selection function doesn't know about requested selection function: " << *it << ", event fails!" << endmsg;
     return false;
    }
  }//loop over selection names
  //return true if none of functions return false;
  return true;
}//end ANDedSelection()

bool MissingETEventSelector::hasOneJet() {
  for (JetCollection::const_iterator it = m_jet->begin(); it != m_jet->end(); ++it) {
    if ( (*it)->pt() < m_jetPtCut ) continue;
    return true;
  }
  return false;
}


bool MissingETEventSelector::hasNJets() {
  int njets = 0;
  for (JetCollection::const_iterator it = m_jet->begin(); it != m_jet->end(); ++it) {
    if ( (*it)->pt() < m_jetPtCut ) continue;
    njets++;
  }
  std::cout<<"njets: "<<njets<<std::endl;
  if (njets >= m_minNjets && njets <=m_maxNjets) {return true;}
  return false;
}

bool MissingETEventSelector::hasMinPerJetEMfraction() {
  float leastJetEMfrac = m_jetVariables->leastJetEMfraction();

  if (leastJetEMfrac > m_minPerJetEMfraction) return true;
  return false;
}

bool MissingETEventSelector::hasMinEventEMfraction() {

  float evtemfrac = 0.;

  evtemfrac = m_jetVariables->JetPtWeightedEventEMfraction();

  if (evtemfrac == -9999.) {return true;}

  if ( evtemfrac >= m_minEventEMfraction) {return true;}
  return false;
}//end hasMinEventEMfraction()

bool MissingETEventSelector::passesN90CellsCut() {

  int lowestn90 = m_jetVariables->lowestN90CellsJet();
  if (lowestn90 < m_N90CellsCut) return false;
  return true;
}

//With Overlap removal
bool MissingETEventSelector::hasNJetsOverlapRemoval(){
  int njets =0;
  for (JetCollection::const_iterator j_it = m_jet->begin(); j_it != m_jet->end(); ++j_it) {
    if ( (*j_it)->pt() < m_jetPtCut ) continue;
    bool overlap=false;
    for (ElectronContainer::const_iterator el_it = m_electrons->begin(); el_it != m_electrons->end(); ++el_it ) {
      bool author = (*el_it)->author(egammaParameters::AuthorElectron);
      if(!author) continue;
      double deltaR = m_analysisTools->deltaR( *j_it, *el_it );
      if( deltaR < m_jetElDeltaR) {
	overlap=true;
	break;
      }
    }
    if(!overlap) ++njets;
  }

  if (njets >= m_minNjets && njets <=m_maxNjets) {return true;}
  return false;
}



bool MissingETEventSelector::hasOneElectron() {
  for (ElectronContainer::const_iterator it = m_electrons->begin(); it != m_electrons->end(); ++it) {
    bool author = (*it)->author(egammaParameters::AuthorElectron);
    if ( !author || (*it)->pt() < m_elecPtCut ) continue;

    const EMShower* shower = (*it)->detail<EMShower>("egDetailAOD");
    if (!shower || shower->parameter(egammaParameters::etcone20) > m_maxelecIsolCone) continue;

    return true;
  }
  return false;
}

bool MissingETEventSelector::hasOneMuon() {
  for (Analysis::MuonContainer::const_iterator it = m_muons->begin(); it != m_muons->end(); ++it ) {
    if ((*it)->pt() < m_muonPtCut ) continue;

    if ((*it)->parameter(MuonParameters::etcone20) > m_maxmuonIsolCone) continue;

    return true;
  }
  return false;
}

// bool MissingETEventSelector::passesMBTSCut() {
//   bool passMBTS(false);
//   double timeDiff(999.), timeA(0.), timeC(0.);
//   int countA(0), countC(0);

//   StatusCode sc = m_mbtsfilter->getTimeDifference(passMBTS,timeDiff,timeA,timeC,countA,countC);

//   if (sc.isFailure() || !passMBTS) return false;
//   return true;
// }

// bool MissingETEventSelector::passesCALOCut() {
//   bool passCalo(false);
//   double timeDiff(999.), timeA(0.), timeC(0.);
//   int countA(0), countC(0);

//   StatusCode sc = m_calofilter->getTimeDifference(passCalo,timeDiff,timeA,timeC,countA,countC);

//   if (sc.isFailure() || !passCalo) return false;
//   return true;
// }

bool MissingETEventSelector::passesMETGoodness() {
  return m_metgoodness.IsGood(m_goodnessConfig.c_str());
}

bool MissingETEventSelector::hasMinMissingETRefFinal() {
  MissingETData *data = METData();
  if (data != 0) {
    float MET = sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2));
    if (MET >= m_minRefFinalMET) {
      return true;
    }
  }
  return false;
}

bool MissingETEventSelector::hasMinMissingETNonInt() {
  MissingETData *data = METData();
  if (data != 0) {
    float MET = sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2));
    if (MET >= m_minNonIntMET) {
      return true;
    }
  }
  return false;
}

bool MissingETEventSelector::hasZboson_mumu() {

  Analysis::MuonContainer *passedMuons = new Analysis::MuonContainer;
  StatusCode sc = evtStore()->record ( passedMuons, m_selectedMuonKey);
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "Not able to store " << m_selectedMuonKey << endmsg;
    return false;
  }

  m_hasZboson_mumu_nmuons->Fill(m_muons->size());
  if (m_muons->size() < 2) { return false; }

  //min and max n jet requirement
  if (!hasNJetsOverlapRemoval()) {return false;}

  double leadPt = 0;
  double secondPt = 0;
  for (Analysis::MuonContainer::const_iterator it = m_muons->begin(); it != m_muons->end(); ++it ) {
    if ((*it)->pt() > leadPt) {
      secondPt = leadPt;
      leadPt = (*it)->pt();
    }
    else if ((*it)->pt() > secondPt) {
      secondPt = (*it)->pt();
    }
  }//loop over muons

  m_hasZboson_mumu_leadpt->Fill(leadPt/GeV);
  m_hasZboson_mumu_secondpt->Fill(secondPt/GeV);

  for (Analysis::MuonContainer::const_iterator it = m_muons->begin(); it != m_muons->end(); ++it ) {
    for (Analysis::MuonContainer::const_iterator it2 = it + 1; it2 != m_muons->end(); ++it2 ) {
      if ( ((*it)->pt() < m_muonPtCut ) || ((*it2)->pt() < m_muonPtCut ) ) continue;

      float invariantMass = ((*it)->hlv() + (*it2)->hlv()).m();
      m_hasZboson_mumu_InvMass->Fill(invariantMass/GeV);
      if (invariantMass > m_minInvMass && invariantMass < m_maxInvMass) {
	Analysis::Muon *mu1 = new Analysis::Muon(**it);
	Analysis::Muon *mu2 = new Analysis::Muon(**it2);
	passedMuons->push_back(mu1);
	passedMuons->push_back(mu2);
	return true;
      }
    }
  }

  return false;
}

bool MissingETEventSelector::hasZboson_ee() {

  ElectronContainer *passedElectrons = new ElectronContainer;
  StatusCode sc = evtStore()->record ( passedElectrons, m_selectedElectronKey);
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "Not able to store " << m_selectedElectronKey << endmsg;
    return false;
  }

  m_hasZboson_ee_nelecs->Fill(m_electrons->size());
  if (m_electrons->size() < 2) { return false; }

  //min and max n jet requirement
  if (!hasNJetsOverlapRemoval()) {return false;}

  double leadPt = 0;
  double secondPt = 0;
  for (ElectronContainer::const_iterator it = m_electrons->begin(); it != m_electrons->end(); ++it ) {
    if ((*it)->pt() > leadPt) {
      secondPt = leadPt;
      leadPt = (*it)->pt();
    }
    else if ((*it)->pt() > secondPt) {
      secondPt = (*it)->pt();
    }
  }//loop over electrons

  m_hasZboson_ee_leadpt->Fill(leadPt/GeV);
  m_hasZboson_ee_secondpt->Fill(secondPt/GeV);

  for (ElectronContainer::const_iterator it = m_electrons->begin(); it != m_electrons->end(); ++it ) {
    for (ElectronContainer::const_iterator it2 = it + 1; it2 != m_electrons->end(); ++it2 ) {
      if ( ((*it)->pt() < m_elecPtCut ) || ((*it2)->pt() < m_elecPtCut )  ||
	   ((int)(*it)->isem()%16 != (int) m_isEM || (int)(*it2)->isem()%16 != (int) m_isEM )) continue;

      float invariantMass = ((*it)->hlv() + (*it2)->hlv()).m();
      m_hasZboson_ee_InvMass->Fill(invariantMass/GeV);
      if (invariantMass > m_minInvMass && invariantMass < m_maxInvMass) {
	Analysis::Electron *el1 = new Analysis::Electron(**it);
	Analysis::Electron *el2 = new Analysis::Electron(**it2);
	passedElectrons->push_back(el1);
	passedElectrons->push_back(el2);
	return true;
      }
    }
  }

  return false;
}

bool MissingETEventSelector::hasgoodZ_tautau(){           
  
 if (m_selectionZtautau == "PassedInvMassAnalysis") {
    // getting passflag from SG
    int* flag = 0;
    if (evtStore()->retrieve(flag, m_passflagZtautau).isFailure() || !flag){ 
        msg() << MSG::DEBUG <<"Failed to retrieve " << m_passflagZtautau <<endmsg;
        return false;
    }	
    if (*flag!=0){
        msg() << MSG::WARNING << "Event ok = flag " << *flag << endmsg;
        return true;
    }
    return false;	
 }   
   
 if (m_selectionZtautau == "PassedLooseSelection") { 
    const INavigable4MomentumCollection* tau_cont = 0;
   
    
    if(evtStore()->retrieve(tau_cont, m_tau_container).isFailure() || !tau_cont )
    {
        msg() << MSG::DEBUG <<"Failed to retrieve " << m_tau_container <<endmsg;
        return false;
    }
    if(tau_cont->size() == 0)
    {
        msg() << MSG::DEBUG <<"Event does not have any chosen tau-jets."<<endmsg; 
	 return  false;
    }

    // getting lepton container from SG
	const INavigable4MomentumCollection* lepton_cont = 0;
    if(evtStore()->retrieve(lepton_cont, m_lepton_container).isFailure() || !lepton_cont )
    {
         msg() << MSG::DEBUG <<"Failed to retrieve " << m_lepton_container<<endmsg; 
        return false;
    }
    if(lepton_cont->size() != 1)
    {
        msg() << MSG::DEBUG <<"Event does not have a single chosen lepton. lepton_cont->size() = " << lepton_cont->size()<< endmsg; 
	       return false;
    }
  return true;
  }

return false;
}
