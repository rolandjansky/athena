/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMHits.h  -  description
 //                              -------------------
 //     begin                : 25 05 2006
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
  #ifndef JEMHits_H
  #define JEMHits_H
  
  #include "CLIDSvc/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {
  
    class JEMHits  {
    public:
      
    JEMHits();
    JEMHits(int crate, int module);
    JEMHits(int crate, int module, const std::vector<unsigned int>&  JetHits, int peak);
  
    virtual ~JEMHits();

    void setPeak(int peak);
    void addJetHits(const std::vector<unsigned int>&  hits);
    
    int crate() const;
    int module() const;
    unsigned int JetHits() const;
    const std::vector<unsigned int>& JetHitsVec() const;
    int peak() const;
    bool forward() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_JetHits;
    
    };
  } // end of namespace

#ifndef JEMHits_ClassDEF_H
#include "TrigT1CaloEvent/JEMHits_ClassDEF.h"
#endif

#endif
