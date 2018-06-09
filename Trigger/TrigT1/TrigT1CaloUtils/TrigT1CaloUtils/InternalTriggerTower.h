/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                     InternalTriggerTower.h  -  description
 //                              -------------------
 //     begin                : Fri Sep 30 2005
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef InternalTriggerTower_H
#define InternalTriggerTower_H

#include "DataModel/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"
  
#include "TrigT1Interfaces/Coordinate.h"

  
namespace LVL1 {
    
   //Doxygen class description below:
   /** 
   A version of the TriggerTower that contains information that would not be availiable
   during datataking. Only use is in TriggerTowerMaker, to allow building of TriggerTowers
   from different types of input data.
   External users should use TriggerTower to access tower information.
   */
class InternalTriggerTower {

public:
    InternalTriggerTower( double phi, double eta, int key);
  
    InternalTriggerTower();  

    virtual ~InternalTriggerTower();
      
/** returns eta coord */
    double eta() const {return m_eta;};
    
/** returns phi coord of tower */
    double phi() const {return m_phi;};
  
/** returns coordinate of tower */
    Coordinate coord() const;

/** returns reference to the vector containing the em amplitudes */
    const std::vector<double> & EmAmps() const;
    
/** returns reference to the vector containing the had amplitudes */
    const std::vector<double> & HadAmps() const;

/** returns reference to the vector containing the em digits */
    const std::vector<int> & EmADC() const;
    
/** returns reference to the vector containing the had digits */
    const std::vector<int> & HadADC() const;

/** Methods to fill towers with analogue amplitudes */
    void addEMAmps(std::vector<double>& energy);
    void addHadAmps(std::vector<double>& energy);

/** Methods to add energy to peak sample of tower (used for CaloCell input) */
    void addEMPeak(double energy);
    void addHadPeak(double energy);

/** Methods to fill towers with digitised amplitudes */
    void addEMADC(std::vector<int>& adc);
    void addHadADC(std::vector<int>& adc);
        
/** returns TT key - that is the key the TT had
when it was created. Added for bug-testing.*/
    unsigned int key() const { return m_key;};    
  
/** Internal data */  
    private:
    double m_phi;
    double m_eta;
    unsigned int m_key;
    std::vector <double> m_EmAmps;
    std::vector <double> m_HadAmps;   
    std::vector <int> m_EmADC;
    std::vector <int> m_HadADC;
    
    };
  
  } // end of namespace

#ifndef InternalTriggerTower_ClassDEF_H
#include "TrigT1CaloUtils/InternalTriggerTower_ClassDEF.h"
#endif

#endif
