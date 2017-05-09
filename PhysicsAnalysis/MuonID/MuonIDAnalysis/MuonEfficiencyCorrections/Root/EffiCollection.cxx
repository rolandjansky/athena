/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EffiCollection.cxx
 *
 *  Created on: Jun 21, 2014
 *      Author: goblirsc
 */

#include "MuonEfficiencyCorrections/EffiCollection.h"

CP::EffiCollection::EffiCollection():
m_central_eff(0),
m_forward_eff(0),
m_calo_eff(0),
m_lowpt_central_eff(0),
m_lowpt_calo_eff(0),
m_effType("EFF"),
m_lowpt_transition(20000.)
{
}

CP::EffiCollection::EffiCollection(std::string file_central, std::string file_calo,
        std::string file_forward,
        std::string file_lowpt_central,
        std::string file_lowpt_calo,
        std::map<std::string, double> lumis_central,
        std::map<std::string, double> lumis_calo,
        std::map<std::string, double> lumis_forward,
        std::map<std::string, double> lumis_lowpt_central,
        std::map<std::string, double> lumis_lowpt_calo,
        SystematicSet sys,
        std::string effType, double lowPtTransition):
                            m_central_eff(0),
                            m_forward_eff(0),
                            m_calo_eff(0),
                            m_lowpt_central_eff(0),
                            m_lowpt_calo_eff(0),
                            m_effType(effType),
                            m_lowpt_transition(lowPtTransition){

    double integral = 0;
    double integral_CT = 0;
    double integral_HighEta = 0;
    double integral_LowPt = 0;
    double integral_LowPtCalo = 0;

    bool isRecoEff = (effType == "EFF");

    // sum up the lumi weighted histos
    for (std::map<std::string, double>::iterator il = lumis_central.begin(); il != lumis_central.end();il++){
        // central muons
        if (!m_central_eff ) {
            m_central_eff = new EfficiencyScaleFactor(file_central,il->first,sys, m_effType,false,isRecoEff);
            m_central_eff->Scale(il->second);
            integral+=il->second;
        }
        else if (m_central_eff->AddPeriod(file_central,il->first,il->second) )
            integral+=il->second;
    }

    // calo muons
    for (std::map<std::string, double>::iterator il = lumis_calo.begin(); il != lumis_calo.end();il++){
        if (!m_calo_eff ) {
            m_calo_eff = new EfficiencyScaleFactor(file_calo,il->first,sys, m_effType,false,isRecoEff);
            m_calo_eff->Scale(il->second);
            integral_CT+=il->second;
        }
        else if (m_calo_eff->AddPeriod(file_calo,il->first,il->second) )
            integral_CT+=il->second;
    }
    // forward muons
    for (std::map<std::string, double>::iterator il = lumis_forward.begin(); il != lumis_forward.end();il++){
        if (!m_forward_eff ) {
            m_forward_eff = new EfficiencyScaleFactor(file_forward,il->first,sys,m_effType);
            m_forward_eff->Scale(il->second);
            integral_HighEta+=il->second;
        }
        else if (m_forward_eff->AddPeriod(file_forward,il->first,il->second) )
            integral_HighEta+=il->second;
    }
    
    // low-pt central muons
    if (m_lowpt_transition > 0){
        for (std::map<std::string, double>::iterator il = lumis_lowpt_central.begin(); il != lumis_lowpt_central.end();il++){
            if (!m_lowpt_central_eff ) {
                m_lowpt_central_eff = new EfficiencyScaleFactor(file_lowpt_central,il->first,sys, m_effType, true);  // last argument tells this to respond to dedicated low pt systematic variations
                m_lowpt_central_eff->Scale(il->second);
                integral_LowPt+=il->second;
            }
            else if (m_lowpt_central_eff->AddPeriod(file_lowpt_central,il->first,il->second) )
                integral_LowPt+=il->second;
        }
    }

    // low-pt CT muons
    if (m_lowpt_transition > 0){
        for (std::map<std::string, double>::iterator il = lumis_lowpt_calo.begin(); il != lumis_lowpt_calo.end();il++){
            if (!m_lowpt_calo_eff ) {
                m_lowpt_calo_eff = new EfficiencyScaleFactor(file_lowpt_calo,il->first,sys, m_effType, true);  // last argument tells this to respond to dedicated low pt systematic variations
                m_lowpt_calo_eff->Scale(il->second);
                integral_LowPtCalo+=il->second;
            }
            else if (m_lowpt_calo_eff->AddPeriod(file_lowpt_calo,il->first,il->second) )
                integral_LowPtCalo+=il->second;
        }
    }
    // now, turn the sums into a weighted average
    if (m_central_eff != NULL){
        m_central_eff->Scale(1./integral);
        m_central_eff->ApplySysVariation();
    }
    if (m_calo_eff != NULL){
        m_calo_eff->Scale(1./integral_CT);
        m_calo_eff->ApplySysVariation();
    }

    if (m_forward_eff != NULL){
        m_forward_eff->Scale(1./integral_HighEta);
        m_forward_eff->ApplySysVariation();
    }
    
    if (m_lowpt_transition > 0){
        if (m_lowpt_central_eff != NULL){
            m_lowpt_central_eff->Scale(1./integral_LowPt);
            m_lowpt_central_eff->ApplySysVariation();
        }
        if (m_lowpt_calo_eff != NULL){
            m_lowpt_calo_eff->Scale(1./integral_LowPtCalo);
            m_lowpt_calo_eff->ApplySysVariation();
        }
    }
}

