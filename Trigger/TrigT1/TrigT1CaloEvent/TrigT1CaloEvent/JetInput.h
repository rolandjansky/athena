/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JetInput.h  -  description
 //                              -------------------
 //     begin                : Tue 17 Jan 2006
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef JetInput_H
  #define JetInput_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif

  namespace LVL1 {
     //Doxygen class description below:
   /**
  A minimal version of JetElement, containing only eta, phi, ET and a JetInputKey,
  ie the bare information needed to execute the Jet trigger algorithm.
  JetInputs are only used as local objects within JetTrigger, i.e. not placed in
  StoreGate.
   */
    class JetInput  {
    public:
      
    JetInput();
    JetInput(double phi, double eta, int energy, int key);
  
    virtual ~JetInput();

    double eta() const;
    double phi() const;
    int energy() const;    
    unsigned int key();
    bool isSaturated() const;
 
  /** return coord of JE */
  Coordinate coord() const;
  
  /** Internal data */
    private:
    int m_energy;
    double m_phi;
    double m_eta;
    unsigned int m_key;
    
    static const int m_saturationThreshold = 1023;
  
    };
  } // end of namespace

#ifndef JetInput_ClassDEF_H
#include "TrigT1CaloEvent/JetInput_ClassDEF.h"
#endif

#endif
