
 //***************************************************************************
 //                           JEMRoI_p1.h  -  description
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
  
#ifndef JEMRoI_p1_H
#define JEMRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::JEMRoI class */

class JEMRoI_p1  {

    friend class JEMRoICnv_p1;
        
public:

    /** Constructors */
    JEMRoI_p1();

    /** Destructor */
    //virtual ~JEMRoI_p1();

  /** Internal data */
  //  protected:
    uint32_t m_roiWord;
    
};

inline JEMRoI_p1::JEMRoI_p1()
  :  m_roiWord(0)
{
}

#endif
