/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
  
  
#ifndef JEMTobRoI_p1_H
#define JEMTobRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::JEMTobRoI class */

class JEMTobRoI_p1  {

    friend class JEMTobRoICnv_p1;
        
public:

    /** Constructors */
    JEMTobRoI_p1();

    /** Destructor */
    //virtual ~JEMTobRoI_p1();

  /** Internal data */
  //  protected:
    uint32_t m_roiWord;
    
};

inline JEMTobRoI_p1::JEMTobRoI_p1()
  :  m_roiWord(0)
{
}

#endif
