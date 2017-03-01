/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! PanTau includes
#include "PanTauAlgs/Tool_TauConstituentSelector.h"

#include "PanTauAlgs/TauConstituent.h"
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/Tool_InformationStore.h"

#include <math.h>


PanTau::Tool_TauConstituentSelector::Tool_TauConstituentSelector(
    const std::string& name ) :
        asg::AsgTool(name),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
{
    declareProperty("Tool_InformationStore",    m_Tool_InformationStore,   "Link to tool with all information");
    declareProperty("Tool_InformationStoreName",    m_Tool_InformationStoreName,   "Link to tool with all information");
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


double PanTau::Tool_TauConstituentSelector::getEtCut(double eta, PanTau::TauConstituent2::Type constituentType) const {
    
    for(unsigned int iEtaBin=0; iEtaBin<m_BinEdges_Eta.size()-1; iEtaBin++) {
        if(m_BinEdges_Eta[iEtaBin] <= eta && eta < m_BinEdges_Eta[iEtaBin+1]) {
            switch(constituentType) {
                case PanTau::TauConstituent2::t_Charged:  return m_Selection_Charged_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_Neutral:  return m_Selection_Neutral_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_Pi0Neut:  return m_Selection_Pi0Neut_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_OutNeut:  return m_Selection_OutNeut_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_OutChrg:  return m_Selection_OutChrg_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_NeutLowA: return m_Selection_NeutLowA_EtaBinned_EtCut[iEtaBin];
                case PanTau::TauConstituent2::t_NeutLowB: return m_Selection_NeutLowB_EtaBinned_EtCut[iEtaBin];
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
StatusCode PanTau::Tool_TauConstituentSelector::SelectTauConstituents(  std::vector<TauConstituent2*> inputList,
                                                                        std::vector<TauConstituent2*>& outputList) const {
    
    unsigned int nConst = inputList.size();
    ATH_MSG_DEBUG("Perform tau constituent selection on " << nConst << "constituents");
    for(unsigned int iConst=0; iConst<nConst; iConst++) {
        ATH_MSG_DEBUG("===>Constituent " << iConst << " / " << nConst);
        
        PanTau::TauConstituent2*         curConstituent  = inputList[iConst];
        
        //general preselection:
        double curEta = fabs( curConstituent->p4().Eta() );
        if(curEta > m_MaxEta) {
            ATH_MSG_DEBUG("\tNot using constituent with eta of " << curEta);
            continue;
        }
        
        bool passesSelection = false;
        ATH_MSG_DEBUG("Before selection, constituent is of type: " << curConstituent->getTypeNameString() );

	// check if constituent is charged:
        if(curConstituent->isOfType(PanTau::TauConstituent2::t_Charged) == true) {
            passesSelection = passesSelection_ChargedConstituent(curConstituent);
            
 	// check if constituent is neutral, assign correctly pi0neut and neut flags:
        } else if(curConstituent->isOfType(PanTau::TauConstituent2::t_Neutral) == true) {
            passesSelection = passesSelection_NeutralConstituent(curConstituent);
            
            //special treatment for the testing neutral flags
            //  a constituent can be a NeutLowA but not a neutral (because neutral Et cut is higher)
            //  => need that the constituent is in the seed for calculating test variables, but need it to NOT be tagged as neutral
            //  => remove the neutral-tag and check if it passes NeutLowA
            //-> repeat those steps for NeutLowB
            if(passesSelection == false) {
                curConstituent->removeTypeFlag(PanTau::TauConstituent2::t_Neutral);
                curConstituent->removeTypeFlag(PanTau::TauConstituent2::t_Pi0Neut);
                passesSelection = passesSelection_NeutLowAConstituent(curConstituent);
                if(passesSelection == false) {
                    curConstituent->removeTypeFlag(PanTau::TauConstituent2::t_NeutLowA);
                    passesSelection = passesSelection_NeutLowBConstituent(curConstituent);
                }
            }
            

 	// apply further selection to constituent in isolation cone:
        } else if(curConstituent->isOfType(PanTau::TauConstituent2::t_OutChrg) == true) {
            passesSelection = passesSelection_OutChrgConstituent(curConstituent);
            
        } else if(curConstituent->isOfType(PanTau::TauConstituent2::t_OutNeut) == true) {
            passesSelection = passesSelection_OutNeutConstituent(curConstituent);
            
        } else {
            ATH_MSG_WARNING("Unhandled constituent type (" << curConstituent->getTypeNameString() << ") when trying to apply constituent selection - constituent will not be selected!");
            passesSelection = false;
        }
        ATH_MSG_DEBUG("After selection (with result " << passesSelection << "), constituent is of type: " << curConstituent->getTypeNameString() );
        
        if(passesSelection == false) continue;
        
        outputList.push_back(inputList[iConst]);
    }
    
    ATH_MSG_DEBUG("Out of " << nConst << " contituents, " << outputList.size() << " passed the selection");
    
    return StatusCode::SUCCESS;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutralConstituent(PanTau::TauConstituent2* tauConstituent) const {
    ATH_MSG_DEBUG("neutral sel for const at: " << tauConstituent);
    TLorentzVector tlv_Constituent = tauConstituent->p4();
    
    double curEta       = tlv_Constituent.Eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_Neutral);
    double curEt        = tlv_Constituent.Et();
    ATH_MSG_DEBUG("curEt (" << curEt << ") < cut_MinEt (" << cut_MinEt << ") ? ");
//    ATH_MSG_DEBUG("curEt (" << curEt << ") < cut_MinEt (" << cut_MinEt << ") ? --- with curEta=" << curEta << "PanTau::TauConstituent2::t_Neutral=" << PanTau::TauConstituent2::t_Neutral);
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }

    ATH_MSG_DEBUG("Passed!");
    
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_Pi0NeutConstituent(PanTau::TauConstituent2* tauConstituent) const {
    ATH_MSG_DEBUG("pi0neut sel for const at: " << tauConstituent);
    TLorentzVector tlv_Constituent = tauConstituent->p4();
    
    double curEta       = tlv_Constituent.Eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_Pi0Neut);
    double curEt        = tlv_Constituent.Et();
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }
    
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_ChargedConstituent(PanTau::TauConstituent2* tauConstituent) const {

  TLorentzVector tlv_Constituent = tauConstituent->p4();

  // we want to use all tracks
  return true;

  /*
    
    double curEta       = tlv_Constituent.Eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_Charged);
    double curEt        = tlv_Constituent.Pt();
    
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }
    
    return true;

  */

}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_OutNeutConstituent(TauConstituent2* TauConstituent) const {
    TLorentzVector tlv_Constituent = TauConstituent->p4();
    
    double curEta    = tlv_Constituent.Eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_OutNeut);
    double curEt     = tlv_Constituent.Et();
    
    if(curEt < cut_MinEt) return false;
    return true;
}


bool    PanTau::Tool_TauConstituentSelector::passesSelection_OutChrgConstituent(TauConstituent2* TauConstituent) const {

  TLorentzVector tlv_Constituent = TauConstituent->p4();

  // we want to use all tracks
  return true;

  /*
    
    double curEta    = tlv_Constituent.Eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_OutChrg);
    double curEt     = tlv_Constituent.Pt();
    
    if(curEt < cut_MinEt) return false;
    return true;

  */
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowAConstituent(TauConstituent2* TauConstituent) const {
    TLorentzVector tlv_Constituent = TauConstituent->p4();
    
    double curEta    = tlv_Constituent.Eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_NeutLowA);
    double curEt     = tlv_Constituent.Pt();
    
    if(curEt < cut_MinEt) return false;
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowBConstituent(TauConstituent2* TauConstituent) const {
    TLorentzVector tlv_Constituent = TauConstituent->p4();
    
    double curEta    = tlv_Constituent.Eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent2::t_NeutLowB);
    double curEt     = tlv_Constituent.Pt();
    
    if(curEt < cut_MinEt) return false;
    return true;
}



