/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JetElement_p1.h  -  description
 //                              -------------------
 //    begin                : 29/11/2007
 //    email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef JETELEMENT_P1_H
#define JETELEMENT_P1_H

#include <vector>

// Forward declaration of the convertor
//class JetElementCnv_p1;
//namespace LVL1 {
   
   //Doxygen class description below:
   /**
   Persistent form of JetElement using T/P separation
   */
class JetElement_p1 {

  friend class JetElementCnv_p1;

public:

    JetElement_p1();
    virtual ~JetElement_p1(){}
      
/** Internal data */

//protected:

    double m_phi;
    double m_eta;
    unsigned int m_key;
    int    m_peak;
    std::vector<int> m_emEnergy;
    std::vector<int> m_hadEnergy;
    std::vector<int> m_em_error;
    std::vector<int> m_had_error;
    std::vector<int> m_link_error;
    
};


inline JetElement_p1::JetElement_p1()
  :     m_phi(0),
        m_eta(0),
        m_key(0),
        m_peak(0),
        m_emEnergy(0),
        m_hadEnergy(0),
        m_em_error(0),
        m_had_error(0),
        m_link_error(0)
 {
 }



//} // end of namespace

#endif
