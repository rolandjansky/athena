/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMRoI_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
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
