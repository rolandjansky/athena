
 //***************************************************************************
 //                           CPMTobRoI_p1.h  -  description
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
  
#ifndef CPMTobRoI_p1_H
#define CPMTobRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::CPMTobRoI class */

class CPMTobRoI_p1  {

    friend class CPMTobRoICnv_p1;
        
public:

    /** Constructors */
    CPMTobRoI_p1();

    /** Destructor */
    //virtual ~CPMTobRoI_p1();

  /** Internal data */
  //  protected:
    uint32_t m_roiWord;
    
};

inline CPMTobRoI_p1::CPMTobRoI_p1()
  :  m_roiWord(0)
{
}

#endif
