  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXEtSums_p1_H
#define CMXEtSums_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXEtSums class */

class CMXEtSums_p1  {

    friend class CMXEtSumsCnv_p1;
        
public:

    /** Constructors */
    CMXEtSums_p1();

    /** Destructor */
    //virtual ~CMXEtSums_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_source;
    int m_peak;
    std::vector <unsigned int> m_Et;
    std::vector <unsigned int> m_Ex;
    std::vector <unsigned int> m_Ey;
    std::vector <int> m_EtError;
    std::vector <int> m_ExError;
    std::vector <int> m_EyError;
    
};

inline CMXEtSums_p1::CMXEtSums_p1()
  :  m_crate(0),
     m_source(0),
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
