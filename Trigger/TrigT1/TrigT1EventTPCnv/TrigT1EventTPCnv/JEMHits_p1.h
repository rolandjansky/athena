/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef JEMHits_p1_H
#define JEMHits_p1_H
  
  
/** T/P separation for TrigT1Calo::JEMHits class */

class JEMHits_p1  {

    friend class JEMHitsCnv_p1;
        
public:

    /** Constructors */
    JEMHits_p1();

    /** Destructor */
    //virtual ~JEMHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_module;
    int m_peak;
    std::vector <unsigned int> m_JetHits;
    
};

inline JEMHits_p1::JEMHits_p1()
  :  m_crate(0),
     m_module(0),
     m_peak(0),
     m_JetHits(0)
{
}

#endif
