/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGIT_H
#define LUCID_DIGIT_H

class LUCID_Digit {
  
 public:
  
  LUCID_Digit();
  LUCID_Digit(const unsigned short tubeID, 
	      const double         npe, 
	      const unsigned short npeGas, 
	      const unsigned short npePmt, 
	      const unsigned short qdc, 
	      const unsigned short tdc,
	      const bool           isHit);
  
  inline unsigned short getTubeID() const { return m_tubeID; }
  inline double         getNpe   () const { return m_npe;    }
  inline unsigned short getNpeGas() const { return m_npeGas; }
  inline unsigned short getNpePmt() const { return m_npePmt; }
  inline unsigned short getQDC   () const { return m_qdc;    }
  inline unsigned short getTDC   () const { return m_tdc;    }
  inline bool           isHit    () const { return m_isHit;  }
  
  inline bool isSideA() { return (m_tubeID <  20); }
  inline bool isSideC() { return (m_tubeID >= 20); }
  
 private: 
  
  unsigned short m_tubeID;
  double         m_npe;
  unsigned short m_npeGas;
  unsigned short m_npePmt;
  unsigned short m_qdc;
  unsigned short m_tdc;
  bool           m_isHit;
};

#endif
