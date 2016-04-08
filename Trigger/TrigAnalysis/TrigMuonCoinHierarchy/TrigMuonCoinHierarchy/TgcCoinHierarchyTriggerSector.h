/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_TGCCOINHIERARCHYTRIGGERSECTOR_H
#define TRIGGER_TGCCOINHIERARCHYTRIGGERSECTOR_H

#include "TrigMuonCoinHierarchy/TgcCoinHierarchy.h"

namespace Trigger {
  class TgcCoinHierarchyTriggerSector {
  public:
    /** Constructor */
    TgcCoinHierarchyTriggerSector();
    /** Destructor */
    virtual ~TgcCoinHierarchyTriggerSector() {};

    /** Add a coincidence */
    unsigned int addCoincidence(const Muon::TgcCoinData* coin, const TgcCoinHierarchy::COINCIDENCE coinType);
    /** Add a hit */
    unsigned int addHit(const Muon::TgcPrepData* hit, const TgcCoinHierarchy::TYPE hitType, 
                        const TgcCoinHierarchy::STATION station);

    /** Get a coincidence */
    const Muon::TgcCoinData* getCoincidence(const unsigned int id, const TgcCoinHierarchy::COINCIDENCE coinType) const;
    /** Get a hit */
    const Muon::TgcPrepData* getHit(const unsigned int id, const TgcCoinHierarchy::TYPE hitType, 
                                    const TgcCoinHierarchy::STATION station) const;

    /** Get the number of coincidences which are coinType in the TgcCoinHierarchyTriggerSector */ 
    unsigned int numCoincidences(const TgcCoinHierarchy::COINCIDENCE coinType) const;
    /** Get the number of hitType (wire or strip) hits in station */
    unsigned int numHits(const TgcCoinHierarchy::TYPE hitType, const TgcCoinHierarchy::STATION station) const;

    /** Set an associated flag for a coincidence */
    void setCoincidenceAssociated(const unsigned int id, const TgcCoinHierarchy::COINCIDENCE coinType);
    /** Set an associated flag for a hit */
    void setHitAssociated(const unsigned int id, const TgcCoinHierarchy::TYPE hitType, 
                          const TgcCoinHierarchy::STATION station);
                                           
    /** Check a coincidence is already associated */
    bool isCoincidenceAssociated(const unsigned int id, const TgcCoinHierarchy::COINCIDENCE coinType) const;
    /** Check a hit is already associated */
    bool isHitAssociated(const unsigned int id, const TgcCoinHierarchy::TYPE hitType, 
                         const TgcCoinHierarchy::STATION station) const;

    TgcCoinHierarchy::TIMING setTiming(const TgcCoinHierarchy::TIMING indexBcid);
    TgcCoinHierarchy::TIMING getTiming()const;

  private:
    /** Vectors of coincidence pointers */
    std::vector<const Muon::TgcCoinData*> m_coins[TgcCoinHierarchy::NCOINCIDENCETYPES][TgcCoinHierarchy::NTIMING];
    /** Vectors of hit pointers */
    std::vector<const Muon::TgcPrepData*> m_hits[TgcCoinHierarchy::NTYPES][TgcCoinHierarchy::NSTATIONS][TgcCoinHierarchy::NTIMING];
    /** Flags of associations for coincidences */
    std::vector<bool> m_coinsAssociated[TgcCoinHierarchy::NCOINCIDENCETYPES][TgcCoinHierarchy::NTIMING];
    /** Flags of associations for hits */
    std::vector<bool> m_hitsAssociated[TgcCoinHierarchy::NTYPES][TgcCoinHierarchy::NSTATIONS][TgcCoinHierarchy::NTIMING];

    /** Timing*/
    TgcCoinHierarchy::TIMING m_bc;

  };
} // end of namespace Trigger

#endif // TRIGGER_TGCCOINHIERARCHYTRIGGERSECTOR_H
