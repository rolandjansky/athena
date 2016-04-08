  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXJetHits_p1_H
#define CMXJetHits_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXJetHits class */

class CMXJetHits_p1  {

    friend class CMXJetHitsCnv_p1;
        
public:

    /** Constructors */
    CMXJetHits_p1();

    /** Destructor */
    //virtual ~CMXJetHits_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_source;
    int m_peak;
    std::vector <unsigned int> m_hits0;
    std::vector <unsigned int> m_hits1;
    std::vector <int> m_error0;
    std::vector <int> m_error1;
    
};

inline CMXJetHits_p1::CMXJetHits_p1()
  :  m_crate(0),
     m_source(0),
     m_peak(0),
     m_hits0(0),
     m_hits1(0),
     m_error0(0),
     m_error1(0)
{
}

#endif
