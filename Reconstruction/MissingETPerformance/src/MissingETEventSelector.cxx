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

using namespace TruthHelper; 
using CLHEP::GeV;

#include <algorithm>

MissingETEventSelector::MissingETEventSelector( const std::string& type,
    const std::string& name,
    const IInterface* parent ) : AthAlgTool( type, name, parent ),
				 //				 m_mbtsfilter("MBTSTimeFilterTool"),
				 //				 m_calofilter("CaloTimeFilterTool"),
				 _jetVariables ("JetVariablesTool"),
				 m_analysisTools( "AnalysisTools", this ){
  declareInterface<MissingETEventSelector>( this );

  declareProperty("ElectronContainer",            _electronContainerName   = "ElectronAODCollection");
  declareProperty("MuonContainerKey",             _muonContainerKey        = "StacoMuonCollection");
  declareProperty("JetKey",                       _jetKey                  = "Cone4H1TopoJets");

  declareProperty("JetPtCut",                     _jetPtCut                = 20.*GeV);
  declareProperty("JetElecOverlapDeltaR",         _jetElDeltaR             = 0.2);
  declareProperty("MinNJets",                     _minNjets                = 0);
  declareProperty("MaxNJets",                     _maxNjets                = 100);
  declareProperty("ElecPtCut",                    _elecPtCut               = 20.*GeV);
  declareProperty("MaxElecIsolCone",              _maxelecIsolCone         = 10.*GeV);
  declareProperty("MaxMuonIsolCone",              _maxmuonIsolCone         = 10.*GeV);
  declareProperty("isEM",                         _isEM                    = 0);
  declareProperty("MuonPtCut",                    _muonPtCut               = 20.*GeV);
  declareProperty("minRefFinalMET",               _minRefFinalMET          = 20.*GeV);
  declareProperty("minNonIntMET",                 _minNonIntMET            = 20.*GeV);
  declareProperty("minInvMass",                   _minInvMass              = 0.);
  declareProperty("maxInvMass",                   _maxInvMass              = 999.*GeV);
  declareProperty("selectionList",                _selectionList);
  declareProperty("minEventEMfraction",           _minEventEMfraction      = 0.);
  declareProperty("minPerJetEMfraction",          _minPerJetEMfraction= 0.);
  declareProperty("N90CellsCut",                  _N90CellsCut             = 5);

  declareProperty("SelectionType",                _selectionType           = "none");
  declareProperty("selectedMuonKey",              _selectedMuonKey         = "SelectedMuons");
  declareProperty("selectedElectronKey",          _selectedElectronKey     = "SelectedElectrons");
  declareProperty("GoodnessConfig",               _goodnessConfig          = "All");
  
  declareProperty("SelectionZtautau",             _selectionZtautau        = "PassedInvMassAnalysis");    
  declareProperty("PassflagZtautau",              _passflagZtautau         = "ZToTwoTausPassedInvMassAnalysis");    
  declareProperty("tau_container",                _tau_container           = "ZToTwoTausChosenTauJets" );
  declareProperty("lepton_container",             _lepton_container        = "ZToTwoTausChosenLepton" );

  declareProperty("FolderName",           m_folderName="");

  declareProperty( "AnalysisTools", m_analysisTools );
  _METdata = 0;
}

MissingETEventSelector::~MissingETEventSelector() {}

StatusCode MissingETEventSelector::initialize() {
  /// get a handle on the analysis tools                                                                                    
  StatusCode sc = m_analysisTools.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on analysis tools" << endreq;
    return sc;
  }

  sc = service("THistSvc", m_thistSvc);

//   if (
//       (_selectionType == "passesMBTSCut") ||
//       (_selectionType == "ANDedSelection" && find(_selectionList.begin(), _selectionList.end(), "passesMBTSCut") != _selectionList.end())
//       ) {
//     sc = m_mbtsfilter.retrieve();
//     if ( sc.isFailure() ) {
//       msg() << MSG::ERROR << "Can't get handle on MBTSTimeFilterTool in MissingETEventSelector" << endreq;
//       return sc;
//     }
//   }//retrieve MBTS tool if necessary

