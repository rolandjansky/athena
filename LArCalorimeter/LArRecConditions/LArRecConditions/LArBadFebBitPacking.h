/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadFebBitPacking_H
#define LArBadFebBitPacking_H

#include "LArRecConditions/LArBadFebEnum.h"
#include "LArRecConditions/LArBadFeb.h"
#include <string>
#include <vector>
#include <map>

class LArBadFeb;

class LArBadFebBitPacking : public LArBadFebEnum {
 public:

  typedef std::vector<PosType>        PosContainer;

  LArBadFebBitPacking();	

  const std::string& stringName( ProblemType pb) const;

  std::pair<bool, ProblemType> enumName( const std::string& str) const;

  PosType bitPosition( ProblemType pb) const {return LArBadFeb::bitPosition(pb);}

  PosType wordSize() const {return sizeof(BitWord);}

  void setBit( ProblemType pb, BitWord& word, bool value=true) const;
  void setBit( ProblemType pb, LArBadFeb& word, bool value=true) const;

  bool setBit( const std::string& name, BitWord& word, bool value=true) const;
  bool setBit( const std::string& name, LArBadFeb& word, bool value=true) const;

  std::string stringStatus( const LArBadFeb& bc) const;

  int numberOfProblemTypes() const { return m_nameVec.size();}

 private:

  std::vector<int>                    m_index;

  std::vector< ProblemType>           m_enumVec;  
  std::vector<std::string>            m_nameVec;
  std::map<std::string, ProblemType>  m_nameMap;

  void addBit( ProblemType pb, const std::string& name);
  int index( ProblemType pb) const {return m_index[pb];}

};

#endif

