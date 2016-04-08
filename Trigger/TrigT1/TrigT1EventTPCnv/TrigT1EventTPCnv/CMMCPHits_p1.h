
 //***************************************************************************
 //                           CMMCPHits_p1.h  -  description
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
