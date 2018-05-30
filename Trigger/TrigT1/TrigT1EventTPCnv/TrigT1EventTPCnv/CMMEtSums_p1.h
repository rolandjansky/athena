/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

 //***************************************************************************
 //                           CMMEtSums_p1.h  -  description
 //                              -------------------
 //     begin                : 21 04 2008
 //     email                : Alan.Watson@cern.ch
 //  ***************************************************************************/
  
  
#ifndef CMMEtSums_p1_H
#define CMMEtSums_p1_H
  
  
/** T/P separation for TrigT1Calo::CMMEtSums class */

class CMMEtSums_p1  {

    friend class CMMEtSumsCnv_p1;
        
public:

    /** Constructors */
    CMMEtSums_p1();

    /** Destructor */
    //virtual ~CMMEtSums_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_dataID;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    std::vector <int> m_EtError;
    std::vector <int> m_ExError;
    std::vector <int> m_EyError;
    
};

inline CMMEtSums_p1::CMMEtSums_p1()
  :  m_crate(0),
     m_dataID(0),
     m_peak(0),
     m_Et(0),
     m_Ex(0),
     m_Ey(0),
     m_EtError(0),
     m_ExError(0),
     m_EyError(0)
{
}

#endif
