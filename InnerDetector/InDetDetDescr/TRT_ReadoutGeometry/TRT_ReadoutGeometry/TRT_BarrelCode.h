/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_BarrelCode.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRT_BarrelCode_h
#define TRT_BarrelCode_h 1

#include <iostream>

// Include the string class header
#include <string>

namespace InDetDD {

  /** @class TRT_BarrelCode 
  
        bit definitions to decode TRT straws in barrel
  */

  class TRT_BarrelCode 
    {
    public:
      
      typedef enum { MODULE_A   = 0, 
		     MODULE_B   = 1,
		     MODULE_C   = 2 }       Module;
      
      TRT_BarrelCode();
      TRT_BarrelCode( unsigned int  isPositive, unsigned int moduleIndex, unsigned phiIndex, unsigned int strawLayerIndex); 
      
      TRT_BarrelCode( const TRT_BarrelCode & right );
      ~TRT_BarrelCode();
      //(operator= ) was returning const ref; changed on 10/12/2014 sroe
      TRT_BarrelCode & operator=( const TRT_BarrelCode & right );
      
      bool operator < (const TRT_BarrelCode & right) const;
      bool operator > (const TRT_BarrelCode & right) const;
      bool operator== (const TRT_BarrelCode & right) const;
      bool operator!= (const TRT_BarrelCode & right) const;
      
      unsigned int isPosZ()              const;
      unsigned int getModuleIndex()      const;
      unsigned int getPhiIndex()         const;
      unsigned int getStrawLayerIndex()  const;
      unsigned int getKey()              const;
      std::string getName()              const;
      
    private:  
      
      // Define key as a bit string:
      //   bit 0    Z (negative=0 or positive=1)
      //   bit 1-2  Module Index (0-2)
      //   bit 3-7  Phi Index
      //   bit 8-12 StrawLayer Index
      
      unsigned short int m_key;
      
      static const int Z_MASK;
      static const int MODULE_MASK;
      static const int PHI_MASK;
      static const int STRAWLAYER_MASK;
      
      
      static const int Z_SHIFT;
      static const int MODULE_SHIFT;
      static const int PHI_SHIFT;
      static const int STRAWLAYER_SHIFT;
      
  };
 
  
  
  inline unsigned int TRT_BarrelCode::isPosZ() const {
    unsigned int ret = m_key;
    return (ret >> Z_SHIFT) & Z_MASK;
  }
  
  inline unsigned int TRT_BarrelCode::getModuleIndex() const{
    unsigned int ret = m_key;
    return (ret >> MODULE_SHIFT) & MODULE_MASK;
  }
  
  inline unsigned int TRT_BarrelCode::getPhiIndex() const{
    unsigned int ret = m_key;
    return (ret >> PHI_SHIFT) & PHI_MASK;
  }
  
  inline unsigned int TRT_BarrelCode::getStrawLayerIndex() const {
    unsigned int ret = m_key;
    return (ret >> STRAWLAYER_SHIFT) & STRAWLAYER_MASK;
  }
  
  inline unsigned int TRT_BarrelCode::getKey() const {
    return m_key;
  }
  
  
  inline bool TRT_BarrelCode::operator==(const TRT_BarrelCode &right) const
    {
      return (getKey() == right.getKey());
    }
  
  inline bool TRT_BarrelCode::operator!=(const TRT_BarrelCode &right) const
    {
      return (getKey() != right.getKey());
    }
  
  inline bool TRT_BarrelCode::operator<(const TRT_BarrelCode &right) const
    {
      return (getKey() < right.getKey());
    }
  
  inline bool TRT_BarrelCode::operator>(const TRT_BarrelCode &right) const
    {
      return (getKey() > right.getKey());
    }
} // End InDetDD namespace

  
inline std::ostream & operator << (std::ostream & os, const InDetDD::TRT_BarrelCode & code) {
  // Plus/minus 
  if (code.isPosZ()) os << '+';
  else os << '-';
  // Module
  unsigned int mod = code.getModuleIndex();
  if (mod==0) os << 'A';
  else if (mod==1) os << 'B';
  else if (mod==2) os << 'C';
  else os << "?";
  // Phi Index:
  os << code.getPhiIndex();
  // Separator:
  os << '/';
  // StrawLayer:  
  os << code.getStrawLayerIndex();
  return os;
}

#endif
