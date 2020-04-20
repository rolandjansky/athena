/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiDetectorElementCollection.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_EndcapCode_h
#define TRT_EndcapCode_h 1

#include <iostream>

// Include the string class header
#include <string>

namespace InDetDD {

  /** @class TRT_EndcapCode 
  
        bit definitions to decode TRT straws in endcap
  */

  class TRT_EndcapCode 
    {
    public:
      
      
      TRT_EndcapCode();
      TRT_EndcapCode( unsigned int  isPositive, 
		      unsigned int wheelIndex, 
		      unsigned int strawLayerIndex, 
		      unsigned int phiIndex); 
      
      TRT_EndcapCode( const TRT_EndcapCode & right );
      ~TRT_EndcapCode();
      //was returning a const ref; changed 10/12/2014, sroe
      TRT_EndcapCode & operator=( const TRT_EndcapCode & right );
      
      bool operator < (const TRT_EndcapCode & right) const;
      bool operator > (const TRT_EndcapCode & right) const;
      bool operator== (const TRT_EndcapCode & right) const;
      bool operator!= (const TRT_EndcapCode & right) const;
      
      unsigned int isPosZ()              const;
      unsigned int getWheelIndex()       const;
      unsigned int getStrawLayerIndex()  const;
      unsigned int getPhiIndex()         const;
      unsigned int getKey()              const;
      std::string getName()              const;
      
    private:  
      
      
      // Define key as a bit string:
      //   bit 0    Z (negative=0 or positive=1)
      //   bit 1-2  Wheel Index (0-2)
      //   bit 3-7  StrawLayer Index
      //   bit 8-12 Phi position (0-31)      
      
      unsigned short int m_key;
      
      static const int Z_MASK;
      static const int WHEEL_MASK;
      static const int STRAWLAYER_MASK;
      static const int PHIPOS_MASK;
      
      static const int Z_SHIFT;
      static const int WHEEL_SHIFT;
      static const int STRAWLAYER_SHIFT;
      static const int PHIPOS_SHIFT;      

    };
  
  
  
  inline unsigned int TRT_EndcapCode::isPosZ() const {
    unsigned int ret = m_key;
    return (ret >> Z_SHIFT) & Z_MASK;
  }
  
  inline unsigned int TRT_EndcapCode::getWheelIndex() const{
    unsigned int ret = m_key;
    return (ret >> WHEEL_SHIFT) & WHEEL_MASK;
  }
  
  
  inline unsigned int TRT_EndcapCode::getStrawLayerIndex() const {
    unsigned int ret = m_key;
    return (ret >> STRAWLAYER_SHIFT) & STRAWLAYER_MASK;
  }

  inline unsigned int TRT_EndcapCode::getPhiIndex() const {
    unsigned int ret = m_key;
    return (ret >> PHIPOS_SHIFT) & PHIPOS_MASK;
  }
  
  inline unsigned int TRT_EndcapCode::getKey() const {
    return m_key;
  }
  
  
  inline bool TRT_EndcapCode::operator==(const TRT_EndcapCode &right) const
    {
      return (getKey() == right.getKey());
    }
  
  inline bool TRT_EndcapCode::operator!=(const TRT_EndcapCode &right) const
    {
      return (getKey() != right.getKey());
    }
  
  inline bool TRT_EndcapCode::operator<(const TRT_EndcapCode &right) const
    {
      return (getKey() < right.getKey());
    }
  
  inline bool TRT_EndcapCode::operator>(const TRT_EndcapCode &right) const
    {
      return (getKey() > right.getKey());
    }
} // End InDetDD namespace

  
inline std::ostream & operator << (std::ostream & os, const InDetDD::TRT_EndcapCode & code) {
  // Plus/minus 
  if (code.isPosZ()) os << '+';
  else os << '-';
  // Wheel
  unsigned int wheel = code.getWheelIndex();
  os << wheel << "/";
  // StrawLayer:  
  os << code.getStrawLayerIndex()<< "/";
  // Phi
  os << code.getPhiIndex();
  return os;
}

#endif
