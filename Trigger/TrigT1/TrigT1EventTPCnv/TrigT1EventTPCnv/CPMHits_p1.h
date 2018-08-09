/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CPMHits_p1_H
#define CPMHits_p1_H
  
  
/** T/P separation for TrigT1Calo::CPMHits class */

class CPMHits_p1  {

    friend class CPMHitsCnv_p1;
        
public:

    /** Constructors */
    CPMHits_p1();

    /** Destructor */
    //virtual ~CPMHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_Hits0;
    std::vector <unsigned int> m_Hits1;
    
};

inline CPMHits_p1::CPMHits_p1()
  :  m_crate(0),
     m_module(0),
     m_peak(0),
     m_Hits0(0),
     m_Hits1(0)
{
}

#endif
