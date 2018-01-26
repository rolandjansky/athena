//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADDATA_H
#define NSWL1_PADDATA_H

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include <ostream>
#include <vector>

namespace NSWL1 {

class PadData;

/**
   @brief Holder of the pad trigger information
   
   Stores info about the pads that were used to compute this trigger,
   as well as the resulting trigger primitive.

*/
struct PadTrigger {
    BandId_t m_bandid; ///< see bandId below
    float m_eta;
    float m_phi;
    int   m_eta_id;
    int   m_phi_id;
    int   m_multiplet_id;
    int   m_gasGap_id;
    std::vector<std::vector<float>> m_pad_strip_info; // hack to match pads to strips until we fix the band id issue
                                                      // m_pad_strip_info.size gives the number of pads in each PadTrigger
                                                      // m_pad_strip_info[i][0] gives the multilayer id of the ith pad in this trigger
                                                      // m_pad_strip_info[i][1] gives the gasgap id     of the ith pad in this trigger
                                                      // m_pad_strip_info[i][2] gives the local min y   of the ith pad in this trigger
                                                      // m_pad_strip_info[i][3] gives the local max y   of the ith pad in this trigger
    
    std::vector<const PadData*> m_pads; ///!< digits contributing to this trigger primitive
    /**
       for now computed as as SingleWedgePadTrigger::halfPadCoordinates().ieta
       
       \todo when we have an inner+outer candidate, will they get the same bandid?
       Also do we need to cache it as a datamember?
    */
    PadTrigger() : m_bandid(0), m_eta(0), m_phi(0), m_eta_id(0), m_phi_id(0), m_multiplet_id(0), m_gasGap_id(0), m_pad_strip_info{{0.}} {}

    BcTag_t bctag()     const;
    int sideId()        const; ///! sideId of the first pad in the trigger
    int sectorId()      const; ///! sectorId of the first pad in the trigger
    BandId_t bandId()   const { return m_bandid; }
    float eta()         const { return m_eta;    } ///! SectorTriggerCandidate eta center
    float phi()         const { return m_phi;    } ///! SectorTriggerCandidate phi center 
    float etaId()       const { return m_eta_id; } ///! SectorTriggerCandidate pad eta index 
    float phiId()       const { return m_phi_id; } ///! SectorTriggerCandidate pad phi index
    float multipletId() const { return m_multiplet_id; } ///! SectorTriggerCandidate pad multiplet index 
    float gasGapId()    const { return m_gasGap_id;    } ///! SectorTriggerCandidate pad gasGap index
};

} // namespace NSWL1

/// \todo
// std::ostream& operator<<(std::ostream& stream, NSWL1::PadTrigger& pt);

#endif