//   if (
//       (_selectionType == "passesCALOCut") ||
//       (_selectionType == "ANDedSelection" &&  find(_selectionList.begin(), _selectionList.end(), "passesCALOCut") != _selectionList.end())
//       ) {
//     sc = m_calofilter.retrieve();
//     if ( sc.isFailure() ) {
//       msg() << MSG::ERROR << "Can't get handle on CaloTimeFilterTool in MissingETEventSelector" << endreq;
//       return sc;
//     }
//   }//retrieve Calo tool if necessary

  sc = _jetVariables.retrieve();
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Can't get handle on JetVariablesTool in MissingETEventSelector" << endreq;
    return sc;
  }

  if (_selectionType == "hasZboson_mumu") {
    hasZboson_mumu_nmuons = new TH1D("hasZboson_mumu_nmuons","hasZboson_mumu_nmuons",10,0,10);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_nmuons",hasZboson_mumu_nmuons);

    hasZboson_mumu_leadpt = new TH1D("hasZboson_mumu_leadpt","hasZboson_mumu_leadpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_leadpt",hasZboson_mumu_leadpt);

    hasZboson_mumu_secondpt = new TH1D("hasZboson_mumu_secondpt","hasZboson_mumu_secondpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_secondpt",hasZboson_mumu_secondpt);

    hasZboson_mumu_InvMass = new TH1D("hasZboson_mumu_InvMass","hasZboson_mumu_InvMass",2000,0.,200.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_mumu_InvMass",hasZboson_mumu_InvMass);
  }

  if (_selectionType == "hasZboson_ee") {
    hasZboson_ee_nelecs = new TH1D("hasZboson_ee_nelecs","hasZboson_ee_nelecs",10,0,10);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_nelecs",hasZboson_ee_nelecs);

    hasZboson_ee_leadpt = new TH1D("hasZboson_ee_leadpt","hasZboson_ee_leadpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_leadpt",hasZboson_ee_leadpt);

    hasZboson_ee_secondpt = new TH1D("hasZboson_ee_secondpt","hasZboson_ee_secondpt",100,0.,100.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_secondpt",hasZboson_ee_secondpt);

    hasZboson_ee_InvMass = new TH1D("hasZboson_ee_InvMass","hasZboson_ee_InvMass",2000,0.,200.);
    sc = m_thistSvc->regHist("/AANT/" + m_folderName +"EventSelector/hasZboson_ee_InvMass",hasZboson_ee_InvMass);
  }

  return sc;
}

StatusCode MissingETEventSelector::finalize() {
  msg() << MSG::DEBUG << "MissingETEventSelector Tool finalize() has been called" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode MissingETEventSelector::retrieveContainers() {
  msg() << MSG::DEBUG << "in MissingETEventSelector::retrieveContainers() " << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  electrons = 0;
  if (evtStore()->contains<ElectronContainer>(_electronContainerName)) {
    sc=evtStore()->retrieve( electrons, _electronContainerName);
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No ElectronContainer found in StoreGate, key:" << _electronContainerName << endreq; 
    return StatusCode::FAILURE;
  }

  jet = 0;
  if (evtStore()->contains<JetCollection>(_jetKey)) {
    sc=evtStore()->retrieve( jet, _jetKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No JetCollection found in StoreGate, key:" << _jetKey << endreq; 
    return StatusCode::FAILURE;
  }

  muons = 0;
  if (evtStore()->contains<Analysis::MuonContainer>(_muonContainerKey)) {
    sc = evtStore()->retrieve( muons, _muonContainerKey );
    if ( sc.isFailure() ) { return sc; }
  }
  else {
    msg() << MSG::WARNING << "No MuonContainer found in StoreGate key: " << _muonContainerKey << endreq;
    return StatusCode::FAILURE;
  }

  _jetVariables->retrieveContainers();

  return sc;
}//end retrieveContainers()

bool MissingETEventSelector::isSelectedEvent() {
  bool isSelected = true;

  if (_selectionType == "ANDedSelection") {
    isSelected = ANDedSelection();
  }
  if (_selectionType == "hasOneElectron") {
    isSelected = hasOneElectron();
  }
  if (_selectionType == "hasOneMuon") {
    isSelected = hasOneMuon();
  }
  if (_selectionType == "hasOneJet") {
    isSelected = hasOneJet();
  }
  if (_selectionType == "hasNJets") {
    isSelected = hasNJets();
  }
  if (_selectionType == "hasMinMETRefFinal") {
    isSelected = hasMinMissingETRefFinal();
  }
  if (_selectionType == "hasMinMETNonInt") {
    isSelected = hasMinMissingETNonInt();
  }
  if (_selectionType == "hasZboson_mumu") {
    isSelected = hasZboson_mumu();
  }
  if (_selectionType == "hasZboson_ee") {
    isSelected = hasZboson_ee();
  }
//   if (_selectionType == "passesMBTSCut") {
//     isSelected = passesMBTSCut();
//   }
//   if (_selectionType == "passesCALOCut") {
//     isSelected = passesCALOCut();
//   }
  if (_selectionType == "passesMETGoodness") {
    isSelected = passesMETGoodness();
  }
  if (_selectionType == "hasMinEventEMfraction") {
    isSelected = hasMinEventEMfraction();
  }
  if (_selectionType == "hasMinPerJetEMfraction") {
    isSelected = hasMinPerJetEMfraction();
  }
  if (_selectionType == "passesN90CellsCut") {
    isSelected = passesN90CellsCut();
  }
  if (_selectionType == "hasgoodZ_tautau") {
    isSelected = hasgoodZ_tautau();
  }

  return isSelected;
}


bool MissingETEventSelector::ANDedSelection() {
  for (std::vector<std::string>::iterator it = _selectionList.begin(); it != _selectionList.end(); ++it) {
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
     msg() << MSG::WARNING << "ANDed selection function doesn't know about requested selection function: " << *it << ", event fails!" << endreq;
     return false;
    }
  }//loop over selection names
  //return true if none of functions return false;
  return true;
}//end ANDedSelection()

bool MissingETEventSelector::hasOneJet() {
  for (JetCollection::const_iterator it = jet->begin(); it != jet->end(); ++it) {
    if ( (*it)->pt() < _jetPtCut ) continue;
    return true;
  }
  return false;
}


bool MissingETEventSelector::hasNJets() {
  int njets = 0;
  for (JetCollection::const_iterator it = jet->begin(); it != jet->end(); ++it) {
    if ( (*it)->pt() < _jetPtCut ) continue;
    njets++;
  }
  std::cout<<"njets: "<<njets<<std::endl;
  if (njets >= _minNjets && njets <=_maxNjets) {return true;}
  return false;
}

bool MissingETEventSelector::hasMinPerJetEMfraction() {
  float leastJetEMfrac = _jetVariables->leastJetEMfraction();

  if (leastJetEMfrac > _minPerJetEMfraction) return true;
  return false;
}

bool MissingETEventSelector::hasMinEventEMfraction() {

  float evtemfrac = 0.;

  evtemfrac = _jetVariables->JetPtWeightedEventEMfraction();

  if (evtemfrac == -9999.) {return true;}

  if ( evtemfrac >= _minEventEMfraction) {return true;}
  return false;
}//end hasMinEventEMfraction()

bool MissingETEventSelector::passesN90CellsCut() {

  int lowestn90 = _jetVariables->lowestN90CellsJet();
  if (lowestn90 < _N90CellsCut) return false;
  return true;
}

//With Overlap removal
bool MissingETEventSelector::hasNJetsOverlapRemoval(){
  int njets =0;
  for (JetCollection::const_iterator j_it = jet->begin(); j_it != jet->end(); ++j_it) {
    if ( (*j_it)->pt() < _jetPtCut ) continue;
    bool overlap=false;
    for (ElectronContainer::const_iterator el_it = electrons->begin(); el_it != electrons->end(); ++el_it ) {
      bool author = (*el_it)->author(egammaParameters::AuthorElectron);
      if(!author) continue;
      double deltaR = m_analysisTools->deltaR( *j_it, *el_it );
      if( deltaR < _jetElDeltaR) {
	overlap=true;
	break;
      }
    }
    if(!overlap) ++njets;
  }

  if (njets >= _minNjets && njets <=_maxNjets) {return true;}
  return false;
}



bool MissingETEventSelector::hasOneElectron() {
  for (ElectronContainer::const_iterator it = electrons->begin(); it != electrons->end(); ++it) {
    bool author = (*it)->author(egammaParameters::AuthorElectron);
    if ( !author || (*it)->pt() < _elecPtCut ) continue;

    const EMShower* shower = (*it)->detail<EMShower>("egDetailAOD");
    if (!shower || shower->parameter(egammaParameters::etcone20) > _maxelecIsolCone) continue;

    return true;
  }
  return false;
}

bool MissingETEventSelector::hasOneMuon() {
  for (Analysis::MuonContainer::const_iterator it = muons->begin(); it != muons->end(); ++it ) {
    if ((*it)->pt() < _muonPtCut ) continue;

    if ((*it)->parameter(MuonParameters::etcone20) > _maxmuonIsolCone) continue;

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
  return m_metgoodness.IsGood(_goodnessConfig.c_str());
}

bool MissingETEventSelector::hasMinMissingETRefFinal() {
  MissingETData *data = METData();
  if (data != 0) {
    float MET = sqrt(pow(data->refFinal()->etx(),2)+pow(data->refFinal()->ety(),2));
    if (MET >= _minRefFinalMET) {
      return true;
    }
  }
  return false;
}

bool MissingETEventSelector::hasMinMissingETNonInt() {
  MissingETData *data = METData();
  if (data != 0) {
    float MET = sqrt(pow(data->truth_nonInt()->etx(),2)+pow(data->truth_nonInt()->ety(),2));
    if (MET >= _minNonIntMET) {
      return true;
    }
  }
  return false;
}

bool MissingETEventSelector::hasZboson_mumu() {

  Analysis::MuonContainer *_passedMuons = new Analysis::MuonContainer;
  StatusCode sc = evtStore()->record ( _passedMuons, _selectedMuonKey);
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "Not able to store " << _selectedMuonKey << endreq;
    return false;
  }

  hasZboson_mumu_nmuons->Fill(muons->size());
  if (muons->size() < 2) { return false; }

  //min and max n jet requirement
  if (!hasNJetsOverlapRemoval()) {return false;}

  double leadPt = 0;
  double secondPt = 0;
  for (Analysis::MuonContainer::const_iterator it = muons->begin(); it != muons->end(); ++it ) {
    if ((*it)->pt() > leadPt) {
      secondPt = leadPt;
      leadPt = (*it)->pt();
    }
    else if ((*it)->pt() > secondPt) {
      secondPt = (*it)->pt();
    }
  }//loop over muons

  hasZboson_mumu_leadpt->Fill(leadPt/1000.);
  hasZboson_mumu_secondpt->Fill(secondPt/1000.);

  for (Analysis::MuonContainer::const_iterator it = muons->begin(); it != muons->end(); ++it ) {
    for (Analysis::MuonContainer::const_iterator it2 = it + 1; it2 != muons->end(); ++it2 ) {
      if ( ((*it)->pt() < _muonPtCut ) || ((*it2)->pt() < _muonPtCut ) ) continue;

      float invariantMass = ((*it)->hlv() + (*it2)->hlv()).m();
      hasZboson_mumu_InvMass->Fill(invariantMass/1000.);
      if (invariantMass > _minInvMass && invariantMass < _maxInvMass) {
	Analysis::Muon *mu1 = new Analysis::Muon(**it);
	Analysis::Muon *mu2 = new Analysis::Muon(**it2);
	_passedMuons->push_back(mu1);
	_passedMuons->push_back(mu2);
	return true;
      }
    }
  }

  return false;
}

bool MissingETEventSelector::hasZboson_ee() {

  ElectronContainer *_passedElectrons = new ElectronContainer;
  StatusCode sc = evtStore()->record ( _passedElectrons, _selectedElectronKey);
  if ( sc.isFailure() ) {
    msg() << MSG::WARNING << "Not able to store " << _selectedElectronKey << endreq;
    return false;
  }

  hasZboson_ee_nelecs->Fill(electrons->size());
  if (electrons->size() < 2) { return false; }

  //min and max n jet requirement
  if (!hasNJetsOverlapRemoval()) {return false;}

  double leadPt = 0;
  double secondPt = 0;
  for (ElectronContainer::const_iterator it = electrons->begin(); it != electrons->end(); ++it ) {
    if ((*it)->pt() > leadPt) {
      secondPt = leadPt;
      leadPt = (*it)->pt();
    }
    else if ((*it)->pt() > secondPt) {
      secondPt = (*it)->pt();
    }
  }//loop over electrons

  hasZboson_ee_leadpt->Fill(leadPt/1000.);
  hasZboson_ee_secondpt->Fill(secondPt/1000.);

  for (ElectronContainer::const_iterator it = electrons->begin(); it != electrons->end(); ++it ) {
    for (ElectronContainer::const_iterator it2 = it + 1; it2 != electrons->end(); ++it2 ) {
      if ( ((*it)->pt() < _elecPtCut ) || ((*it2)->pt() < _elecPtCut )  ||
	   ((int)(*it)->isem()%16 != (int) _isEM || (int)(*it2)->isem()%16 != (int) _isEM )) continue;

      float invariantMass = ((*it)->hlv() + (*it2)->hlv()).m();
      hasZboson_ee_InvMass->Fill(invariantMass/1000.);
      if (invariantMass > _minInvMass && invariantMass < _maxInvMass) {
	Analysis::Electron *el1 = new Analysis::Electron(**it);
	Analysis::Electron *el2 = new Analysis::Electron(**it2);
	_passedElectrons->push_back(el1);
	_passedElectrons->push_back(el2);
	return true;
      }
    }
  }

  return false;
}

bool MissingETEventSelector::hasgoodZ_tautau(){           
  
 if (_selectionZtautau == "PassedInvMassAnalysis") {
    // getting passflag from SG
    int* flag = 0;
    if (evtStore()->retrieve(flag, _passflagZtautau).isFailure() || !flag){ 
        msg() << MSG::DEBUG <<"Failed to retrieve " << _passflagZtautau <<endreq;
        return StatusCode::FAILURE;
    }	
    if (*flag!=0){
        msg() << MSG::WARNING << "Event ok = flag " << *flag << endreq;
        return true;
    }
    return false;	
 }   
   
 if (_selectionZtautau == "PassedLooseSelection") { 
    const INavigable4MomentumCollection* tau_cont = 0;
   
    
    if(evtStore()->retrieve(tau_cont, _tau_container).isFailure() || !tau_cont )
    {
        msg() << MSG::DEBUG <<"Failed to retrieve " << _tau_container <<endreq;
        return StatusCode::FAILURE;
    }
    if(tau_cont->size() == 0)
    {
        msg() << MSG::DEBUG <<"Event does not have any chosen tau-jets."<<endreq; 
	 return  false;
    }

    // getting lepton container from SG
	const INavigable4MomentumCollection* lepton_cont = 0;
    if(evtStore()->retrieve(lepton_cont, _lepton_container).isFailure() || !lepton_cont )
    {
         msg() << MSG::DEBUG <<"Failed to retrieve " << _lepton_container<<endreq; 
        return StatusCode::FAILURE;
    }
    if(lepton_cont->size() != 1)
    {
        msg() << MSG::DEBUG <<"Event does not have a single chosen lepton. lepton_cont->size() = " << lepton_cont->size()<< endreq; 
	       return false;
    }
  return true;
  }

return false;
}
