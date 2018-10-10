/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMEtSums_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef JEMEtSums_p1_H
#define JEMEtSums_p1_H
  
  
/** T/P separation for TrigT1Calo::JEMEtSums class */

class JEMEtSums_p1  {

    friend class JEMEtSumsCnv_p1;
        
public:

    /** Constructors */
    JEMEtSums_p1();

    /** Destructor */
    //virtual ~JEMEtSums_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    
};

inline JEMEtSums_p1::JEMEtSums_p1()
  :  m_crate(0),
     m_module(0),
     m_peak(0),
     m_Et(0),
     m_Ex(0),
     m_Ey(0)
{
}

#endif
