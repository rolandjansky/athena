/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCELLTIMEOFFSET_H
#define LARCELLTIMEOFFSET_H

#include "AthenaKernel/CLASS_DEF.h" 
#include "Identifier/HWIdentifier.h"
#include <map>

class LArCellTimeOffset {
 public:
  LArCellTimeOffset();                                              //Constructor
  virtual ~LArCellTimeOffset() {};                                  //Destructor 
  inline void setTimeOffset(const HWIdentifier fId, const float offset) //Setter
    {m_cellTimeMap[fId]=offset;}    
  inline float TimeOffset(const HWIdentifier fId) const;                  //Getter

  inline unsigned size() const
    { return m_cellTimeMap.size();}
  
  inline void setDefaultReturnValue(const float value)
    { m_defaultReturnValue=value; }
  
  inline std::map <HWIdentifier,float>::const_iterator begin() const
    { return m_cellTimeMap.begin();}

  inline std::map <HWIdentifier,float>::const_iterator end() const
    { return m_cellTimeMap.end();}

 private:
  std::map <HWIdentifier,float> m_cellTimeMap;
  float m_defaultReturnValue;
};

inline float LArCellTimeOffset::TimeOffset(const HWIdentifier fId) const {
  const std::map <HWIdentifier,float>::const_iterator it=m_cellTimeMap.find(fId);
  if (it==m_cellTimeMap.end())
    return m_defaultReturnValue;
  else
    return it->second;
}

CLASS_DEF( LArCellTimeOffset, 141392342, 1) 
#endif 
