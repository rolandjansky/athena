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
m_calo_eff(0)
{
}

CP::EffiCollection::EffiCollection(std::string file_central, std::string file_calo,
        std::string file_forward,
        std::map<std::string, double> lumis_central,
        std::map<std::string, double> lumis_calo,
        std::map<std::string, double> lumis_forward,
        SystematicSet sys):
			                m_central_eff(0),
			                m_forward_eff(0),
			                m_calo_eff(0) {

    double integral = 0;
    double integral_CT = 0;
    double integral_HighEta = 0;

    // sum up the lumi weighted histos
    for (std::map<std::string, double>::iterator il = lumis_central.begin(); il != lumis_central.end();il++){
        // central muons
        if (!m_central_eff ) {
            m_central_eff = new EfficiencyScaleFactor(file_central,il->first,sys);
            m_central_eff->Scale(il->second);
            integral+=il->second;
        }
        else if (m_central_eff->AddPeriod(file_central,il->first,il->second) )
            integral+=il->second;
    }

    // calo muons
    for (std::map<std::string, double>::iterator il = lumis_calo.begin(); il != lumis_calo.end();il++){
        if (!m_calo_eff ) {
            m_calo_eff = new EfficiencyScaleFactor(file_calo,il->first,sys);
            m_calo_eff->Scale(il->second);
            integral_CT+=il->second;
        }
        else if (m_calo_eff->AddPeriod(file_calo,il->first,il->second) )
            integral_CT+=il->second;
    }
    // forward muons
    for (std::map<std::string, double>::iterator il = lumis_forward.begin(); il != lumis_forward.end();il++){
        if (!m_forward_eff ) {
            m_forward_eff = new EfficiencyScaleFactor(file_forward,il->first,sys);
            m_forward_eff->Scale(il->second);
            integral_HighEta+=il->second;
        }
        else if (m_forward_eff->AddPeriod(file_forward,il->first,il->second) )
            integral_HighEta+=il->second;
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
}

bool CP::EffiCollection::CheckConsistency (){

    return (m_central_eff->CheckConsistency() &&
            m_calo_eff->CheckConsistency() &&
            m_forward_eff->CheckConsistency());
}


CP::EffiCollection::EffiCollection(const CP::EffiCollection & other){

    m_central_eff = new CP::EfficiencyScaleFactor(*(other.m_central_eff));
    m_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_calo_eff));
    m_forward_eff = new CP::EfficiencyScaleFactor(*(other.m_forward_eff));

}
CP::EffiCollection & CP::EffiCollection::operator = (const CP::EffiCollection & other){

    if (this == &other){
        return *this;               
    }
    delete m_central_eff;
    delete m_calo_eff;
    delete m_forward_eff;
    m_central_eff = new CP::EfficiencyScaleFactor(*(other.m_central_eff));
    m_calo_eff = new CP::EfficiencyScaleFactor(*(other.m_calo_eff));
    m_forward_eff = new CP::EfficiencyScaleFactor(*(other.m_forward_eff));
    return *this;

}
CP::EfficiencyScaleFactor* CP::EffiCollection::operator ()(const xAOD::Muon& mu) {


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
}
