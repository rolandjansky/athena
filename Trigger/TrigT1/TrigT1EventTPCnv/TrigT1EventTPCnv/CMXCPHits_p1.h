/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMXCPHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMXCPHits_p1_H
#define CMXCPHits_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXCPHits class */

class CMXCPHits_p1  {

    friend class CMXCPHitsCnv_p1;
        
public:

    /** Constructors */
    CMXCPHits_p1();

    /** Destructor */
    //virtual ~CMXCPHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_cmx;
    int m_source;
    int m_peak;
    std::vector <unsigned int> m_hits0;
    std::vector <unsigned int> m_hits1;
    std::vector <int> m_error0;
    std::vector <int> m_error1;
    
};

inline CMXCPHits_p1::CMXCPHits_p1()
  :  m_crate(0),
     m_cmx(0),
     m_source(0),
     m_peak(0),
     m_hits0(0),
     m_hits1(0),
     m_error0(0),
     m_error1(0)
{
}

#endif
