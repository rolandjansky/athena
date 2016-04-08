/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

#include "GaudiKernel/MsgStream.h"

using Muon::TgcCoinData;
using Muon::TgcPrepData;

namespace Trigger {
  TgcCoinHierarchy::TgcCoinHierarchy() {
    // Initialization for coincidences
    for(int iCOINTYPE=0; iCOINTYPE<NCOINCIDENCETYPES; iCOINTYPE++) {
      m_coincidence[iCOINTYPE] = 0;
      m_duplicatedCoincidence[iCOINTYPE] = false;
    }

    // Initialization for hits
    for(int iTYPE=0; iTYPE<NTYPES; iTYPE++) {
      for(int iSTATION=0; iSTATION<NSTATIONS; iSTATION++) {
        // Nothing done for hits  
      }
    }
    m_bc_coin = TgcCoinHierarchy::UNKNOWN;
    m_bc_hit  = TgcCoinHierarchy::UNKNOWN;
  }
  
  TgcCoinHierarchy::~TgcCoinHierarchy() {
  }
   
  bool TgcCoinHierarchy::setCoincidence(const TgcCoinData* coinData, 
					const TgcCoinData::CoinDataType coinDataType, 
					const bool isStrip, 
					const TIMING indexBcid) {
    const COINCIDENCE coincidence = CoinDataTypeToCOINCIDENCE(coinDataType, isStrip);

    return setCoincidence(coinData, coincidence, indexBcid);
  }

  bool TgcCoinHierarchy::setCoincidence(const TgcCoinData* coinData, 
					const TgcCoinHierarchy::COINCIDENCE coincidence, 
					const TIMING indexBcid
					) {
    // Check that the input TgcCoinData pointer is not NULL
    if(!coinData) return false;
    // Check that the input TgcCoinHierarchy::COINCIDENCE enum is valid 
    if(coincidence<=NOTCOINCIDENCE || coincidence>SL) return false;

    // Check that the input coincidence is duplicated
    if(m_coincidence[coincidence]) m_duplicatedCoincidence[coincidence] = true;
    // Set the input pointer 
    m_coincidence[coincidence] = coinData;
    // Set TIMING
    if(m_bc_coin==indexBcid or m_bc_coin==UNKNOWN){
      m_bc_coin = indexBcid;
    }
    else if(m_bc_coin!=indexBcid){
      /*
      std::cerr << "TIMING MISSMATCH:" << __FILE__ << ":" << __LINE__
		<< " m_bc_coin:" << m_bc_coin << " indexBcid:" << indexBcid
		<< std::endl;
      */
    }

    return true;
  }

  const TgcCoinData* TgcCoinHierarchy::getCoincidence(const TgcCoinData::CoinDataType coinDataType, const bool isStrip) const {
    const COINCIDENCE coincidence = CoinDataTypeToCOINCIDENCE(coinDataType, isStrip);

    return getCoincidence(coincidence);
  }

  const TgcCoinData* TgcCoinHierarchy::getCoincidence(const TgcCoinHierarchy::COINCIDENCE coincidence
						      ) const { 
    // Check that the input TgcCoinHierarchy::COINCIDENCE enum is valid 
    if(coincidence<=NOTCOINCIDENCE || coincidence>SL) return 0;

    return m_coincidence[coincidence];
  }

  bool TgcCoinHierarchy::hasCoincidence(const TgcCoinData::CoinDataType coinDataType,const bool isStrip) const {
    const COINCIDENCE coincidence = CoinDataTypeToCOINCIDENCE(coinDataType, isStrip);
    return hasCoincidence(coincidence);
  }

  bool TgcCoinHierarchy::hasCoincidence(const TgcCoinHierarchy::COINCIDENCE coincidence) const {
    if(coincidence<=NOTCOINCIDENCE || coincidence>SL) return false;
    
    // Check that the corresponding pointer is not NULL 
    return static_cast<bool>(m_coincidence[coincidence]);
  }

