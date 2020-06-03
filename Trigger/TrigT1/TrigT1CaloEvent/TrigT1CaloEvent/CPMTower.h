/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMTower.h  -  description
 //                              -------------------
 //     begin                : Thurs Dec 14 2006
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CPMTower_H
#define CPMTower_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

#include "TrigT1Interfaces/Coordinate.h"

  
namespace LVL1 {
   
   //Doxygen class description below:
/** The CPMTower class contains the TriggerTower information received by
    the Cluster Processor Modules. This is a subset of the information
    in the TriggerTower class, which represents the data produced by
    the PreProcessor. In particular, it only contains the final (LUT)
    ET, and not the ADC data, and only for towers used by the Cluster
    Processor.

    The main purpose of the CPMTower class is to allow unpacking and
    analysis of the tower data from the CPM readout. */
    
class CPMTower {

public:

/** Constructors */
    CPMTower();  
    
    CPMTower(double phi, double eta);
    
    CPMTower(double phi, double eta,
             const std::vector<int>& em_et,
             const std::vector<int>& em_error,
             const std::vector<int>& had_et,
             const std::vector<int>& had_error,
             int peak);
    
/** Destructor */
    virtual ~CPMTower();

/** Methods to fill towers with digits, calibrated ET vectors and status flags*/
    void fill(const std::vector<int>& em_et,
              const std::vector<int>& em_error,
	      const std::vector<int>& had_et,
              const std::vector<int>& had_error,
              int peak);
    
/** return the final ET values using the in algorithms */
    int emEnergy() const {return m_em_energy[m_peak];};
    int hadEnergy() const {return m_had_energy[m_peak];};

/** return the ET values for a specified slice */
    int emSliceEnergy(int slice) const;
    int hadSliceEnergy(int slice) const;

/** return references to the vectors containing the ET values. */
    const std::vector<int> & emEnergyVec() const {return m_em_energy;};
    const std::vector<int> & hadEnergyVec() const {return m_had_energy;};
      
/** returns eta coord */
    double eta() const {return m_eta;};
    
/** returns phi coord of tower */
    double phi() const {return m_phi;};
  
/** returns coordinate of TT */
    Coordinate coord() const;
           
/** return peak positions in ET and error vectors */
    int peak() const {return m_peak;};    
     
/** Utilities to report on status of tower */    
    int emError() const {return m_em_error[m_peak];};
    int hadError() const {return m_had_error[m_peak];};
    int emSliceError(int slice) const;
    int hadSliceError(int slice) const;
    const std::vector<int> & emErrorVec() const {return m_em_error;};
    const std::vector<int> & hadErrorVec() const {return m_had_error;};
  
/** Internal data */  
    private:
    std::vector <int> m_em_energy;
    std::vector <int> m_had_energy;
    std::vector <int> m_em_error;
    std::vector <int> m_had_error;
    double m_phi;
    double m_eta;
    int m_peak;
  
    };
  } // end of namespace

#ifndef CPMTower_ClassDEF_H
#include "TrigT1CaloEvent/CPMTower_ClassDEF.h"
#endif

#endif
