/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHOUGH_HITNTUPLE_H
#define MUONHOUGH_HITNTUPLE_H

#include <map>
#include <vector>

#include "MuonLayerHough/MuonLayerHough.h"
#include "MuonLayerHough/MuonPhiLayerHough.h"
#include "MuonStationIndex/MuonStationIndex.h"

class TTree;

namespace MuonHough {

    struct DataIndex {
        DataIndex(int key) : m_data(key) {}
        DataIndex(int sector, int region, int layer, int type) : m_data(100000 * sector + 10000 * region + 100 * layer + type) {}
        int sector() const { return m_data / 100000; }
        Muon::MuonStationIndex::DetectorRegionIndex region() const {
            return static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(m_data / 10000 % 10);
        }
        Muon::MuonStationIndex::LayerIndex layer() const { return static_cast<Muon::MuonStationIndex::LayerIndex>(m_data / 100 % 10); }
        int type() const { return m_data % 100; }
        int key() const { return m_data; }
        int m_data;
    };
    bool operator<(const DataIndex& index1, const DataIndex& index2);

    typedef std::map<DataIndex, HitVec> SectorData;

    struct EventData {
        std::map<int, SectorData> sectors;

        void dump() const;
        static void dump(const std::map<int, SectorData>& data);
        void reset() { reset(sectors); }
        void reset(std::map<int, SectorData>& region) {
            region.clear();
        }
        void sort();
        static void sort(std::map<int, SectorData>& data);
    };

    struct HitNtuple {
        void initForWrite(TTree& tree);

        void initForRead(TTree& tree);
        void initForReadseg(TTree& tree);

        void fill(std::vector<MuonDebugInfo>& muons);
        void fill(MuonDebugInfo& muon);

        void fill(const Hit& hit);
        void fill(const HitVec& hits);

        void fill(const PhiHit& hit);
        void fill(const PhiHitVec& hits);

        void fill(int sTgcPadAssociated, int sTgcPadNotAssociated);

        bool read(EventData& event, std::vector<MuonDebugInfo>& muons);
        bool readseg(std::vector<SegDebugInfo>& segments);

        void reset();
        void init();

        static const int HITSIZE = 50000;
        // hit info
        int nhits;
        int lay[HITSIZE];
        float x[HITSIZE];
        float ymin[HITSIZE];
        float ymax[HITSIZE];
        float w[HITSIZE];

        // extra hit info
        int ndebug;
        int type[HITSIZE];
        int region[HITSIZE];
        int sector[HITSIZE];
        int layer[HITSIZE];
        int sublayer[HITSIZE];
        int pdgId[HITSIZE];
        int barcode[HITSIZE];
        int muonIndex[HITSIZE];
        int clusterSize[HITSIZE];
        int clusterLayers[HITSIZE];
        int isEtaPhi[HITSIZE];
        int trigConfirm[HITSIZE];
        int binpos[HITSIZE];
        int bintheta[HITSIZE];
        float time[HITSIZE];
        float r[HITSIZE];
        float ph[HITSIZE];
        float phn[HITSIZE];
        float ph1[HITSIZE];
        float ph2[HITSIZE];
        float rot[HITSIZE];

        static const int PHIHITSIZE = 1000;
        // hit info
        int pnhits;
        int play[PHIHITSIZE];
        float pr[PHIHITSIZE];
        float phimin[PHIHITSIZE];
        float phimax[PHIHITSIZE];
        float pw[PHIHITSIZE];

        // extra hit info
        int pndebug;
        int ptype[PHIHITSIZE];
        int pregion[PHIHITSIZE];
        int psector[PHIHITSIZE];
        int player[PHIHITSIZE];
        int psublayer[PHIHITSIZE];
        int ppdgId[PHIHITSIZE];
        int pbarcode[PHIHITSIZE];
        int pmuonIndex[PHIHITSIZE];
        int pclusterSize[PHIHITSIZE];
        int pclusterLayers[PHIHITSIZE];
        int pisEtaPhi[PHIHITSIZE];
        int pbinpos[PHIHITSIZE];
        int pbintheta[PHIHITSIZE];
        float ptime[PHIHITSIZE];
        float ppr[PHIHITSIZE];
        float pph[PHIHITSIZE];
        float pph1[PHIHITSIZE];
        float pph2[PHIHITSIZE];
        float prot[PHIHITSIZE];

        static const int ETAMAXSIZE = 100;
        int netamaxima;
        int nsTgcPadAssociated[ETAMAXSIZE];
        int nsTgcPadNotAssociated[ETAMAXSIZE];

        static const int MUONSIZE = 50;
        // muon/truth info
        int nmuons;
        int tpdgId[MUONSIZE];
        int tbarcode[MUONSIZE];
        int tmuonIndex[MUONSIZE];
        float pt[MUONSIZE];
        float eta[MUONSIZE];
        float phi[MUONSIZE];
        int nmdts[MUONSIZE];
        int nrpcs[MUONSIZE];
        int ntgcs[MUONSIZE];
        int ncscs[MUONSIZE];
        int ntmdts[MUONSIZE];
        int ntrpcs[MUONSIZE];
        int nttgcs[MUONSIZE];
        int ntcscs[MUONSIZE];

        // muon truth seg info
        static const int SEGSIZE = 150;
        int nsegs;
        int sbarcode[SEGSIZE];
        float sposx[SEGSIZE];
        float sposy[SEGSIZE];
        float sposz[SEGSIZE];
        float sdirx[SEGSIZE];
        float sdiry[SEGSIZE];
        float sdirz[SEGSIZE];
        int snPrecHits[SEGSIZE];
        int snTrigHits[SEGSIZE];
        int sSector[SEGSIZE];
        int sChIndex[SEGSIZE];
    };
}  // namespace MuonHough

#endif