  bool TgcCoinHierarchy::setHit(const TgcPrepData* hitData, const bool isStrip
				,const STATION station, const TIMING indexBcid) {
    // Check that the input STATION enum is valid 
    if(station!=TGC1 && station!=TGC2 && station!=TGC3) return false;

    const unsigned int type = isStripToUnsignedInt(isStrip);    

    // Check that the input hit is not already added
    std::vector<const TgcPrepData*>::const_iterator it   = m_Hits[type][station].begin();
    std::vector<const TgcPrepData*>::const_iterator it_e = m_Hits[type][station].end();
    for(; it!=it_e; it++) {
      if(hitData->identify()==(*it)->identify()) return false;
    } 

    m_Hits[type][station].push_back(hitData);
    // Set TIMING
    if(m_bc_hit==indexBcid or m_bc_hit==UNKNOWN){
      m_bc_hit = indexBcid;
    }
    else if(m_bc_hit!=indexBcid){
      /*
      std::cerr << "TIMING MISSMATCH:" << __FILE__ << ":" << __LINE__
		<< " m_bc_hit:" << m_bc_hit << " indexBcid:" << indexBcid
		<< std::endl;
      */
    }

    return true;
  }

  const std::vector<const TgcPrepData*>* TgcCoinHierarchy::getHits(const bool isStrip, const STATION station) const {
    // Check that the input STATION enum is valid 
    if(station!=TGC1 && station!=TGC2 && station!=TGC3) return 0;

    const unsigned int type = isStripToUnsignedInt(isStrip);    

    return &m_Hits[type][station];
  }

  unsigned int TgcCoinHierarchy::numHits(const bool isStrip, const STATION station) const {
    // Check that the input STATION enum is valid 
    if(station!=TGC1 && station!=TGC2 && station!=TGC3) return 0;

    const unsigned int type = isStripToUnsignedInt(isStrip);    

    return m_Hits[type][station].size();
  }

  TgcCoinData::CoinDataType TgcCoinHierarchy::highestCoinDataType() const {
    return COINCIDENCEToCoinDataType(highestCoincidence());
  }

  TgcCoinHierarchy::COINCIDENCE TgcCoinHierarchy::highestCoincidence() const {
    // Return highest coincidence SL -> HIPTWIRE -> HIPTSTRIP -> TRACKLETWIRE -> TRACKLETSTRIP
    for(int iCOINCIDENCE=SL; iCOINCIDENCE>=TRACKLETSTRIP; iCOINCIDENCE--) {
      if(m_coincidence[iCOINCIDENCE]) return intToCOINCIDENCE(iCOINCIDENCE);
    }
    
    return NOTCOINCIDENCE;
  }

  bool TgcCoinHierarchy::hasAnyDuplicatedCoincidence() const {
    // bool is "isStrip" 
    return (hasDuplicatedCoincidence(TgcCoinData::TYPE_HIPT,     false) || 
            hasDuplicatedCoincidence(TgcCoinData::TYPE_HIPT,     true ) || 
            hasDuplicatedCoincidence(TgcCoinData::TYPE_TRACKLET, false) || 
            hasDuplicatedCoincidence(TgcCoinData::TYPE_TRACKLET, true ));
  }

  bool TgcCoinHierarchy::hasDuplicatedCoincidence(const TgcCoinData::CoinDataType coinDataType, 
						  const bool isStrip) const {
    const COINCIDENCE coincidence = CoinDataTypeToCOINCIDENCE(coinDataType, isStrip);
    if(coincidence<=NOTCOINCIDENCE || coincidence>=SL) return false; // SL should be unique

    return m_duplicatedCoincidence[coincidence];
  }

