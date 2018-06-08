/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMTower_p1.h  -  description
 //                              -------------------
 //    begin                : 29/11/2007
 //    email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CPMTower_P1_H
#define CPMTower_P1_H

#include <vector>

// Forward declaration of the convertor
//class CPMTowerCnv_p1;
//namespace LVL1 {
   
   //Doxygen class description below:
   /**
   Persistent form of CPMTower using T/P separation
   */
class CPMTower_p1 {

  friend class CPMTowerCnv_p1;

public:

    CPMTower_p1();
    virtual ~CPMTower_p1(){}
      
/** Internal data */

//protected:

    std::vector<int> m_em_energy;
    std::vector<int> m_had_energy;
    std::vector<int> m_em_error;
    std::vector<int> m_had_error;
    double m_phi;
    double m_eta;
    int m_peak;
};


inline CPMTower_p1::CPMTower_p1()
  :     m_em_energy(0),
        m_had_energy(0),
        m_em_error(0),
        m_had_error(0),
        m_phi(0),
        m_eta(0),
        m_peak(0)
 {
 }



//} // end of namespace

#endif
