/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          ModuleEnergy.cxx  -  description
                             -------------------
    begin                : Tues Sep 4 2007
                           Loosely based on Ed Moyse's JetEnergyModule
    email                : Alan.Watson@cern.ch
 ***************************************************************************/

#include "TrigT1CaloUtils/ModuleEnergy.h"
#include "TrigT1CaloUtils/QuadLinear.h"

#include <math.h>

namespace LVL1 {

ModuleEnergy::ModuleEnergy(const xAOD::JetElementMap_t* JEContainer, unsigned int crate,
                           unsigned int module, int JEThresholdEtSum, int JEThresholdEtMiss, const std::map<int, int>* TEMasks, int slice):
  m_jetElementThresholdEtSum(JEThresholdEtSum),
  m_jetElementThresholdEtMiss(JEThresholdEtMiss),
  m_Et(0),
  m_Ex(0),
  m_Ey(0),
  m_signX(1),
  m_signY(1),
  m_crate(crate),
  m_module(module),
  m_debug(false)
{
  // Only fill ModuleEnergys where crate, module numbers valid
  if (m_debug) std::cout << "Create ModuleEnergy for crate "
                         << crate << " module " << module << std::endl;
  if (m_crate <= 1 && m_module <= 15) {
    /** Set up Ex, Ey signs for this module */
    m_signY = ( (module < 8) ? 1 : -1 );
    m_signX = ( (crate == 0) ? m_signY : -m_signY );
    /** Look for the JetElements associated with this module */
    bool saturated = false;
    JetEnergyModuleKey get;
    std::vector<unsigned int> keys = get.jeKeys(crate, module);
    std::vector<unsigned int>::const_iterator it = keys.begin();
    for (; it != keys.end(); it++) {
      xAOD::JetElementMap_t::const_iterator test=JEContainer->find(*it);
      if (test != JEContainer->end()) {
	/** Check JE not masked in TE trigger */
	double eta = test->second->eta();
	int ieta = int((eta + (eta>0 ? 0.005 : -0.005))/0.1);
	/// Use jet element if it is not in the masked list
	if (TEMasks == 0 || TEMasks->find(ieta) == TEMasks->end()) {
          /** Test for saturation */
          if (test->second->isSaturated()) {
	    saturated = true;
          }
          else {
            /** Get ET for requested time slice */
            int jetElementET;
            if (slice < 0) jetElementET = test->second->et();
            else           jetElementET = test->second->sliceET(slice);
            /** Test against ETmiss algorithm threshold */
            if (jetElementET > m_jetElementThresholdEtMiss) {
              /** Get sin, cos factors for this JetElement */
              double phi = test->second->phi();
              int cosPhi = 0;
              int sinPhi = 0;
              getSinCos(eta,phi,cosPhi,sinPhi);
                /** Convert to Ex, Ey and sum. Replicate integer arithmetic used in hardware */
              unsigned int EnergyX = ((jetElementET*cosPhi) & 0x7ffffc00);
              unsigned int EnergyY = ((jetElementET*sinPhi) & 0x7ffffc00);
              m_Ex += EnergyX;
              m_Ey += EnergyY;
              if (m_debug) std::cout << "JE phi = " << phi << ", ET = " << jetElementET
                                   << ", Ex = " << (EnergyX>>10) << ", Ey = " << (EnergyY>>10) << std::endl;
            }
            /** Test against ETsum algorithm threshold & add if passes */
            if ( jetElementET > m_jetElementThresholdEtSum )
               m_Et += jetElementET;
          } // End of processing of unsaturated jetElement
        } // End processing of unvetoed element
      } // End check for end of container
    } // End loop through keys

    /** Convert Ex, Ey back to 1 GeV/count integers*/
    m_Ex = m_Ex>>12;
    m_Ey = m_Ey>>12;
    
    /** Check for overflows. Set ET to full scale if these occur */
    if (saturated || (m_Ex > (1<<m_EtBits)-1)) m_Ex = (1<<m_EtBits)-1;
    if (saturated || (m_Ey > (1<<m_EtBits)-1)) m_Ey = (1<<m_EtBits)-1;
    if (saturated || (m_Et > (1<<m_EtBits)-1)) m_Et = (1<<m_EtBits)-1;

    if (m_debug) {
      std::cout << "Crate " << crate << " Module " << module <<
                   " sums: " << std::endl;
      std::cout << "  Ex = " << static_cast<int>(m_Ex)*m_signX << std::endl; 
      std::cout << "  Ey = " << static_cast<int>(m_Ey)*m_signY << std::endl;
      std::cout << "  Et = " << m_Et << std::endl;
    }
  }
}

ModuleEnergy::ModuleEnergy(unsigned int crate, unsigned int module,
                           unsigned int et, unsigned int ex,
			   unsigned int ey) :
  m_jetElementThresholdEtSum(0),
  m_jetElementThresholdEtMiss(0),
  m_Et(et),
  m_Ex(ex),
  m_Ey(ey),
  m_signX(1),
  m_signY(1),
  m_crate(crate),
  m_module(module),
  m_debug(false)
{
  // Only fill ModuleEnergys where crate, module numbers valid
  if (m_debug) std::cout << "Create ModuleEnergy for crate "
                         << crate << " module " << module << std::endl;
  if (m_crate <= 1 && m_module <= 15) {
    /** Set up Ex, Ey signs for this module */
    m_signY = ( (module < 8) ? 1 : -1 );
    m_signX = ( (crate == 0) ? m_signY : -m_signY );
    
    /** Check for overflows. Set ET to full scale if these occur */
    if (m_Ex > (1<<m_EtBits)-1) m_Ex = (1<<m_EtBits)-1;
    if (m_Ey > (1<<m_EtBits)-1) m_Ey = (1<<m_EtBits)-1;
    if (m_Et > (1<<m_EtBits)-1) m_Et = (1<<m_EtBits)-1;
    
    if (m_debug) {
      std::cout << "Crate " << crate << " Module " << module <<
                   " sums: " << std::endl;
      std::cout << "  Ex = " << static_cast<int>(m_Ex)*m_signX << std::endl; 
      std::cout << "  Ey = " << static_cast<int>(m_Ey)*m_signY << std::endl;
      std::cout << "  Et = " << m_Et << std::endl;
    }
  }
}


ModuleEnergy::~ModuleEnergy(){
}

/** return crate number */
unsigned int ModuleEnergy::crate() const {
  return m_crate;
}
/** return module number */
unsigned int ModuleEnergy::module() const {
  return m_module;
}

/** return Et, Ex, Ey sums of contained JEs (up to 32 JEs) */
unsigned int ModuleEnergy::et() const {
  return m_Et;
}
unsigned int ModuleEnergy::ex() const {
  return m_Ex;
}
unsigned int ModuleEnergy::ey() const {
  return m_Ey;
}

/** return signs of Ex and Ey for this module */
int ModuleEnergy::signX() const {
  return m_signX;
}
int ModuleEnergy::signY() const {
  return m_signY;
}

/** return cos, sin coefficients for a given JetElement */
void ModuleEnergy::getSinCos(double eta, double phi, int& cosPhi, int& sinPhi) {
    
  /** Different phi granularities in central and forward calorimeters */
  unsigned int SinCos[8]    = {401,1187,1928,2594,3161,3607,3913,4070};
  unsigned int fwdSinCos[4] = {794,2261,3384,3992};

  /** Each module spans 1 quadrant in phi. Hence want phi position relative to module edge */
  float modPhi = fmod(phi, M_PI/2.);

  /// Barrel and endcap calorimeters
  if (fabs(eta) < 3.2) {
    int phiBin = (int)(modPhi*16/M_PI);
    if (m_crate > 0) {                  /// Even quadrants, modPhi measured from horizontal
       cosPhi = SinCos[phiBin];
       sinPhi = SinCos[7-phiBin];
    }
    else {                              /// Odd quadrants, modPhi measured from vertical
       cosPhi = SinCos[7-phiBin];
       sinPhi = SinCos[phiBin];
    }
  }
  /// Forward calorimeters
  else {
    int phiBin = (int)(modPhi*8/M_PI);
    if (m_crate > 0) {
       cosPhi = fwdSinCos[phiBin];
       sinPhi = fwdSinCos[3-phiBin];
    }
    else {
       cosPhi = fwdSinCos[3-phiBin];
       sinPhi = fwdSinCos[phiBin];
    }
  }

} // end of getSinCos

} // end of namespace bracket


