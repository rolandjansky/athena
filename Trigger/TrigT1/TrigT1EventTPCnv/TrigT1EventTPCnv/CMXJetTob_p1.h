  
 //***************************************************************************
 //  *                                                                         *
 //  *   This program is free software; you can redistribute it and/or modify  *
  // *   it under the terms of the GNU General Public License as published by  *
//   *   the Free Software Foundation; either version 2 of the License, or     *
 //  *   (at your option) any later version.                                   *
 //  *                                                                         *
 //  ***************************************************************************/
  
#ifndef CMXJetTob_p1_H
#define CMXJetTob_p1_H
  
  
/** T/P separation for TrigT1Calo::CMXJetTob class */

class CMXJetTob_p1  {

    friend class CMXJetTobCnv_p1;
        
public:

    /** Constructors */
    CMXJetTob_p1();

    /** Destructor */
    //virtual ~CMXJetTob_p1();

  /** Internal data */
  //  protected:
    int m_crate;
    int m_jem;
    int m_frame;
    int m_location;
    int m_peak;
    std::vector<int> m_energyLarge;
    std::vector<int> m_energySmall;
    std::vector<int> m_error;
    std::vector<unsigned int> m_presenceMap;
    
};

inline CMXJetTob_p1::CMXJetTob_p1()
  :  m_crate(0),
     m_jem(0),
     m_frame(0),
     m_location(0),
     m_peak(0),
     m_energyLarge(0),
     m_energySmall(0),
     m_error(0),
     m_presenceMap(0)
{
}

#endif
