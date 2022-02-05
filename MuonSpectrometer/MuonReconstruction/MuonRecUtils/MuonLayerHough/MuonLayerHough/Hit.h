/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGH_HIT_H
#define MUONHOUGH_HIT_H

#include <functional>
#include <map>
#include <memory>

#include "CxxUtils/fpcompare.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Trk {
    class PrepRawData;
}
namespace Muon {
    class TgcClusterObj3D;
}

namespace MuonHough {
    static constexpr int UNINITIALIZED = -99999;
    /// struct containing additional debug information on the hits that is not needed for the actual alg
    /// but very useful for debugging
    class HitDebugInfo {
    public:
        HitDebugInfo() = default;

        HitDebugInfo(int type_, int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_, Muon::MuonStationIndex::LayerIndex layer_,
                     int sublayer_);

        int type{UNINITIALIZED};    /// technology type
        int sector{UNINITIALIZED};  /// sector
        Muon::MuonStationIndex::DetectorRegionIndex region{
            Muon::MuonStationIndex::DetectorRegionUnknown};                              /// detector region (endcapA/barrel/endcapC)
        Muon::MuonStationIndex::LayerIndex layer{Muon::MuonStationIndex::LayerUnknown};  /// layer (inner/middle/outer)
        int sublayer{UNINITIALIZED};                                                     /// sublayer within layer
        int pdgId{UNINITIALIZED};                                                        /// pdgId of the associated truth particle (if any)
        int barcode{UNINITIALIZED};                                                      /// barcode of truth particle
        int muonIndex{UNINITIALIZED};                                                    /// index of reconstructed muon
        int clusterSize{UNINITIALIZED};                                                  /// cluster size
        int clusterLayers{UNINITIALIZED};                                                /// number of layers in the cluster
        bool isEtaPhi{false};                                                            /// flag whether confirmed in the other projection
        bool trigConfirm{false};                                                         /// flag whether confirmed by a trigger hit
        int binpos{UNINITIALIZED};    /// spacial bin of the hit with the highest value in the hough space
        int bintheta{UNINITIALIZED};  /// angular bin of the hit with the highest value in the hough space
        float time{UNINITIALIZED};    /// measured time
        float r{UNINITIALIZED};       /// drift radius for MDT, strip pitch for strips
        float ph{UNINITIALIZED};      /// maximum value in the hough space
        float phn{UNINITIALIZED};     /// maximum value in the hough space in neighbouring sector; dege
        float ph1{UNINITIALIZED};     /// maximum value of the hit projected into the first other layer in the hough space
        float ph2{UNINITIALIZED};     /// maximum value of the hit projected into the second other layer in the hough space
        float rot{UNINITIALIZED};     /// angle corresponding to the maximum in the hough space; poin to IP
    };

    /// struct containing all hit information needed for the Hough transform
    class Hit {
    public:
        /// constructor, takes ownership of the HitDebugInfo pointer
        Hit(int layer_, float x_, float ymin_, float ymax_, float w_, HitDebugInfo* d_ = nullptr, const Trk::PrepRawData* prd_ = nullptr,
            const Muon::TgcClusterObj3D* tgc_ = nullptr);

        /// destructor
        ~Hit();

        /// copy constructor
        Hit(const Hit& h_);

        /// =operator
        Hit& operator=(const Hit& h_);

        int layer{UNINITIALIZED};   /// layer identifier (packed word containing technology/sublayer)
        float x{UNINITIALIZED};     /// global hit position (x=r in barrel, x=z in endcap)
        float ymin{UNINITIALIZED};  /// minimum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
        float ymax{UNINITIALIZED};  /// maximum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
        float w{UNINITIALIZED};     /// weight of the hit

        /// access to debug information
        const HitDebugInfo* debugInfo() const { return m_debug.get(); }
        HitDebugInfo* debugInfo() { return m_debug.get(); }

        /// access to assiciated hit, either the prd or the tgc pointer is set in athena
        const Trk::PrepRawData* prd{nullptr};
        const Muon::TgcClusterObj3D* tgc{nullptr};

    private:
        std::unique_ptr<HitDebugInfo> m_debug{nullptr};  /// pointer to debug information

        /// copy function for internal use
        void copy(const Hit& hit);
    };

    /// struct containing all hit information needed for the Hough transform
    struct PhiHit {
        /// constructor, takes ownership of the HitDebugInfo pointer
        PhiHit(int layer_, float r_, float phimin_, float phimax_, float w_, HitDebugInfo* d_ = 0, const Trk::PrepRawData* prd_ = 0,
               const Muon::TgcClusterObj3D* tgc_ = 0);

