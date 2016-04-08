/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHY_H
#define TRIGGER_TGCCOINHIERARCHY_H

#include "MuonTrigCoinData/TgcCoinData.h"
#include "MuonPrepRawData/TgcPrepData.h"

#include <vector> 

class MsgStream;

namespace Trigger {
  class TgcCoinHierarchy {
  public:
    /** enum for wire and strip */ 
    enum TYPE { 
      WIRE = 0,
      STRIP = 1,
      NTYPES = 2
    };
    /** enum for stations */
    enum STATION {
      NOTSTATION = -1,
      TGC1 = 0,
      TGC2 = 1, 
      TGC3 = 2,
      NSTATIONS = 3
    };
    /** enum for coincidence types. This enum should not be changed because algorithms depend on these relations */
    enum COINCIDENCE {
      NOTCOINCIDENCE = -1,
      TRACKLETSTRIP = 0,
      TRACKLETWIRE = 1,
      HIPTSTRIP = 2,
      HIPTWIRE = 3,
      SL = 4,
      NCOINCIDENCETYPES = 5 
    };
    /** enum for the numbers of sides (Side A and Side C), regions (Forward and Endcap) and trigger sectors */
    enum SIZES {
      NSIDES = 2,
      NREGIONS = 2,
      NPHIS = 48+1 // Endcap phi starts from 1 and ends at 48
    };

    /** enum for the numbers of timing type */
    enum TIMING {
      PRIOR = 0,
      CURR  = 1,
      NEXT  = 2,
      NTIMING  = 3,
      UNKNOWN  = -1
    };


    /** Constructor */
    TgcCoinHierarchy();

    /** Destructor */
    ~TgcCoinHierarchy();

    /** Set TgcCoinData with Muon::TgcCoinData::CoinDataType enum and isStrip */
    bool setCoincidence(const Muon::TgcCoinData* coinData, const Muon::TgcCoinData::CoinDataType coinDataType, 
                        const bool isStrip=false, const TIMING indexBcid=UNKNOWN);
    /** Set TgcCoinData with Trigger::TgcCoinHierarchy::COINCIDENCE enum */
    bool setCoincidence(const Muon::TgcCoinData* coinData, const COINCIDENCE coincidence, const TIMING indexBcid=UNKNOWN);
    /** Get TgcCoinData with Muon::TgcCoinData::CoinDataType enum and isStrip */
    const Muon::TgcCoinData* getCoincidence(const Muon::TgcCoinData::CoinDataType coinDataType, 
                                            const bool isStrip=false) const;
    /** Get TgcCoinData with Trigger::TgcCoinHierarchy::COINCIDENCE enum */
    const Muon::TgcCoinData* getCoincidence(const COINCIDENCE coincidence) const;
    /** Check the existence of coincidence with Muon::TgcCoinData::CoinDataType enum and isStrip */
    bool hasCoincidence(const Muon::TgcCoinData::CoinDataType coinDataType, const bool isStrip=false) const;
    /** Check the existence of coincidence with gcCoinHierarchy::COINCIDENCE enum */
    bool hasCoincidence(const COINCIDENCE coincidence) const;

    /** Set a TgcPrepData */
    bool setHit(const Muon::TgcPrepData* hit, const bool isStrip, const STATION station, const TIMING indexBcid=UNKNOWN);
    /** Get multiple TgcPrepData's */
    const std::vector<const Muon::TgcPrepData*>* getHits(const bool isStrip, const STATION station) const; 
    /** Get the number of TgcPrepData's */
    unsigned int numHits(const bool isStrip, const STATION station) const;

    /** Get the highest coincidece with Muon::TgcCoinData::CoinDataType enum */
    Muon::TgcCoinData::CoinDataType highestCoinDataType() const;
    /** Get the highest coincidece with Trigger::TgcCoinHierarchy::COINCIDENCE enum */
    COINCIDENCE highestCoincidence() const;

    /** Check any duplicated coincidences (Tracklet or HiPt) in the TgcCoinHierarchy */
    bool hasAnyDuplicatedCoincidence() const;
    /** Check duplicated coincidences (Tracklet or HiPt) in the TgcCoinHierarchy with Muon::TgcCoinData::CoinDataType 
        enum and isStrip */
    bool hasDuplicatedCoincidence(const Muon::TgcCoinData::CoinDataType coinDataType, const bool isStrip=false) const;

    /** Convert Muon::TgcCoinData::CoinDataType enum and isStrip to Trigger::TgcCoinHierarchy::COINCIDENCE enum */
    COINCIDENCE CoinDataTypeToCOINCIDENCE(const Muon::TgcCoinData::CoinDataType coinDataType, const bool isStrip) const;
    /** Convert isStrip to Trigger::TgcCoinHierarchy::TYPE enum */
    TYPE isStripToTYPE(const bool isStrip) const;
    /** Convert stationName to Trigger::TgcCoinHierarchy::STATION enum */
    STATION stationNameToSTATION(const int stationName) const;
    /** Get isForward from stationName */
    bool stationNameToIsForward(const int stationName) const;
    /** Convert integer to Trigger::TgcCoinHierarchy::COINCIDENCE enum */
    COINCIDENCE intToCOINCIDENCE(const int iCOINCIDENCE) const;

    /** Dump TgcCoinHierarchy */
    void dump(MsgStream& ms, MSG::Level level) const;


    /** TIMING tool  */
    TIMING uintToTiming(const unsigned int indexBcid)const;
    int    timingToInt(const TIMING indexBcid)const;
    TIMING getTimingCoin()const;
    TIMING getTimingHit()const;
  private:
    /** Convert Trigger::TgcCoinHierarchy::COINCIDENCE enum to Muon::TgcCoinData::CoinDataType enum */
    Muon::TgcCoinData::CoinDataType COINCIDENCEToCoinDataType(const COINCIDENCE coincidence) const;
    /** Convert isStrip to unsigned integer */
    unsigned int isStripToUnsignedInt(const bool isStrip) const;

    /** Pointer of each coincidence */
    const Muon::TgcCoinData* m_coincidence[NCOINCIDENCETYPES];
    /** Pointer of hits in each station and type (wire or strip) */
    std::vector<const Muon::TgcPrepData*> m_Hits[NTYPES][NSTATIONS];

    /** Flags for duplicated coincidences */
    bool m_duplicatedCoincidence[NCOINCIDENCETYPES];
    TIMING m_bc_coin;
    TIMING m_bc_hit;

  };
} // end of namespace Trigger

#endif // TRIGGER_TGCCOINHIERARCHY_H
