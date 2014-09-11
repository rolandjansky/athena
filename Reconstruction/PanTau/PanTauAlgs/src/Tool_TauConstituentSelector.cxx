/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//! Gaudi includes
#include "AthenaKernel/errorcheck.h"

//! PanTau includes
#include "PanTauAlgs/Tool_TauConstituentSelector.h"

#include "PanTauEvent/TauConstituent.h"
#include "PanTauAlgs/Tool_InformationStore.h"

#include <math.h>


PanTau::Tool_TauConstituentSelector::Tool_TauConstituentSelector(
    const std::string& ty,
    const std::string& na,
    const IInterface* pa ) :
        AthAlgTool(ty,na,pa),
        m_Tool_InformationStore("PanTau::Tool_InformationStore/Tool_InformationStore")
{
    declareInterface<ITool_TauConstituentSelector>(this);

    declareProperty("Tool_InformationStore",    m_Tool_InformationStore,   "Link to tool with all information");
}

PanTau::Tool_TauConstituentSelector::~Tool_TauConstituentSelector() {
}

StatusCode PanTau::Tool_TauConstituentSelector::initialize() {

    CHECK( AlgTool::initialize() );
    ATH_MSG_INFO(" initialize()");
    
    CHECK( m_Tool_InformationStore.retrieve() );
    
    CHECK( m_Tool_InformationStore->getInfo_Double("TauConstituents_MaxEta", m_MaxEta) );
    
    //eta bin edges
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_BinEdges_Eta", m_BinEdges_Eta) );
    
    //et cuts for types used in mode reco
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Neutral_EtaBinned_EtCut", m_Selection_Neutral_EtaBinned_EtCut) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut", m_Selection_Pi0Neut_EtaBinned_EtCut) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_Charged_EtaBinned_EtCut", m_Selection_Charged_EtaBinned_EtCut) );
    
    //et cuts for types interesting for jet rej.
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_OutNeut_EtaBinned_EtCut", m_Selection_OutNeut_EtaBinned_EtCut) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_OutChrg_EtaBinned_EtCut", m_Selection_OutChrg_EtaBinned_EtCut) );
    
    //et cuts for neutral test types with lower et cuts
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_NeutLowA_EtaBinned_EtCut", m_Selection_NeutLowA_EtaBinned_EtCut) );
    CHECK( m_Tool_InformationStore->getInfo_VecDouble("TauConstituents_Selection_NeutLowB_EtaBinned_EtCut", m_Selection_NeutLowB_EtaBinned_EtCut) );
    
    
    return StatusCode::SUCCESS;
} 


