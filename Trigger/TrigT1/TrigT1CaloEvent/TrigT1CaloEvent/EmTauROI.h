/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                          emtauroi.h  -  description
                             -------------------
    begin                : Mon Jan 22 2001
    email                : moyse@heppch.ph.qmw.ac.uk
 ***************************************************************************/


#ifndef EMTAUROI_H
#define EMTAUROI_H

// Gaudi kernel stuff.

#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#include "AthLinks/ElementLink.h"
#include "AthContainers/DataVector.h"


//LVL1 Calo trigger includes
#include "TrigT1CaloEvent/TriggerTower.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
                                                                 
namespace LVL1 {

/**This class defines the em/tau hadron ROI
which are generated by the LVL1 Calo Trigger.  It isn't the final RoI produced and passed to L2  -
that happens in the ROD simulation, so this class has some extra methods used for debugging

*@author moyse
*/

  class EmTauROI {

  public:
   
    // constructor
    EmTauROI(double phi, double eta, int energy, unsigned long int roiWord,const std::vector<unsigned int>& algorithmType,
      unsigned int hadCoreEnergy, unsigned int hadRingIsolationEnergy, unsigned int emRingIsolationEnergy,
      unsigned int emClusterEnergy, unsigned int tauClusterEnergy);

    // destructor
    ~EmTauROI();

    /** returns the 32bit ROI word. This is defined
	in the ATLAS note daq-98-089.*/
    unsigned int roiWord() const;

    /** returns TRUE if ROI constructed from a 
	trigger tower that was saturated - in other
	words this ROI is signalling that it does not
	know its correct energy. 
	*/
    bool saturated() const;

    /** returns phi coord of ROI */
    double phi() const;

    /** returns eta coord of ROI */
    double eta() const;
		
    /** returns the energy of the ROI core, i.e. the summed (in both layers)
    energy of the central 4 TriggerTowers*/
    int energy() const;

    /** returns TRUE if threshold number <em>threshold_number</em> has been passed 
	by this ROI. Thresholds numbered from 1 to TrigT1CaloDefs::numOfCPThresholds*/
    bool thresholdPassed(int thresh) const;
	
    /** returns type of thresholds number <em>threshold_number</em>
  Thresholds numbered from 1 to TrigT1CaloDefs::numOfCPThresholds*/
    TrigT1CaloDefs::ClusterAlgorithm thresholdType(int thresh) const;


 /** return energy */
  unsigned int emRingIsolationEnergy() const {return m_emRingIsolationEnergy;};
 /** return energy */
  unsigned int hadRingIsolationEnergy() const {return m_hadRingIsolationEnergy;} ;
 /** return energy */
  unsigned int hadCoreEnergy() const {return m_hadCoreEnergy;};
 /** return energy */
  unsigned int clusterEnergy() const {return m_emClusterEnergy;};
  unsigned int tauClusterEnergy() const {return m_tauClusterEnergy;};

  private: // Private attributes

    /** phi coord of ROI */
    double m_phi;

    /** eta coord of ROI */
    double m_eta;
		
 	/**Energy of ROI core, i.e. the summed (both layers)
 	energy of the central 4 TriggerTowers*/
 	int m_energy;

  /** this is the actual format of the data sent from
	the LVL1 hardware. See  ATL-DAQ-99-015 for
	further explanation. */
  unsigned long int m_roiWord;

    /** Contains the algo type of thresh sets. This should really be in a database.*/
  std::vector<unsigned int> m_algorithmType;

  /**Used for debugging only! Not present to higher triggers in hardware*/
  unsigned int m_emRingIsolationEnergy;
  /**Used for debugging only! Not present to higher triggers in hardware*/
	unsigned int m_hadRingIsolationEnergy;
  /**Used for debugging only! Not present to higher triggers in hardware*/
	unsigned int m_hadCoreEnergy;
  /**Used for debugging only! Not present to higher triggers in hardware*/
	unsigned int m_emClusterEnergy;
        unsigned int m_tauClusterEnergy;
private: // Private methods
  /** checks that the passed threshold is a valid number */
  void checkValidThreshNum(unsigned int thresh) const {if ( (thresh<1)||(thresh>TrigT1CaloDefs::numOfCPThresholds) ) std::cout <<"EmTauROI:: Threshold "<<thresh<<" outside range"<<std::endl;};

};
}//end of LVL1 namespace defn

#endif
