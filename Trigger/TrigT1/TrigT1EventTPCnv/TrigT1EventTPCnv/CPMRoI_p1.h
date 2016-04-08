
 //***************************************************************************
 //                           CPMRoI_p1.h  -  description
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
  
#ifndef CPMRoI_p1_H
#define CPMRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::CPMRoI class */

class CPMRoI_p1  {

    friend class CPMRoICnv_p1;
        
public:

    /** Constructors */
    CPMRoI_p1();

    /** Destructor */
    //virtual ~CPMRoI_p1();

  /** Internal data */
  //  protected:
    uint32_t m_roiWord;
    
};

inline CPMRoI_p1::CPMRoI_p1()
  :  m_roiWord(0)
{
}

#endif
