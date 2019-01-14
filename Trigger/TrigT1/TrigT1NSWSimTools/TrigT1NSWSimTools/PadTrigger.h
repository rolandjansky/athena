//  -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADDATA_H
#define NSWL1_PADDATA_H

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include <vector>


namespace NSWL1 {
    class PadData;
    class SectorTriggerCandidate;
    /**
    @brief Holder of the pad trigger information
    
    Stores info about the pads that were used to compute this trigger,
    as well as the resulting trigger primitive.

    */
    struct PadTrigger {
        uint16_t m_bandid; ///< see bandId below
        float m_eta;
        float m_phi;
        float m_etamin;
        float m_etamax;
        float m_phimin;
        float m_phimax;
        int   m_eta_id;
        int   m_phi_id;
        int   m_multiplet_id;
        int   m_gasGap_id;
        int   m_isSmall;
        std::vector<float> m_trglocalminY;
        std::vector<float> m_trglocalmaxY;
        std::vector<std::vector<float>> m_pad_strip_info; // hack to match pads to strips until we fix the band id issue
                                                        // m_pad_strip_info.size gives the number of pads in each PadTrigger
                                                        // m_pad_strip_info[i][0] gives the multilayer id of the ith pad in this trigger
                                                        // m_pad_strip_info[i][1] gives the gasgap id     of the ith pad in this trigger
                                                        // m_pad_strip_info[i][2] gives the local min y   of the ith pad in this trigger
                                                        // m_pad_strip_info[i][3] gives the local max y   of the ith pad in this trigger
        
        std::vector< std::shared_ptr<PadData>> m_pads; ///!< digits contributing to this trigger primitive
        
        /**
        for now computed as as SingleWedgePadTrigger::halfPadCoordinates().ieta
        
        \todo when we have an inner+outer candidate, will they get the same bandid?
        Also do we need to cache it as a datamember?
        */
        PadTrigger() : m_bandid(0), m_eta(0), m_phi(0), m_eta_id(0), m_phi_id(0), m_multiplet_id(0), m_gasGap_id(0), m_pad_strip_info{{0.}} {
        
        }

        uint16_t bctag()     const;
        int sideId()        const; ///! sideId of the first pad in the trigger
        int sectorId()      const; ///! sectorId of the first pad in the trigger
        uint16_t bandId()   const { return m_bandid; }
        float eta()         const { return m_eta;    } ///! SectorTriggerCandidate eta center
        float phi()         const { return m_phi;    } ///! SectorTriggerCandidate phi center 
        float etaId()       const { return m_eta_id; } ///! SectorTriggerCandidate pad eta index 
        float phiId()       const { return m_phi_id; } ///! SectorTriggerCandidate pad phi index
        float multipletId() const { return m_multiplet_id; } ///! SectorTriggerCandidate pad multiplet index 
        float gasGapId()    const { return m_gasGap_id;    } ///! SectorTriggerCandidate pad gasGap index
        float etaMin()      const {return m_etamin;}
        float etaMax()      const {return m_etamax;}
        float phiMin()      const {return m_phimin;}
        float phiMax()      const {return m_phimax;}
        int isSmall ()      const {return m_isSmall;}
        
        std::vector<float> const trglocalminY() { return m_trglocalminY; }
        std::vector<float> const trglocalmaxY() { return m_trglocalmaxY; }
    };

} // namespace NSWL1


#endif