double PanTau::Tool_TauConstituentSelector::getEtCut(double eta, PanTau::TauConstituent::Type constituentType) const {
    
    for(unsigned int iEtaBin=0; iEtaBin<m_BinEdges_Eta.size()-1; iEtaBin++) {
        if(m_BinEdges_Eta[iEtaBin] <= eta && eta < m_BinEdges_Eta[iEtaBin+1]) {
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



StatusCode PanTau::Tool_TauConstituentSelector::SelectTauConstituents(  std::vector<TauConstituent*> inputList,
                                                                        std::vector<TauConstituent*>& outputList) const {
    
    unsigned int nConst = inputList.size();
    ATH_MSG_DEBUG("Perform tau constituent selection on " << nConst << "constituents");
    for(unsigned int iConst=0; iConst<nConst; iConst++) {
        ATH_MSG_DEBUG("===>Constituent " << iConst << " / " << nConst);
        
        PanTau::TauConstituent*         curConstituent  = inputList[iConst];
        
        //general preselection:
        double curEta = fabs( curConstituent->hlv().eta() );
        if(curEta > m_MaxEta) {
            ATH_MSG_DEBUG("\tNot using constituent with eta of " << curEta);
            continue;
        }
        
        bool passesSelection = false;
        ATH_MSG_DEBUG("Before selection, constituent is of type: " << curConstituent->getTypeNameString() );
        if(curConstituent->isOfType(PanTau::TauConstituent::t_Charged) == true) {
            passesSelection = passesSelection_ChargedConstituent(curConstituent);
            
        } else if(curConstituent->isOfType(PanTau::TauConstituent::t_Neutral) == true) {
            passesSelection = passesSelection_NeutralConstituent(curConstituent);
            
            //special treatment for the testing neutral flags
            //  a constituent can be a NeutLowA but not a neutral (because neutral Et cut is higher)
            //  => need that the constituent is in the seed for calculating test variables, but need it to NOT be tagged as neutral
            //  => remove the neutral-tag and check if it passes NeutLowA
            //-> repeat those steps for NeutLowB
            if(passesSelection == false) {
                curConstituent->removeTypeFlag(PanTau::TauConstituent::t_Neutral);
                curConstituent->removeTypeFlag(PanTau::TauConstituent::t_Pi0Neut);
                passesSelection = passesSelection_NeutLowAConstituent(curConstituent);
                if(passesSelection == false) {
                    curConstituent->removeTypeFlag(PanTau::TauConstituent::t_NeutLowA);
                    passesSelection = passesSelection_NeutLowBConstituent(curConstituent);
                }
            }
            
        } else if(curConstituent->isOfType(PanTau::TauConstituent::t_OutChrg) == true) {
            passesSelection = passesSelection_OutChrgConstituent(curConstituent);
            
        } else if(curConstituent->isOfType(PanTau::TauConstituent::t_OutNeut) == true) {
            passesSelection = passesSelection_OutNeutConstituent(curConstituent);
            
        } else {
            ATH_MSG_WARNING("Unhandled constituent type (" << curConstituent->getTypeNameString() << ") when trying to apply constituent selection - constituent will not be selected!");
            passesSelection = false;
        }
        ATH_MSG_DEBUG("After selection (with result " << passesSelection << "), constituent is of type: " << curConstituent->getTypeNameString() );
        
        if(passesSelection == false) {continue;}
        
        outputList.push_back(inputList[iConst]);
    }
    
    ATH_MSG_DEBUG("Out of " << nConst << " contituents, " << outputList.size() << " passed the selection");
    
    return StatusCode::SUCCESS;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutralConstituent(PanTau::TauConstituent* tauConstituent) const {
    ATH_MSG_DEBUG("neutral sel for const at: " << tauConstituent);
    CLHEP::HepLorentzVector hlv_Constituent = tauConstituent->hlv();
    
    double curEta       = hlv_Constituent.eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent::t_Neutral);
    double curEt        = hlv_Constituent.et();
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }
    
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_Pi0NeutConstituent(PanTau::TauConstituent* tauConstituent) const {
    ATH_MSG_DEBUG("pi0neut sel for const at: " << tauConstituent);
    CLHEP::HepLorentzVector hlv_Constituent = tauConstituent->hlv();
    
    double curEta       = hlv_Constituent.eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent::t_Pi0Neut);
    double curEt        = hlv_Constituent.et();
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }
    
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_ChargedConstituent(PanTau::TauConstituent* tauConstituent) const {
    CLHEP::HepLorentzVector hlv_Constituent = tauConstituent->hlv();
    
    double curEta       = hlv_Constituent.eta();
    double cut_MinEt    = getEtCut(fabs(curEta), PanTau::TauConstituent::t_Charged);
    double curEt        = hlv_Constituent.perp();
    
    if(curEt < cut_MinEt) {
        ATH_MSG_DEBUG("\tNot using constituent at eta " << curEta << " with et of " << curEt);
        return false;
    }
    
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_OutNeutConstituent(TauConstituent* TauConstituent) const {
    CLHEP::HepLorentzVector hlv_Constituent = TauConstituent->hlv();
    
    double curEta    = hlv_Constituent.eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent::t_OutNeut);
    double curEt     = hlv_Constituent.et();
    
    if(curEt < cut_MinEt) return false;
    return true;
}


bool    PanTau::Tool_TauConstituentSelector::passesSelection_OutChrgConstituent(TauConstituent* TauConstituent) const {
    CLHEP::HepLorentzVector hlv_Constituent = TauConstituent->hlv();
    
    double curEta    = hlv_Constituent.eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent::t_OutChrg);
    double curEt     = hlv_Constituent.perp();
    
    if(curEt < cut_MinEt) return false;
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowAConstituent(TauConstituent* TauConstituent) const {
    CLHEP::HepLorentzVector hlv_Constituent = TauConstituent->hlv();
    
    double curEta    = hlv_Constituent.eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent::t_NeutLowA);
    double curEt     = hlv_Constituent.perp();
    
    if(curEt < cut_MinEt) return false;
    return true;
}



bool    PanTau::Tool_TauConstituentSelector::passesSelection_NeutLowBConstituent(TauConstituent* TauConstituent) const {
    CLHEP::HepLorentzVector hlv_Constituent = TauConstituent->hlv();
    
    double curEta    = hlv_Constituent.eta();
    double cut_MinEt = getEtCut(fabs(curEta), PanTau::TauConstituent::t_NeutLowB);
    double curEt     = hlv_Constituent.perp();
    
    if(curEt < cut_MinEt) return false;
    return true;
}



