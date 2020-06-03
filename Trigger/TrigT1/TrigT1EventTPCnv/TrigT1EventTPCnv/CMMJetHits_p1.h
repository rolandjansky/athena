/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMMJetHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMMJetHits_p1_H
#define CMMJetHits_p1_H
  
  
/** T/P separation for TrigT1Calo::CMMJetHits class */

class CMMJetHits_p1  {

    friend class CMMJetHitsCnv_p1;
        
public:

    /** Constructors */
    CMMJetHits_p1();

    /** Destructor */
    //virtual ~CMMJetHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector <unsigned int> m_hits;
    std::vector <int> m_errors;
    
};

inline CMMJetHits_p1::CMMJetHits_p1()
  :  m_crate(0),
     m_dataID(0),
     m_peak(0),
     m_hits(0),
     m_errors(0)
{
}

#endif
