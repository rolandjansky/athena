/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanBitPacking_H
#define LArBadChanBitPacking_H

#include "LArRecConditions/LArBadChannelEnum.h"
#include <string>
#include <vector>
#include <map>

class LArBadChannel;

class LArBadChanBitPacking : public LArBadChannelEnum {
 public:

  typedef std::vector<PosType>        PosContainer;

  LArBadChanBitPacking();	

  const std::string& stringName( ProblemType pb) const;

  std::pair<bool, ProblemType> enumName( const std::string& str) const;

  PosType bitPosition( ProblemType pb) const {return m_bitPos[static_cast<PosType>(pb)];}

  PosType wordSize() const {return sizeof(BitWord);}

  void setBit( ProblemType pb, BitWord& word, bool value=true) const;
  void setBit( ProblemType pb, LArBadChannel& word, bool value=true) const;

  bool setBit( const std::string& name, BitWord& word, bool value=true) const;
  bool setBit( const std::string& name, LArBadChannel& word, bool value=true) const;

  std::string stringStatus( const LArBadChannel& bc) const;

  // These BitWord masks are used to do gain-dependent masking in LArBadChannelMasker.
  BitWord highGainMask() const   {return m_highGainMask;}
  BitWord mediumGainMask() const {return m_mediumGainMask;}
  BitWord lowGainMask() const    {return m_lowGainMask;}

  int numberOfProblemTypes() const { return m_nameVec.size();}

 private:

  // This is used to describe the dependence of each ProblemType on LAr gain.
  enum GainDependence { independent, low, medium, high};

  PosContainer               m_bitPos;
  std::vector<int>           m_index;
  std::vector< std::pair<ProblemType, GainDependence> >   m_enumVec;  

  std::vector<std::string>   m_nameVec;
  std::map<std::string, ProblemType>  m_nameMap;

  // These are non-static so that different LArBadChanBitPacking versions can be used simultaneously.
  BitWord m_highGainMask;
  BitWord m_mediumGainMask;
  BitWord m_lowGainMask;

  void addBit( ProblemType pb, const std::string& name, GainDependence gaindep);
  int index( ProblemType pb) const {return m_index[pb];}
  void initMasks();
};

#endif