        /// destructor
        ~PhiHit();

        /// copy constructor
        PhiHit(const PhiHit& h_);

        /// =operator
        PhiHit& operator=(const PhiHit& h_);

        int layer{UNINITIALIZED};     /// layer identifier (packed word containing technology/sublayer)
        float r{UNINITIALIZED};       /// global hit position (x=r in barrel, x=z in endcap)
        float phimin{UNINITIALIZED};  /// minimum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
        float phimax{UNINITIALIZED};  /// maximum value of the hit in the precision coordinate (y=z in barrel, y=r in endcap)
        float w{UNINITIALIZED};       /// weight of the hit

        /// access to debug information
        const HitDebugInfo* debugInfo() const { return m_debug.get(); }
        HitDebugInfo* debugInfo() { return m_debug.get(); }

        /// access to assiciated hit, either the prd or the tgc pointer is set in athena
        const Trk::PrepRawData* prd{nullptr};
        const Muon::TgcClusterObj3D* tgc{nullptr};

    private:
        std::unique_ptr<HitDebugInfo> m_debug{nullptr};  /// pointer to debug information

        /// copy function for internal use
        void copy(const PhiHit& hit);
    };

    /// struct containing truth or track information
    struct MuonDebugInfo {
        int pdgId{UNINITIALIZED};      /// pdgId of the true muon
        int barcode{UNINITIALIZED};    /// barcode of the true muon
        int muonIndex{UNINITIALIZED};  /// index of the associated track
        float pt{UNINITIALIZED};       /// pt
        float eta{UNINITIALIZED};      /// eta
        float phi{UNINITIALIZED};      /// phi

        // number of reconstructed hits
        int nmdts{0};
        int nrpcs{0};
        int ntgcs{0};
        int ncscs{0};
        int nmms{0};
        int nstgcs{0};

        // number of true hits
        int ntmdts{0};
        int ntrpcs{0};
        int nttgcs{0};
        int ntcscs{0};
        int ntmms{0};
        int ntstgcs{0};
    };

    /// struct containing truth or segment information
    struct SegDebugInfo {
        float sposx{UNINITIALIZED};
        float sposy{UNINITIALIZED};
        float sposz{UNINITIALIZED};
        float sdirx{UNINITIALIZED};
        float sdiry{UNINITIALIZED};
        float sdirz{UNINITIALIZED};
        int snPrecHits{0};
        int snTrigHits{0};
        int sSector{0};
        int sChIndex{0};
    };

    /// struct to sort the hits
    struct SortHitsPerLayer {
        bool operator()(const Hit& hit1, const Hit& hit2) const {
            return compare(hit1.ymin, hit2.ymin, hit1.layer, hit2.layer, hit1.debugInfo(), hit2.debugInfo());
        }

        bool operator()(const PhiHit& hit1, const PhiHit& hit2) const {
            return compare(hit1.phimin, hit2.phimin, hit1.layer, hit2.layer, hit1.debugInfo(), hit2.debugInfo());
        }

        bool compare(float val1, float val2, int lay1, int lay2, const HitDebugInfo* db1, const HitDebugInfo* db2) const {
            if (db1 && db2) {
                if (db1->sector != db2->sector) return db1->sector < db2->sector;
                if (db1->region != db2->region) return db1->region < db2->region;
                if (db1->type != db2->type) return db1->type < db2->type;
                if (db1->layer != db2->layer) return db1->layer < db2->layer;
                if (db1->sublayer != db2->sublayer) return db1->sublayer < db2->sublayer;
            } else {
                if (lay1 != lay2) return lay1 < lay2;
            }
            return CxxUtils::fpcompare::less(val1, val2);
        }

        bool operator()(const Hit* hit1, const Hit* hit2) const { return operator()(*hit1, *hit2); }
        bool operator()(const PhiHit* hit1, const PhiHit* hit2) const { return operator()(*hit1, *hit2); }
   
        bool operator()(const std::shared_ptr<Hit>& hit1, const std::shared_ptr<Hit>& hit2) const { return operator()(*hit1, *hit2); }
        bool operator()(const std::shared_ptr<PhiHit>& hit1, const std::shared_ptr<PhiHit>& hit2) const { return operator()(*hit1, *hit2); }
   
    };

}  // namespace MuonHough

#endif
