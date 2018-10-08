/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecConditions/LArBadChanBitPacking.h"
#include "LArRecConditions/LArBadChannel.h"

LArBadChanBitPacking::LArBadChanBitPacking() : 
  m_bitPos( 8*sizeof(BitWord),0),
  m_index( 8*sizeof(BitWord),-1),
  m_highGainMask(0),
  m_mediumGainMask(0),
  m_lowGainMask(0)
{
  // Here, specify each problem and its gain dependence.
  addBit( deadReadoutBit, "deadReadout", independent);
  addBit( deadCalibBit, "deadCalib", independent);
  addBit( deadPhysBit, "deadPhys", independent);
  addBit( almostDeadBit, "almostDead", independent);
  addBit( shortBit, "short", independent);
  addBit( unstableBit, "unstable", independent);
  addBit( distortedBit, "distorted", independent);
  addBit( lowNoiseHGBit, "lowNoiseHG", high);
  addBit( highNoiseHGBit, "highNoiseHG", high);
  addBit( unstableNoiseHGBit, "unstableNoiseHG", high);
  addBit( lowNoiseMGBit, "lowNoiseMG", medium);
  addBit( highNoiseMGBit, "highNoiseMG", medium);
  addBit( unstableNoiseMGBit, "unstableNoiseMG", medium);
  addBit( lowNoiseLGBit, "lowNoiseLG", low);
  addBit( highNoiseLGBit, "highNoiseLG", low);
  addBit( unstableNoiseLGBit, "unstableNoiseLG", low);
  addBit( missingFEBBit, "missingFEB", independent);
  addBit( peculiarCalibrationLineBit, "peculiarCalibrationLine", low);
  addBit( problematicForUnknownReasonBit, "problematicForUnknownReason", independent);
  addBit( sporadicBurstNoiseBit, "sporadicBurstNoise", independent);
  addBit( deadSCACellBit, "deadSCACell", independent);
  addBit( badFirstSampleBit, "badFirstSample", independent);
  addBit( unflaggedByLADIeSBit, "unflaggedByLADIeS", independent);

  for (unsigned int i=0; i<m_enumVec.size(); i++) {
    m_nameMap[m_nameVec[i]] = m_enumVec[i].first;
    /// Initial assignment of bit positions same as enumerator values.
    /// This may be changed FIXME provide method for changing it
    m_bitPos[m_enumVec[i].first] = m_enumVec[i].first;

    m_index[m_enumVec[i].first] = i;
  }

  initMasks(); // initialize the gain masks
}

void LArBadChanBitPacking::addBit( ProblemType pb, const std::string& name, 
				   GainDependence gaindep)
{
  m_enumVec.push_back( std::make_pair( pb, gaindep));
  m_nameVec.push_back(name);
}


const std::string& LArBadChanBitPacking::stringName( ProblemType pb) const 
{
  return m_nameVec.at(index(pb));
}

std::pair<bool, LArBadChannelEnum::ProblemType>
LArBadChanBitPacking::enumName( const std::string& str) const
{
  std::map<std::string, ProblemType>::const_iterator i = m_nameMap.find( str);
  if (i != m_nameMap.end()) { return std::make_pair(true,i->second);}
  else return std::make_pair(false, (ProblemType) 0);
}


void LArBadChanBitPacking::setBit( ProblemType pb, BitWord& word, bool value) const {
  BitWord mask = 1;
  mask = mask << m_bitPos[static_cast<PosType>(pb)]; // all at 0 except pb bit
  if (value) word |= mask;
  else word &= ~mask;
}

bool LArBadChanBitPacking::setBit( const std::string& name, 
				   BitWord& word, bool value) const
{
  std::pair<bool, ProblemType> res = enumName( name);
  if (res.first) { // name corresponds to enum
    setBit( res.second, word, value);
    return true;
  }
  else return false; // name not recognized
}

void LArBadChanBitPacking::setBit( ProblemType pb, LArBadChannel& word, bool value) const
{
  setBit( pb, word.packedDataRef(), value);
}

bool LArBadChanBitPacking::setBit( const std::string& name, LArBadChannel& word, bool value) const
{
  return setBit( name, word.packedDataRef(), value);
}

std::string LArBadChanBitPacking::stringStatus( const LArBadChannel& bc) const
{
  std::string res;
  for (unsigned int i=0; i<m_enumVec.size(); ++i) {
    if ( ! bc.statusOK( m_enumVec.at(i).first  )) {
      if (!res.empty()) res += " ";
      res += m_nameVec.at(i);
    }
  }
  return res;
}

void LArBadChanBitPacking::initMasks()
{
  for( std::vector< std::pair<ProblemType, GainDependence> >::const_iterator iter = m_enumVec.begin();
       iter != m_enumVec.end(); ++iter)  //loop over all problems
  {
    // set the appropriate bits in each mask according to the GainDependence of the ProblemType
    if(iter->second == independent)
    {
      setBit(iter->first, m_highGainMask);
      setBit(iter->first, m_mediumGainMask);
      setBit(iter->first, m_lowGainMask);
    }
    else if(iter->second == high)
      setBit(iter->first, m_highGainMask);
    else if(iter->second == medium)
      setBit(iter->first, m_mediumGainMask);
    else if(iter->second == low)
      setBit(iter->first, m_lowGainMask);
  }
}
