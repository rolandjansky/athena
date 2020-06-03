/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           TriggerTower_p1.h  -  description
 //                              -------------------
 //    begin                : 29/11/2007
 //    email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef TRIGGERTOWER_P1_H
#define TRIGGERTOWER_P1_H

#include <vector>

// Forward declaration of the convertor
//class TriggerTowerCnv_p1;  
//namespace LVL1 {
   
   //Doxygen class description below:
   /**
   Persistent form of TriggerTower using T/P separation
   */
class TriggerTower_p1 {

  friend class TriggerTowerCnv_p1;

public:

    TriggerTower_p1();
    virtual ~TriggerTower_p1(){}  
      
/** Internal data */

//protected:

    std::vector<int> m_em_energy;
    std::vector<int> m_had_energy;
    std::vector<int> m_em_adc;
    std::vector<int> m_had_adc;
    std::vector<int> m_em_BCID;
    std::vector<int> m_had_BCID;
    std::vector<int> m_em_extBCID;
    std::vector<int> m_had_extBCID;
    double m_phi;
    double m_eta;
    unsigned int m_key;  // can we drop this?
    int m_em_error;
    int m_had_error;
    int m_em_peak;
    int m_em_adc_peak;
    int m_had_peak;
    int m_had_adc_peak;
};


inline TriggerTower_p1::TriggerTower_p1()
  :     m_em_energy(0),
        m_had_energy(0),
        m_em_adc(0),
        m_had_adc(0),
        m_em_BCID(0),
        m_had_BCID(0),
        m_em_extBCID(0),
        m_had_extBCID(0),
        m_phi(0),
        m_eta(0),
        m_key(0),
        m_em_error(0),
        m_had_error(0),
        m_em_peak(0),
        m_em_adc_peak(0),
        m_had_peak(0),
        m_had_adc_peak(0)
 {
 }



//} // end of namespace

#endif
