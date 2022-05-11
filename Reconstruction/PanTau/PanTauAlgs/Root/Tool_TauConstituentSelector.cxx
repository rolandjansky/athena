/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PanTauAlgs/Tool_TauConstituentSelector.h"
#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/Tool_InformationStore.h"


PanTau::Tool_TauConstituentSelector::Tool_TauConstituentSelector(const std::string& name) :
  asg::AsgTool(name),
  m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
{
  declareProperty("Tool_InformationStore", m_Tool_InformationStore, "Link to tool with all information");
  declareProperty("Tool_InformationStoreName", m_Tool_InformationStoreName, "Link to tool with all information");
}


PanTau::Tool_TauConstituentSelector::~Tool_TauConstituentSelector() {
}


StatusCode PanTau::Tool_TauConstituentSelector::initialize() {

  ATH_MSG_INFO(" initialize()");
  m_init=true;
    
  ATH_CHECK( HelperFunctions::bindToolHandle( m_Tool_InformationStore, m_Tool_InformationStoreName ) );
  ATH_CHECK( m_Tool_InformationStore.retrieve() );
    
  ATH_CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_MaxEta", m_MaxEta) );
    
  //eta bin edges
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_BinEdges_Eta", m_BinEdges_Eta) );
    
  //et cuts for types used in mode reco
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Neutral_EtaBinned_EtCut", m_Selection_Neutral_EtaBinned_EtCut) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut", m_Selection_Pi0Neut_EtaBinned_EtCut) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Charged_EtaBinned_EtCut", m_Selection_Charged_EtaBinned_EtCut) );
    
  //et cuts for types interesting for jet rej.
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_OutNeut_EtaBinned_EtCut", m_Selection_OutNeut_EtaBinned_EtCut) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_OutChrg_EtaBinned_EtCut", m_Selection_OutChrg_EtaBinned_EtCut) );
    
  //et cuts for neutral test types with lower et cuts
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_NeutLowA_EtaBinned_EtCut", m_Selection_NeutLowA_EtaBinned_EtCut) );
  ATH_CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_NeutLowB_EtaBinned_EtCut", m_Selection_NeutLowB_EtaBinned_EtCut) );    
    
  return StatusCode::SUCCESS;
} 


double PanTau::Tool_TauConstituentSelector::getEtCut(double eta, PanTau::TauConstituent::Type constituentType) const {
    
  for (unsigned int iEtaBin=0; iEtaBin<m_BinEdges_Eta.size()-1; iEtaBin++) {
    if (m_BinEdges_Eta[iEtaBin] <= eta && eta < m_BinEdges_Eta[iEtaBin+1]) {
      switch(constituentType) {
      case PanTau::TauConstituent::t_Charged:  return m_Selection_Charged_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_Neutral:  return m_Selection_Neutral_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_Pi0Neut:  return m_Selection_Pi0Neut_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_OutNeut:  return m_Selection_OutNeut_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_OutChrg:  return m_Selection_OutChrg_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_NeutLowA: return m_Selection_NeutLowA_EtaBinned_EtCut[iEtaBin];
      case PanTau::TauConstituent::t_NeutLowB: return m_Selection_NeutLowB_EtaBinned_EtCut[iEtaBin];
      default:
	return 9999999.;
      }
    }
  }
    
  ATH_MSG_WARNING("Eta value of " << eta << " could not be matched to any eta bin!");
  return 9999999.;
}


/**
 * Function to further select PFOs of the various categories (basically apply additional ET cuts): 
 * 
 */
