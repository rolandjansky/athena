/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMMCPHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMMCPHits_p1_H
#define CMMCPHits_p1_H
  
  
/** T/P separation for TrigT1Calo::CMMCPHits class */

class CMMCPHits_p1  {

    friend class CMMCPHitsCnv_p1;
        
public:

    /** Constructors */
    CMMCPHits_p1();

    /** Destructor */
    //virtual ~CMMCPHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector <unsigned int> m_hits0;
    std::vector <unsigned int> m_hits1;
    std::vector <int> m_error0;
    std::vector <int> m_error1;
    
};

inline CMMCPHits_p1::CMMCPHits_p1()
  :  m_crate(0),
     m_dataID(0),
     m_peak(0),
     m_hits0(0),
     m_hits1(0),
     m_error0(0),
     m_error1(0)
{
}

#endif
