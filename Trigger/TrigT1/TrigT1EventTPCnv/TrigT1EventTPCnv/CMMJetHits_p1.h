
 //***************************************************************************
 //                           CMMJetHits_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     copyright            : (C) 2008 by Alan Watson
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