StatusCode PanTau::Tool_TauConstituentSelector::SelectTauConstituents(const std::vector<TauConstituent*>& inputList,
								      std::vector<TauConstituent*>& outputList) const {    
  unsigned int nConst = inputList.size();

  for (unsigned int iConst=0; iConst<nConst; iConst++) {

    PanTau::TauConstituent* curConstituent = inputList[iConst];
        
    //general preselection:
    double curEta = std::abs( curConstituent->p4().Eta() );
    if (curEta > m_MaxEta) {
      ATH_MSG_DEBUG("\tNot using constituent with eta of " << curEta);
      continue;
    }
        
    bool passesSelection = false;

    // check if constituent is charged:
    if (curConstituent->isOfType(PanTau::TauConstituent::t_Charged)) {
      passesSelection = passesSelection_ChargedConstituent(curConstituent);
            
      // check if constituent is neutral, assign correctly pi0neut and neut flags:
    } else if (curConstituent->isOfType(PanTau::TauConstituent::t_Neutral)) {
      passesSelection = passesSelection_NeutralConstituent(curConstituent);
            
      //special treatment for the testing neutral flags
      //  a constituent can be a NeutLowA but not a neutral (because neutral Et cut is higher)
      //  => need that the constituent is in the seed for calculating test variables, but need it to NOT be tagged as neutral
      //  => remove the neutral-tag and check if it passes NeutLowA
      //-> repeat those steps for NeutLowB
      if (!passesSelection) {
	curConstituent->removeTypeFlag(PanTau::TauConstituent::t_Neutral);
	curConstituent->removeTypeFlag(PanTau::TauConstituent::t_Pi0Neut);
	passesSelection = passesSelection_NeutLowAConstituent(curConstituent);
	if (!passesSelection) {
	  curConstituent->removeTypeFlag(PanTau::TauConstituent::t_NeutLowA);
	  passesSelection = passesSelection_NeutLowBConstituent(curConstituent);
	}
      }            
      // apply further selection to constituent in isolation cone:
    } else if (curConstituent->isOfType(PanTau::TauConstituent::t_OutChrg)) {
      passesSelection = passesSelection_OutChrgConstituent(curConstituent);
            
    } else if (curConstituent->isOfType(PanTau::TauConstituent::t_OutNeut)) {
      passesSelection = passesSelection_OutNeutConstituent(curConstituent);
            
    } else {
      ATH_MSG_WARNING("Unhandled constituent type (" << curConstituent->getTypeNameString() 
		      << ") when trying to apply constituent selection - constituent will not be selected!");
      passesSelection = false;
    }
        
    if (!passesSelection) continue;
        
    outputList.push_back(inputList[iConst]);
  }
    
  return StatusCode::SUCCESS;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_NeutralConstituent(PanTau::TauConstituent* tauConstituent) const {

  TLorentzVector tlv_Constituent = tauConstituent->p4();
    
  double curEta       = tlv_Constituent.Eta();
  double cut_MinEt    = getEtCut(std::abs(curEta), PanTau::TauConstituent::t_Neutral);
  double curEt        = tlv_Constituent.Et();

  if (curEt < cut_MinEt) {
    ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
    return false;
  }

  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_Pi0NeutConstituent(PanTau::TauConstituent* tauConstituent) const {

  TLorentzVector tlv_Constituent = tauConstituent->p4();
    
  double curEta       = tlv_Constituent.Eta();
  double cut_MinEt    = getEtCut(std::abs(curEta), PanTau::TauConstituent::t_Pi0Neut);
  double curEt        = tlv_Constituent.Et();
  if (curEt < cut_MinEt) {
    ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
    return false;
  }
    
  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_ChargedConstituent(PanTau::TauConstituent* /*tauConstituent*/) const {
  // we want to use all tracks
  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_OutNeutConstituent(TauConstituent* TauConstituent) const {
  TLorentzVector tlv_Constituent = TauConstituent->p4();
    
  double curEta    = tlv_Constituent.Eta();
  double cut_MinEt = getEtCut(std::abs(curEta), PanTau::TauConstituent::t_OutNeut);
  double curEt     = tlv_Constituent.Et();
    
  if (curEt < cut_MinEt) return false;
  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_OutChrgConstituent(TauConstituent* /*TauConstituent*/) const {
  // we want to use all tracks
  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowAConstituent(TauConstituent* TauConstituent) const {
  TLorentzVector tlv_Constituent = TauConstituent->p4();
    
  double curEta    = tlv_Constituent.Eta();
  double cut_MinEt = getEtCut(std::abs(curEta), PanTau::TauConstituent::t_NeutLowA);
  double curEt     = tlv_Constituent.Pt();
    
  if (curEt < cut_MinEt) return false;
  return true;
}


bool PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowBConstituent(TauConstituent* TauConstituent) const {
  TLorentzVector tlv_Constituent = TauConstituent->p4();
    
  double curEta    = tlv_Constituent.Eta();
  double cut_MinEt = getEtCut(std::abs(curEta), PanTau::TauConstituent::t_NeutLowB);
  double curEt     = tlv_Constituent.Pt();
    
  if (curEt < cut_MinEt) return false;
  return true;
}
