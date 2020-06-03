/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CPMTobRoI_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
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
