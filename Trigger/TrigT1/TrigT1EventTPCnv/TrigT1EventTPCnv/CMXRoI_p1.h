/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
  
  
#ifndef CMXRoI_p1_H
#define CMXRoI_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXRoI class */

class CMXRoI_p1  {

    friend class CMXRoICnv_p1;
        
public:

    /** Constructors */
    CMXRoI_p1();

    /** Destructor */
    //virtual ~CMXRoI_p1();

  /** Internal data */
  //  protected:
   std::vector<uint32_t> m_roiWords;
    
};

inline CMXRoI_p1::CMXRoI_p1()
  :  m_roiWords(0)
{
}

#endif