bool CP::EffiCollection::CheckConsistency (){

    return (m_central_eff && m_central_eff->CheckConsistency() &&
            m_calo_eff && m_calo_eff->CheckConsistency() &&
            m_forward_eff && m_forward_eff->CheckConsistency() && 
                (m_lowpt_transition < 0 || (
                    m_lowpt_central_eff && m_lowpt_central_eff->CheckConsistency() &&
                    m_lowpt_calo_eff && m_lowpt_calo_eff->CheckConsistency())
                 )
              );
}


CP::EffiCollection::EffiCollection(const CP::EffiCollection & other){

    m_central_eff = new CP::EfficiencyScaleFactor(*(other.m_central_eff));
    m_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_calo_eff));
    m_forward_eff = new CP::EfficiencyScaleFactor(*(other.m_forward_eff));
    m_effType = other.m_effType;
    m_lowpt_transition = other.m_lowpt_transition;
    if (m_lowpt_transition > 0){
        m_lowpt_central_eff = new CP::EfficiencyScaleFactor(*(other.m_lowpt_central_eff));
        m_lowpt_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_lowpt_calo_eff));
    }
    else{
        m_lowpt_central_eff = 0;
        m_lowpt_calo_eff = 0;
    }

}
CP::EffiCollection & CP::EffiCollection::operator = (const CP::EffiCollection & other){

    if (this == &other){
        return *this;               
    }
    m_effType = other.m_effType;
    m_lowpt_transition = other.m_lowpt_transition;
    
    if (m_central_eff) delete m_central_eff;
    if (m_calo_eff) delete m_calo_eff;
    if (m_forward_eff) delete m_forward_eff;
    if (m_lowpt_central_eff) delete m_lowpt_central_eff;
    if (m_lowpt_calo_eff) delete m_lowpt_calo_eff;
    m_central_eff = new CP::EfficiencyScaleFactor(*(other.m_central_eff));
    m_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_calo_eff));
    m_forward_eff = new CP::EfficiencyScaleFactor(*(other.m_forward_eff));
    if (m_lowpt_transition > 0){
        m_lowpt_central_eff = new CP::EfficiencyScaleFactor(*(other.m_lowpt_central_eff));
        m_lowpt_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_lowpt_calo_eff));
    }
    else {
        m_lowpt_central_eff = 0;
        m_lowpt_calo_eff = 0;
    }
    return *this;

}
CP::EfficiencyScaleFactor* CP::EffiCollection::operator ()(const xAOD::Muon& mu) {

	// yes, we need the second check - negative pt does happen!
    if (mu.pt() < m_lowpt_transition && m_lowpt_transition > 0){
        if (mu.author() == xAOD::Muon::CaloTag){
            return m_lowpt_calo_eff;
        }
        return m_lowpt_central_eff;
    }
    if (mu.author() == xAOD::Muon::CaloTag){
        return m_calo_eff;
    }
    else if (fabs(mu.eta())<2.5){
        return m_central_eff;
    }
    else{
        return m_forward_eff;
    }
}
std::string CP::EffiCollection::sysname (void){
    if (m_central_eff) return m_central_eff->sysname();
    else return "";
}

CP::EffiCollection::~EffiCollection() {

    if (m_central_eff) delete m_central_eff;
    if (m_calo_eff) delete m_calo_eff;
    if (m_forward_eff) delete m_forward_eff;
    if (m_lowpt_central_eff) delete m_lowpt_central_eff;
    if (m_lowpt_calo_eff) delete m_lowpt_calo_eff;
}
