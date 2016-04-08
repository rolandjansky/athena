
 //***************************************************************************
 //                           CMMRoI_p1.h  -  description
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
  
#ifndef CMMRoI_p1_H
#define CMMRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::CMMRoI class */

class CMMRoI_p1  {

    friend class CMMRoICnv_p1;
        
public:

    /** Constructors */
    CMMRoI_p1();

    /** Destructor */
    //virtual ~CMMRoI_p1();

  /** Internal data */
  //  protected:
   uint32_t m_jetEtRoiWord;
   uint32_t m_energyRoiWord0;
   uint32_t m_energyRoiWord1;
   uint32_t m_energyRoiWord2;
    
};

inline CMMRoI_p1::CMMRoI_p1()
  :  m_jetEtRoiWord(0),
     m_energyRoiWord0(0),
     m_energyRoiWord1(0),
     m_energyRoiWord2(0)
{
}

#endif
