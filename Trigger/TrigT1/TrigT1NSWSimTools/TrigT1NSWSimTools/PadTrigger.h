//  -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_PADDATA_H
#define NSWL1_PADDATA_H

#include "TrigT1NSWSimTools/TriggerTypes.h"
#include "TrigT1NSWSimTools/GeoUtils.h"
#include <ostream>
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
        int   m_isSmall;
        int m_moduleIdInner;
        int m_moduleIdOuter;
        std::vector<float> m_trglocalminYInner;
        std::vector<float> m_trglocalmaxYInner;
        std::vector<float> m_trglocalminYOuter;
        std::vector<float> m_trglocalmaxYOuter;
        std::vector<int> m_trgSelectedLayersInner;
        std::vector<int> m_trgSelectedLayersOuter;
        std::vector<int> m_trgSelectedBandsInner;
        std::vector<int> m_trgSelectedBandsOuter;
        std::vector<int> m_trgPadPhiIndicesInner;
        std::vector<int> m_trgPadPhiIndicesOuter;
        std::vector<int> m_trgPadEtaIndicesInner;
        std::vector<int> m_trgPadEtaIndicesOuter;        
        
        std::vector<std::vector<float>> m_pad_strip_info; // hack to match pads to strips until we fix the band id issue
                                                        // m_pad_strip_info.size gives the number of pads in each PadTrigger
                                                        // m_pad_strip_info[i][0] gives the multilayer id of the ith pad in this trigger
                                                        // m_pad_strip_info[i][1] gives the gasgap id     of the ith pad in this trigger
                                                        // m_pad_strip_info[i][2] gives the local min y   of the ith pad in this trigger
                                                        // m_pad_strip_info[i][3] gives the local max y   of the ith pad in this trigger
        
        std::vector< std::shared_ptr<PadData>> m_pads; ///!< digits contributing to this trigger primitive
        std::vector< std::shared_ptr<PadData>> m_padsInner; ///!< digits contributing to this trigger primitive
        std::vector< std::shared_ptr<PadData>> m_padsOuter; ///!< digits contributing to this trigger primitive
        int m_triggerindex;
        /**
        for now computed as as SingleWedgePadTrigger::halfPadCoordinates().ieta
        
        \todo when we have an inner+outer candidate, will they get the same bandid?
        Also do we need to cache it as a datamember?
        */
        PadTrigger() : m_bandid(0), m_eta(0), m_phi(0), m_eta_id(0), m_phi_id(0),m_moduleIdInner(0),m_moduleIdOuter(0), m_multiplet_id(0), m_pad_strip_info{{0.}} {
        
        }

        uint16_t bctag()     const;
        int index() const { return m_triggerindex;}
        int sideId()        const; ///! sideId of the first pad in the trigger
        int sectorId()      const; ///! sectorId of the first pad in the trigger
        uint16_t bandId()   const { return m_bandid; }
        float eta()         const { return m_eta;    } ///! SectorTriggerCandidate eta center
        float phi()         const { return m_phi;    } ///! SectorTriggerCandidate phi center 
        float etaId()       const { return m_eta_id; } ///! SectorTriggerCandidate pad eta index 
        float phiId()       const { return m_phi_id; } ///! SectorTriggerCandidate pad phi index
        float multipletId() const { return m_multiplet_id; } ///! SectorTriggerCandidate pad multiplet index 
        float etaMin()      const {return m_etamin;}
        float etaMax()      const {return m_etamax;}
        float phiMin()      const {return m_phimin;}
        float phiMax()      const {return m_phimax;}
        int isSmall ()      const {return m_isSmall;}
        int moduleIdInner() const { return m_moduleIdInner;}
        int moduleIdOuter() const { return m_moduleIdOuter;}
        std::vector<float> const trglocalminYInner() const { return m_trglocalminYInner; }
        std::vector<float> const trglocalmaxYInner() const { return m_trglocalmaxYInner; }
        std::vector<float> const trglocalminYOuter() const { return m_trglocalminYOuter; }
        std::vector<float> const trglocalmaxYOuter() const { return m_trglocalmaxYOuter; }
        std::vector<int> trgSelectedLayersInner()const{ return m_trgSelectedLayersInner;}
        std::vector<int> trgSelectedLayersOuter() const { return m_trgSelectedLayersOuter; }
        std::vector<int> trgSelectedBandsInner() const { return m_trgSelectedBandsInner;}
        std::vector<int> trgSelectedBandsOuter() const { return m_trgSelectedBandsOuter;}
        std::vector<int> trgPadPhiIndicesInner() const { return m_trgPadPhiIndicesInner;}
        std::vector<int> trgPadPhiIndicesOuter() const { return  m_trgPadPhiIndicesOuter;}
        std::vector<int> trgPadEtaIndicesInner() const { return m_trgPadEtaIndicesInner;}
        std::vector<int> trgPadEtaIndicesOuter() const { return m_trgPadEtaIndicesOuter;}         
        
        
    };

} // namespace NSWL1

/// \todo
// std::ostream& operator<<(std::ostream& stream, NSWL1::PadTrigger& pt);

#endif
