/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 RPCPadParameters
 -----------------------------------------
 ***************************************************************************/

#include <iostream>
#include "MuonRPC_Cabling/RPCPadParameters.h"

RPCPadParameters::RPCPadParameters(){
    //    std::cout << "RPCPadParameters constructor" << std::endl;
    m_eta_and_phi=true;
    m_feet_on=false;
    m_cma_mask=0x0; // no mask
    m_feet_thresholds=0x88; 
}

RPCPadParameters::RPCPadParameters(const RPCPadParameters& parameters)
{
    m_eta_and_phi=parameters.eta_and_phi();
    m_feet_on=parameters.feet_on();
    m_feet_thresholds=parameters.feet_thresholds();
    m_cma_mask=parameters.cma_mask();
}

RPCPadParameters::~RPCPadParameters() { }

RPCPadParameters&  RPCPadParameters::operator=(const RPCPadParameters& parameters)
{
    if(this!=&parameters){
      m_eta_and_phi=parameters.eta_and_phi();
      m_feet_on=parameters.feet_on();
      m_feet_thresholds=parameters.feet_thresholds();
      m_cma_mask=parameters.cma_mask();
    }
    return *this;
}

unsigned short int RPCPadParameters::feet_threshold(unsigned short int it) const {
    if (it>=3) return 0;
    return (0x7 & (m_feet_thresholds >> (3*it)));
}

bool RPCPadParameters::set_feet_threshold(unsigned short int it, unsigned short int th) {
    static short int mask[3]={0x1F8, 0x1c7, 0x3F };
    
    if (it>=3||th>=6) return false;
    
    m_feet_thresholds=(m_feet_thresholds&mask[it])+(th << (3*it));
    return true;
}

void  RPCPadParameters::Print(std::ostream& stream){
    stream << "Pad Parameters : " 
           << " eta_and_phi = " << m_eta_and_phi
           << " cma_mask = " << m_cma_mask
           << " feet_on = " << m_feet_on;
    if (m_feet_on) stream << " feet_thresholds = " << feet_threshold(0)
                       << " " << feet_threshold(1)
                       << " " << feet_threshold(2);
    stream << std::endl;
}


