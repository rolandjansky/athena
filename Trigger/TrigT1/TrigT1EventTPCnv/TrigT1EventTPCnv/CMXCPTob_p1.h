
 //***************************************************************************
 //                           CMXCPTob_p1.h  -  description
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
  
#ifndef CMXCPTob_p1_H
#define CMXCPTob_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXCPTob class */

class CMXCPTob_p1  {

    friend class CMXCPTobCnv_p1;
        
public:

    /** Constructors */
    CMXCPTob_p1();

    /** Destructor */
    //virtual ~CMXCPTob_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_cmx;
    int m_cpm;
    int m_chip;
    int m_location;
    int m_peak;
    std::vector<int> m_energy;
    std::vector<int> m_isolation;
    std::vector<int> m_error;
    std::vector<unsigned int> m_presenceMap;
    
};

inline CMXCPTob_p1::CMXCPTob_p1()
  :  m_crate(0),
     m_cmx(0),
     m_cpm(0),
     m_chip(0),
     m_location(0),
     m_peak(0),
     m_energy(0),
     m_isolation(0),
     m_error(0),
     m_presenceMap(0)
{
}

#endif
