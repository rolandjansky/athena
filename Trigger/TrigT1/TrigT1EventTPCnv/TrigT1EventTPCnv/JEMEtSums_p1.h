
 //***************************************************************************
 //                           JEMEtSums_p1.h  -  description
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
