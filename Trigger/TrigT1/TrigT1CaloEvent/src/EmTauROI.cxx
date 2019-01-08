/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          emtauroi.cxx  -  description
                             -------------------
    begin                : Mon Jan 22 2001
    email                : moyse@heppch.ph.qmw.ac.uk
 ***************************************************************************/


#include "TrigT1CaloEvent/EmTauROI.h"

namespace LVL1 {

EmTauROI::EmTauROI(double phi, double eta, int energy, unsigned long int roiWord, std::vector<unsigned int> algorithmType,
  unsigned int hadCoreEnergy, unsigned int hadRingIsolationEnergy, unsigned int emRingIsolationEnergy,
  unsigned int emClusterEnergy, unsigned int tauClusterEnergy):
  m_phi(phi),
  m_eta(eta),
  m_energy(energy),
  m_roiWord(roiWord),
  m_algorithmType(algorithmType),
  m_emRingIsolationEnergy(emRingIsolationEnergy),
  m_hadRingIsolationEnergy(hadRingIsolationEnergy),
  m_hadCoreEnergy(hadCoreEnergy),
  m_emClusterEnergy(emClusterEnergy),
  m_tauClusterEnergy(tauClusterEnergy)
{
}

EmTauROI::~EmTauROI(){
}

/** returns the 32bit ROI word. This is defined
in the ATLAS note daq-98-089.<p>*/
unsigned int LVL1::EmTauROI::roiWord() const{
  return m_roiWord;
}


/** returns TRUE if threshold number threshold_number has been passed by
this ROI. */
bool LVL1::EmTauROI::thresholdPassed(int thresh) const {

	if (m_roiWord & (1<<(thresh-1)) ){
		return true;
	}else{
		return false;
	}
}

/** returns type of thresholds number <em>threshold_number</em>.*/
TrigT1CaloDefs::ClusterAlgorithm LVL1::EmTauROI::thresholdType(int thresh) const{
	// This information is currently stored as a vector 
	// within all EmTauROIs. This is not ideal - I
	// would rather it was looked up from a database.
  // maybe I should just look it up from CTPCaloConfig?
	if (thresh<1 || thresh > int(TrigT1CaloDefs::numOfCPThresholds)){
		// outside allowed values.
		return TrigT1CaloDefs::ClustAlgError;
	}
	switch ( m_algorithmType[thresh-1] ){
    case 0 : return TrigT1CaloDefs::EMAlg;
             break;
    case 1 : return TrigT1CaloDefs::TauAlg;
             break;
    default : return TrigT1CaloDefs::ClustAlgError;
  }//end switch
		
	// shouldn't ever be reached.
	return TrigT1CaloDefs::ClustAlgError;
}


/** returns eta coord of ROI */
double LVL1::EmTauROI::eta() const {
  return m_eta;
}

/** returns phi coord of ROI */
double LVL1::EmTauROI::phi() const {
  return m_phi;
}


/**returns energy of ROI core*/
int LVL1::EmTauROI::energy() const {
	return m_energy;
}

/** returns TRUE if ROI constructed from a 
trigger tower that was saturated - in other
words this ROI is signalling that it does not
know its correct energy.
*/
bool LVL1::EmTauROI::saturated() const {
  // Saturation flag is bit 16 of RoIWord
  bool sat = (m_roiWord >> 16)&0x1;
  return sat;
}


}//end of LVL1 namespace defn