  TgcCoinHierarchy::COINCIDENCE TgcCoinHierarchy::CoinDataTypeToCOINCIDENCE(const TgcCoinData::CoinDataType coinDataType, 
									    const bool isStrip) const {
    if(coinDataType==TgcCoinData::TYPE_TRACKLET) { 
      if(isStrip) return TRACKLETSTRIP;
      return TRACKLETWIRE;
    } 
    if(coinDataType==TgcCoinData::TYPE_HIPT) { 
      if(isStrip) return HIPTSTRIP;
      return HIPTWIRE;
    } 
    if(coinDataType==TgcCoinData::TYPE_SL) return SL;
    return NOTCOINCIDENCE;
  }

  TgcCoinHierarchy::TYPE TgcCoinHierarchy::isStripToTYPE(const bool isStrip) const {
    if(isStrip) return STRIP;
    return WIRE;
  }

  TgcCoinHierarchy::STATION TgcCoinHierarchy::stationNameToSTATION(const int stationName) const {
    if(stationName==41 || stationName==42) return TGC1;
    if(stationName==43 || stationName==44) return TGC2;
    if(stationName==45 || stationName==46) return TGC3;
    return NOTSTATION;
  }

  bool TgcCoinHierarchy::stationNameToIsForward(const int stationName) const {
    // stationName==41, 43, 45 are Forward
    // stationName==42, 44, 46 are Endcap
    return stationName%2;
  }

  void TgcCoinHierarchy::dump(MsgStream& ms, MSG::Level level) const {
    unsigned int bc = timingToInt(m_bc_coin);
    ms << "TIMING COIN:" << bc << endreq;
    bc = timingToInt(m_bc_hit);
    ms << "TIMING HIT :" << bc << endreq;

    const TgcCoinData* highestCoin = this->getCoincidence(this->highestCoincidence());
    if(highestCoin) {
      ms << level << "*** TgcCoinHierarchy object : "
         << (highestCoin->isAside() ? "Side A " : "Side C ") 
         << (highestCoin->isForward() ? "Forward " : "Endcap  ") 
         << "phi=" << highestCoin->phi() 
         << " ***" << endreq; 

      // SL
      if(this->hasCoincidence(SL)) {
        const TgcCoinData* sl = this->getCoincidence(SL);
        ms << level << "Sector Logic   : trackletId=" << sl->trackletId() 
           << ", trackletIdStrip=" << sl->trackletIdStrip() << endreq;  
      } else {
        ms << level << "Sector Logic   : does not exist" << endreq; 
      }

      // HIPTWIRE 
      if(this->hasCoincidence(HIPTWIRE)) {
        const TgcCoinData* hw = this->getCoincidence(HIPTWIRE);
        ms << level << "HiPt Wire      : trackletId=" << hw->trackletId() 
	   << ", isDuplicated=" << (m_duplicatedCoincidence[HIPTWIRE] ? "YES" : "NO") << endreq;
      } else {
        ms << level << "HiPt Wire      : does not exist" << endreq; 
      }

      // HIPTSTRIP
      if(this->hasCoincidence(HIPTSTRIP)) {
        const TgcCoinData* hs = this->getCoincidence(HIPTSTRIP);
        ms << level << "HiPt Strip     : trackletId=" << hs->trackletId() 
	   << ", isDuplicated=" << (m_duplicatedCoincidence[HIPTSTRIP] ? "YES" : "NO") << endreq;
      } else {
        ms << level << "HiPt Strip     : does not exist" << endreq; 
      }

      // TRACKLETWIRE 
      if(this->hasCoincidence(TRACKLETWIRE)) {
        const TgcCoinData* tw = this->getCoincidence(TRACKLETWIRE);
        ms << level << "Tracklet Wire  : trackletId=" << tw->trackletId() 
	   << ", isDuplicated=" << (m_duplicatedCoincidence[TRACKLETWIRE] ? "YES" : "NO") << endreq;
      } else {
        ms << level << "Tracklet Wire  : does not exist" << endreq; 
      }

      // TRACKLETSTRIP
      if(this->hasCoincidence(TRACKLETSTRIP)) {
        const TgcCoinData* ts = this->getCoincidence(TRACKLETSTRIP);
        ms << level << "Tracklet Strip : trackletId=" << ts->trackletId() 
           << ", isDuplicated=" << (m_duplicatedCoincidence[TRACKLETSTRIP] ? "YES" : "NO") << endreq;
      } else {
        ms << level << "Tracklet Strip : does not exist" << endreq; 
      }

      // HITS
      for(unsigned int iWS=WIRE; iWS<=STRIP; iWS++) {
        bool isStrip = (iWS==STRIP ? true : false);
        for(unsigned int iST=TGC1; iST<=TGC3; iST++) {
          STATION station = (iST==TGC1 ? TGC1 : (iST==TGC2 ? TGC2 : TGC3));
          unsigned int numHits = this->numHits(isStrip, station);
          ms << level << "Hit " << (isStrip ? "Strip " : "Wire  ") 
             << "TGC" << iST+1 << " : " << numHits << " hits" << endreq;
        }
      }
    }
    else {
      ms << level << "*** TgcCoinHierarchy object : There is no highest coincidence. This is strange. ***" << endreq;
    }

    ms << level << "*******************************************************" << endreq;

  }

