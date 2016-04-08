
 //***************************************************************************
 //                           JEMHits_p1.h  -  description
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
