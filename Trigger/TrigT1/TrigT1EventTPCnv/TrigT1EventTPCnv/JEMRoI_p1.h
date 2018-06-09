/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           JEMRoI_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
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