  TgcCoinData::CoinDataType TgcCoinHierarchy::COINCIDENCEToCoinDataType(const TgcCoinHierarchy::COINCIDENCE coincidence)
    const {
    if(coincidence==TRACKLETSTRIP || coincidence==TRACKLETWIRE) return TgcCoinData::TYPE_TRACKLET;
    if(coincidence==HIPTSTRIP || coincidence==HIPTWIRE) return TgcCoinData::TYPE_HIPT;
    if(coincidence==SL) return TgcCoinData::TYPE_SL;
    return TgcCoinData::TYPE_UNKNOWN;
  }

  unsigned int TgcCoinHierarchy::isStripToUnsignedInt(const bool isStrip) const {
    return static_cast<unsigned int>(isStrip);
  }

  TgcCoinHierarchy::COINCIDENCE TgcCoinHierarchy::intToCOINCIDENCE(const int iCOINCIDENCE) const {
    if(iCOINCIDENCE==NOTCOINCIDENCE) return NOTCOINCIDENCE;
    if(iCOINCIDENCE==TRACKLETSTRIP) return TRACKLETSTRIP;
    if(iCOINCIDENCE==TRACKLETWIRE) return TRACKLETWIRE;
    if(iCOINCIDENCE==HIPTSTRIP) return HIPTSTRIP;
    if(iCOINCIDENCE==HIPTWIRE) return HIPTWIRE;
    if(iCOINCIDENCE==SL) return SL;
    if(iCOINCIDENCE==NCOINCIDENCETYPES) return NCOINCIDENCETYPES;
    return NOTCOINCIDENCE;
  }

  TgcCoinHierarchy::TIMING TgcCoinHierarchy::uintToTiming(const unsigned int indexBcid) const {
    return (indexBcid==0)?TgcCoinHierarchy::PRIOR:
      (indexBcid==1)?TgcCoinHierarchy::CURR:
      (indexBcid==2)?TgcCoinHierarchy::NEXT:
      TgcCoinHierarchy::UNKNOWN;
  }

  int TgcCoinHierarchy::timingToInt(const TgcCoinHierarchy::TIMING indexBcid) const {
    return (indexBcid==PRIOR)?0:
      (indexBcid==CURR) ?1:
      (indexBcid==NEXT) ?2:
      -1;
  }



  TgcCoinHierarchy::TIMING TgcCoinHierarchy::getTimingCoin() const {
    return m_bc_coin;
  }

  TgcCoinHierarchy::TIMING TgcCoinHierarchy::getTimingHit() const {
    return m_bc_hit;
  }

} // end of namespace Trigger
