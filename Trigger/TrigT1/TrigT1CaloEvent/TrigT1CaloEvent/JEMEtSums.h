
 //***************************************************************************
 //                           JEMEtSums.h  -  description
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
  
  #ifndef JEMEtSums_H
  #define JEMEtSums_H
  
  #include "AthenaKernel/CLASS_DEF.h"


  #include <iostream>
  #ifndef  TRIGGERSPACE
  #include "TrigT1Interfaces/Coordinate.h"
  #else
  #include "Coordinate.h"
  #endif 
  namespace LVL1 {
  
    class JEMEtSums  {
    public:
      
    JEMEtSums();
    JEMEtSums(int crate, int module);
    JEMEtSums(int crate, int module, 
              const std::vector<unsigned int>&  Et, 
	      const std::vector<unsigned int>&  Ex, 
	      const std::vector<unsigned int>&  Ey,
	      int peak);
  
    virtual ~JEMEtSums();

    void setPeak(int peak);
    void addEt(const std::vector<unsigned int>&  Et);
    void addEx(const std::vector<unsigned int>&  Ex);
    void addEy(const std::vector<unsigned int>&  Ey);
    
    int crate() const;
    int module() const;
    unsigned int Et() const;
    unsigned int Ex() const;
    unsigned int Ey() const;
    const std::vector<unsigned int>& EtVec() const;
    const std::vector<unsigned int>& ExVec() const;
    const std::vector<unsigned int>& EyVec() const;
    int peak() const;
    bool forward() const;
      
  /** Internal data */
    private:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    
    };
  } // end of namespace

#ifndef JEMEtSums_ClassDEF_H
#include "TrigT1CaloEvent/JEMEtSums_ClassDEF.h"
#endif

#endif
