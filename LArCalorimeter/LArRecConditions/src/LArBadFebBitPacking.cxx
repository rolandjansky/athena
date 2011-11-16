/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArBadFebBitPacking.h"
#include "LArRecConditions/LArBadFeb.h"

LArBadFebBitPacking::LArBadFebBitPacking() : 
  m_index( 8*sizeof(BitWord),-1)
{
  // Here, specify each problem and its name
  addBit( deadAllBit, "deadAll");
  addBit( deadReadoutBit, "deadReadout");
  addBit( inErrorBit, "inError");
  addBit( deactivatedInOKSBit, "deactivatedInOKS");
  addBit( maskParity, "maskParity");
  addBit( maskBCID, "maskBCID");
  addBit( maskSampleHeader, "maskSampleHeader");
  addBit( maskEVTID, "maskEVTID");
  addBit( maskScacStatus, "maskScacStatus");
  addBit( maskScaOutOfRange, "maskScaOutOfRange");
  addBit( maskGainMismatch, "maskGainMismatch");
  addBit( maskTypeMismatch, "maskTypeMismatch");
  addBit( maskNumOfSamples, "maskNumOfSamples");
  addBit( maskEmptyDataBlock, "maskEmptyDataBlock");
  addBit( maskDspBlockSize, "maskDspBlockSize");
  addBit( maskCheckSum, "maskCheckSum");
  addBit( maskMissingHeader, "maskMissingHeader");
  addBit( maskBadGain, "maskBadGain");

  for (unsigned int i=0; i<m_enumVec.size(); i++) {
    m_index[m_enumVec[i]] = i;
  }
}

void LArBadFebBitPacking::addBit( ProblemType pb, const std::string& name)
{
  m_enumVec.push_back(pb);
  m_nameVec.push_back(name);
  m_nameMap[name] = pb;
}


  const std::string& LArBadFebBitPacking::stringName( ProblemType pb) const 
  {
    return m_nameVec.at(index(pb));
  }

  std::pair<bool, LArBadFebEnum::ProblemType>
    LArBadFebBitPacking::enumName( const std::string& str) const
  {
    std::map<std::string, ProblemType>::const_iterator i = m_nameMap.find( str);
    if (i != m_nameMap.end()) { return std::make_pair(true,i->second);}
    else return std::make_pair(false, (ProblemType) 0);
  }


  void LArBadFebBitPacking::setBit( ProblemType pb, BitWord& word, bool value) const {
    BitWord mask = 1;
    mask = mask << bitPosition(pb); // all at 0 except pb bit
    if (value) word |= mask;
    else word &= ~mask;
  }

  bool LArBadFebBitPacking::setBit( const std::string& name, 
				    BitWord& word, bool value) const
  {
    std::pair<bool, ProblemType> res = enumName( name);
    if (res.first) { // name corresponds to enum
      setBit( res.second, word, value);
      return true;
    }
    else return false; // name not recognized
  }

  void LArBadFebBitPacking::setBit( ProblemType pb, LArBadFeb& word, bool value) const
  {
    setBit( pb, word.packedDataRef(), value);
  }

  bool LArBadFebBitPacking::setBit( const std::string& name, LArBadFeb& word, bool value) const
  {
    return setBit( name, word.packedDataRef(), value);
  }

  std::string LArBadFebBitPacking::stringStatus( const LArBadFeb& bc) const
  {
    std::string res;
    for (unsigned int i=0; i<m_enumVec.size(); ++i) {
      if ( ! bc.statusOK( m_enumVec.at(i))) {
	if (!res.empty()) res += " ";
	res += m_nameVec.at(i);
      }
    }
    return res;
  }
