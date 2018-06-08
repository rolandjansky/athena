/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           TriggerTower.h  -  description
 //                              -------------------
 //     begin                : Mon Sep 25 2000
 //     email                : e.moyse@qmw.ac.uk
 //  ***************************************************************************/
  
  
#ifndef TriggerTower_H
#define TriggerTower_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigT1Interfaces/Coordinate.h"

  
namespace LVL1 {
   
   //Doxygen class description below:
   /**
   Trigger towers are the inputs to all other parts of the calorimeter trigger.
   They are formed by analogue summation of cells (represented in simulation
   by LArTTL1 and TileTTL1 objects). The L1Calo PreProcessor digitizes these,
   performs Bunch-Crossing IDentification, final calibration and reduction
   to 8 bit words.

   The TriggerTower class represents the output of the PreProcessor.
   Each TriggerTower object actually contains information from a pair of 
   trigger towers, one EM and one Hadronic, sharing the same eta, phi 
   coordinates. This is convenient for algorithm emulation, though it does
   not represent the readout arrangement.

   For each tower, the TriggerTower contains the final ET (LUT output)
   as well as a vector of ADC data (at 25ns intervals) from which it was
   formed, plus error flags and raw BCID output. As the PreProcessor
   readout can read variable numbers of slices of all of these data, the
   internal storage is in std::vectors, and methods exist to return all
   slices as well as just the data corresponding to the triggered crossing.
   */
class TriggerTower {

public:

/** Constructors */
    TriggerTower();  
    
    TriggerTower( double phi, double eta, unsigned int key);
    
    TriggerTower( double phi, double eta, unsigned int key,
                  const std::vector<int>& em_digits,
                  const std::vector<int>& em_et,
		  const std::vector<int>& em_digits_bcid,
                  const std::vector<int>& em_bcid, 
		  int em_error, int em_peak, int em_adc_peak,
                  const std::vector<int>& had_digits,
                  const std::vector<int>& had_et,
		  const std::vector<int>& had_digits_bcid,
                  const std::vector<int>& had_bcid, 
		  int had_error, int had_peak, int had_adc_peak);
/** Destructor */
    virtual ~TriggerTower();

/** Methods to fill towers with digits, calibrated ET vectors and status flags*/
    void addEM(const std::vector<int>& digits, const std::vector<int>& et, 
               const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
	       int error, int peak, int adc_peak);
    void addHad(const std::vector<int>& digits, const std::vector<int>& et, 
                const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
		int error, int peak, int adc_peak);
/** Methods to specify location of tower */
    void setCoord(double eta, double phi);
    void setKey(unsigned int key);
    
/** return the final energy values using the in algorithms */
    int emEnergy() const;
    int hadEnergy() const;

/** return references to the vectors containing the final calubrated ET values.
    In simulation there will only be one timeslice, but data readout may 
    contain more */
    const std::vector<int> & emLUT() const;
    const std::vector<int> & hadLUT() const;

/** return references to the vectors containing the ADC digits */
    const std::vector<int> & emADC() const;
    const std::vector<int> & hadADC() const;
      
/** returns eta coord */
    double eta() const {return m_eta;};
    
/** returns phi coord of tower */
    double phi() const {return m_phi;};
  
/** returns coordinate of TT */
    Coordinate coord() const;
    
/** returns TT key - that is the key the TT had
    when it was created. Added for bug-testing. Decide whether still needed?*/
    unsigned int key() const { return m_key;};
           
/** return peak positions in LUT and ADC vectors */
    int emPeak() const {return m_em_peak;};    
    int emADCPeak() const {return m_em_adc_peak;};    
    int hadPeak() const {return m_had_peak;};    
    int hadADCPeak() const {return m_had_adc_peak;};  
     
/** Utilities to report on status of tower */    
    bool isEMSaturated() const;
    bool isHadSaturated() const;
    int emError() const {return m_em_error;};
    int hadError() const {return m_had_error;};
    int emBCID() const;
    int hadBCID() const;
    const std::vector<int> & emBCIDvec() const;
    const std::vector<int> & hadBCIDvec() const;
    const std::vector<int> & emBCIDext() const;
    const std::vector<int> & hadBCIDext() const;
  
/** Internal data */  
    private:
    std::vector <int> m_em_energy;
    std::vector <int> m_had_energy;
    std::vector <int> m_em_adc;
    std::vector <int> m_had_adc;   
    std::vector <int> m_em_BCID;
    std::vector <int> m_had_BCID;
    std::vector <int> m_em_extBCID;
    std::vector <int> m_had_extBCID;
    double m_phi;
    double m_eta;
    unsigned int m_key;
    int m_em_error;
    int m_had_error;
    int m_em_peak;
    int m_em_adc_peak;
    int m_had_peak;
    int m_had_adc_peak;
    
    static const int m_saturation = 255;
  
    };
  } // end of namespace

#ifndef TriggerTower_ClassDEF_H
#include "TrigT1CaloEvent/TriggerTower_ClassDEF.h"
#endif

#endif
