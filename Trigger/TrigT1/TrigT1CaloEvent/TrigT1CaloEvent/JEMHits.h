
 //***************************************************************************
 //                           JEMHits.h  -  description
 //                              -------------------
 //     begin                : 25 05 2006
 //     copyright            : (C) 2006 by Alan Watson
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
  #ifndef JEMHits_H
  #define JEMHits_H
  
  #include "AthenaKernel/CLASS_DEF.h"


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
