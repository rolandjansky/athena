/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/EnergyCorrelatorGeneralizedRatiosTool.h"
#include "JetSubStructureUtils/EnergyCorrelatorGeneralized.h" 

using namespace std;
using fastjet::PseudoJet;

EnergyCorrelatorGeneralizedRatiosTool::EnergyCorrelatorGeneralizedRatiosTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
}

int EnergyCorrelatorGeneralizedRatiosTool::modifyJet(xAOD::Jet &jet) const {

  if (!jet.isAvailable<float>(m_prefix+"ECFG_2_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_4_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_1") ||

      !jet.isAvailable<float>(m_prefix+"ECFG_2_1_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_1_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_2_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_2_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_4_2_2") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_3_3_1") ||
      !jet.isAvailable<float>(m_prefix+"ECFG_4_4_1")) {
    ATH_MSG_WARNING("Energy correlation fractions with prefix '"<<m_prefix<<"' are not all available. Exiting..");
    return 1;
  }
  
  float ecfg_2_1 = jet.getAttribute<float>(m_prefix+"ECFG_2_1");
  float ecfg_3_2 = jet.getAttribute<float>(m_prefix+"ECFG_3_2");

  float ecfg_4_2 = jet.getAttribute<float>(m_prefix+"ECFG_4_2");
  float ecfg_3_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_1");

  // these ones for t/H discrimination
  float ecfg_2_1_2 = jet.getAttribute<float>(m_prefix+"ECFG_2_1_2");
  float ecfg_3_1_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_1_1");
  float ecfg_3_2_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_2_1");  
  float ecfg_3_2_2 = jet.getAttribute<float>(m_prefix+"ECFG_3_2_2");
  float ecfg_4_2_2 = jet.getAttribute<float>(m_prefix+"ECFG_4_2_2");
  float ecfg_3_3_1 = jet.getAttribute<float>(m_prefix+"ECFG_3_3_1");
  float ecfg_4_4_1 = jet.getAttribute<float>(m_prefix+"ECFG_4_4_1");

  // N2
    
  if(ecfg_2_1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"N2", ecfg_3_2  / (pow(ecfg_2_1, 2.0)));
  else
    jet.setAttribute(m_prefix+"N2", -999.0);

  // N3
  if(ecfg_3_1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"N3", ecfg_4_2  / (pow(ecfg_3_1, 2.0)));
  else
    jet.setAttribute(m_prefix+"N3", -999.0);

  // M2
  if(ecfg_2_1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"M2", ecfg_3_2  / ecfg_2_1);
  else
    jet.setAttribute(m_prefix+"M2", -999.0);

  // L-series variables
  // (experimental for ttH t/H discrimination)

  /*
    The exponents are determined in order to make 
    the whole thing dimensionless
    
    E = (a*n) / (b*m)
    for an ECFG_X_Y_Z, a=Y, n=Z

    e.g. for L1
    
    ecfg_3_3_1 / ecfg_2_1_2
    E = (3*1) / (1*2) = 3./2.
  */

  if(ecfg_2_1_2 > 1e-8) // Prevent div-0
    {
      jet.setAttribute(m_prefix+"L1", ecfg_3_2_1 / (pow(ecfg_2_1_2, (1.) )));
      jet.setAttribute(m_prefix+"L2", ecfg_3_3_1 / (pow(ecfg_2_1_2, (3./2.) )));
    }
  else
    {
      jet.setAttribute(m_prefix+"L1",-999.0);
      jet.setAttribute(m_prefix+"L2",-999.0);
    }

  if(ecfg_3_3_1 > 1e-8) // Prevent div-0
    {  
      jet.setAttribute(m_prefix+"L3", ecfg_3_1_1 / (pow(ecfg_3_3_1, (1./3.) )) );
      jet.setAttribute(m_prefix+"L4", ecfg_3_2_2 / (pow(ecfg_3_3_1, (4./3.) )) );
    }
  else
    {
      jet.setAttribute(m_prefix+"L3",-999.0);
      jet.setAttribute(m_prefix+"L4",-999.0);
    }

  if(ecfg_4_4_1 > 1e-8) // Prevent div-0
    jet.setAttribute(m_prefix+"L5", ecfg_4_2_2 / (pow(ecfg_4_4_1, (1.) )) );
  else
    jet.setAttribute(m_prefix+"L5",-999.0);
  
  return 0;
}
