/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonLayerHough/HitNtuple.h"

#include <TTree.h>

#include <algorithm>
#include <iostream>

namespace MuonHough {

    void EventData::sort() { sort(sectors); }

    void EventData::sort(std::map<int, SectorData>& data) {
        std::map<int, SectorData>::iterator it = data.begin();
        std::map<int, SectorData>::iterator it_end = data.end();
        for (; it != it_end; ++it) {
            SectorData::iterator sit = it->second.begin();
            SectorData::iterator sit_end = it->second.end();
            for (; sit != sit_end; ++sit) {
                HitVec& list = sit->second;
                std::stable_sort(list.begin(), list.end(), SortHitsPerLayer());
            }
        }
    }

    bool operator<(const DataIndex& index1, const DataIndex& index2) { return index1.m_data < index2.m_data; }

    void HitNtuple::initForWrite(TTree& tree) {
        init();
        tree.Branch("nhits", &nhits, "nhits/I");
        tree.Branch("lay", &lay, "lay[nhits]/I");
        tree.Branch("x", &x, "x[nhits]/F");
        tree.Branch("ymin", &ymin, "ymin[nhits]/F");
        tree.Branch("ymax", &ymax, "ymax[nhits]/F");
        tree.Branch("w", &w, "w[nhits]/F");

        // extra hit info
        tree.Branch("ndebug", &ndebug, "ndebug/I");
        tree.Branch("type", &type, "type[ndebug]/I");
        tree.Branch("region", &region, "region[ndebug]/I");
        tree.Branch("sector", &sector, "sector[ndebug]/I");
        tree.Branch("layer", &layer, "layer[ndebug]/I");
        tree.Branch("sublayer", &sublayer, "sublayer[ndebug]/I");
        tree.Branch("pdgId", &pdgId, "pdgId[ndebug]/I");
        tree.Branch("barcode", &barcode, "barcode[ndebug]/I");
        tree.Branch("muonIndex", &muonIndex, "muonIndex[ndebug]/I");
        tree.Branch("clusterSize", &clusterSize, "clusterSize[ndebug]/I");
        tree.Branch("clusterLayers", &clusterLayers, "clusterLayers[ndebug]/I");
        tree.Branch("isEtaPhi", &isEtaPhi, "isEtaPhi[ndebug]/I");
        tree.Branch("trigConfirm", &trigConfirm, "trigConfirm[ndebug]/I");
        tree.Branch("binpos", &binpos, "binpos[ndebug]/I");
        tree.Branch("bintheta", &bintheta, "bintheta[ndebug]/I");
        tree.Branch("time", &time, "time[ndebug]/F");
        tree.Branch("r", &r, "r[ndebug]/F");
        tree.Branch("ph", &ph, "ph[ndebug]/F");
        tree.Branch("phn", &phn, "phn[ndebug]/F");
        tree.Branch("ph1", &ph1, "ph1[ndebug]/F");
        tree.Branch("ph2", &ph2, "ph2[ndebug]/F");
        tree.Branch("rot", &rot, "rot[ndebug]/F");

        tree.Branch("pnhits", &pnhits, "pnhits/I");
        tree.Branch("play", &play, "play[pnhits]/I");
        tree.Branch("pr", &pr, "pr[pnhits]/F");
        tree.Branch("phimin", &phimin, "phimin[pnhits]/F");
        tree.Branch("phimax", &phimax, "phimax[pnhits]/F");
        tree.Branch("pw", &pw, "pw[pnhits]/F");

        // extra hit info
        tree.Branch("pndebug", &pndebug, "pndebug/I");
        tree.Branch("ptype", &ptype, "ptype[pndebug]/I");
        tree.Branch("pregion", &pregion, "pregion[pndebug]/I");
        tree.Branch("psector", &psector, "psector[pndebug]/I");
        tree.Branch("player", &player, "player[pndebug]/I");
        tree.Branch("psublayer", &psublayer, "psublayer[pndebug]/I");
        tree.Branch("ppdgId", &ppdgId, "ppdgId[pndebug]/I");
        tree.Branch("pbarcode", &pbarcode, "pbarcode[pndebug]/I");
        tree.Branch("pmuonIndex", &pmuonIndex, "pmuonIndex[pndebug]/I");
        tree.Branch("pclusterSize", &pclusterSize, "pclusterSize[pndebug]/I");
        tree.Branch("pclusterLayers", &pclusterLayers, "pclusterLayers[pndebug]/I");
        tree.Branch("pisEtaPhi", &pisEtaPhi, "pisEtaPhi[pndebug]/I");
        tree.Branch("pbinpos", &pbinpos, "pbinpos[pndebug]/I");
        tree.Branch("pbintheta", &pbintheta, "pbintheta[pndebug]/I");
        tree.Branch("ptime", &ptime, "ptime[pndebug]/F");
        tree.Branch("ppr", &ppr, "ppr[pndebug]/F");
        tree.Branch("pph", &pph, "pph[pndebug]/F");
        tree.Branch("pph1", &pph1, "pph1[pndebug]/F");
        tree.Branch("pph2", &pph2, "pph2[pndebug]/F");
        tree.Branch("prot", &prot, "prot[pndebug]/F");

        tree.Branch("netamaxima", &netamaxima, "netamaxima/I");
        tree.Branch("sTgcPadAssociated", &nsTgcPadAssociated, "nsTgcPadAssociated[netamaxima]/I");
        tree.Branch("sTgcPadNotAssociated", &nsTgcPadNotAssociated, "nsTgcPadNotAssociated[netamaxima]/I");

        tree.Branch("nmuons", &nmuons, "nmuons/I");
        tree.Branch("tpdgId", &tpdgId, "tpdgId[nmuons]/I");
        tree.Branch("tbarcode", &tbarcode, "tbarcode[nmuons]/I");
        tree.Branch("tmuonIndex", &tmuonIndex, "tmuonIndex[nmuons]/I");
        tree.Branch("pt", &pt, "pt[nmuons]/F");
        tree.Branch("eta", &eta, "eta[nmuons]/F");
        tree.Branch("phi", &phi, "phi[nmuons]/F");
        tree.Branch("nmdts", &nmdts, "nmdts[nmuons]/I");
        tree.Branch("nrpcs", &nrpcs, "nrpcs[nmuons]/I");
        tree.Branch("ntgcs", &ntgcs, "ntgcs[nmuons]/I");
        tree.Branch("ncscs", &ncscs, "ncscs[nmuons]/I");
        tree.Branch("ntmdts", &ntmdts, "ntmdts[nmuons]/I");
        tree.Branch("ntrpcs", &ntrpcs, "ntrpcs[nmuons]/I");
        tree.Branch("nttgcs", &nttgcs, "nttgcs[nmuons]/I");
        tree.Branch("ntcscs", &ntcscs, "ntcscs[nmuons]/I");

        tree.Branch("nsegs", &nsegs, "nsegs/I");
        tree.Branch("sbarcode", &sbarcode, "sbarcode[nsegs]/I");
        tree.Branch("sposx", &sposx, "sposx[nsegs]/F");
        tree.Branch("sposy", &sposy, "sposy[nsegs]/F");
        tree.Branch("sposz", &sposz, "sposz[nsegs]/F");
        tree.Branch("sdirx", &sdirx, "sdirx[nsegs]/F");
        tree.Branch("sdiry", &sdiry, "sdiry[nsegs]/F");
        tree.Branch("sdirz", &sdirz, "sdirz[nsegs]/F");
        tree.Branch("snPrecHits", &snPrecHits, "snPrecHits[nsegs]/I");
        tree.Branch("snTrigHits", &snTrigHits, "snTrigHits[nsegs]/I");
        tree.Branch("sSector", &sSector, "sSector[nsegs]/I");
        tree.Branch("sChIndex", &sChIndex, "sChIndex[nsegs]/I");
    }

    void HitNtuple::initForRead(TTree& tree) {
        tree.SetBranchAddress("nhits", &nhits);
        tree.SetBranchAddress("lay", &lay);
        tree.SetBranchAddress("x", &x);
        tree.SetBranchAddress("ymin", &ymin);
        tree.SetBranchAddress("ymax", &ymax);
        tree.SetBranchAddress("w", &w);

        // extra hit info
        tree.SetBranchAddress("ndebug", &ndebug);
        tree.SetBranchAddress("type", &type);
        tree.SetBranchAddress("region", &region);
        tree.SetBranchAddress("sector", &sector);
        tree.SetBranchAddress("layer", &layer);
        tree.SetBranchAddress("sublayer", &sublayer);
        tree.SetBranchAddress("pdgId", &pdgId);
        tree.SetBranchAddress("barcode", &barcode);
        tree.SetBranchAddress("muonIndex", &muonIndex);
        tree.SetBranchAddress("clusterSize", &clusterSize);
        tree.SetBranchAddress("clusterLayers", &clusterLayers);
        tree.SetBranchAddress("isEtaPhi", &isEtaPhi);
        tree.SetBranchAddress("trigConfirm", &trigConfirm);
        tree.SetBranchAddress("binpos", &binpos);
        tree.SetBranchAddress("bintheta", &bintheta);
        tree.SetBranchAddress("time", &time);
        tree.SetBranchAddress("r", &r);
        tree.SetBranchAddress("ph", &ph);
        tree.SetBranchAddress("phn", &phn);
        tree.SetBranchAddress("ph1", &ph1);
        tree.SetBranchAddress("ph2", &ph2);
        tree.SetBranchAddress("rot", &rot);

        tree.SetBranchAddress("netamaxima", &netamaxima);
        tree.SetBranchAddress("sTgcPadAssociated", &nsTgcPadAssociated);
        tree.SetBranchAddress("sTgcPadNotAssociated", &nsTgcPadNotAssociated);

        tree.SetBranchAddress("nmuons", &nmuons);
        tree.SetBranchAddress("tpdgId", &tpdgId);
        tree.SetBranchAddress("tbarcode", &tbarcode);
        tree.SetBranchAddress("tmuonIndex", &tmuonIndex);
        tree.SetBranchAddress("pt", &pt);
        tree.SetBranchAddress("eta", &eta);
        tree.SetBranchAddress("phi", &phi);
        tree.SetBranchAddress("nmdts", &nmdts);
        tree.SetBranchAddress("nrpcs", &nrpcs);
        tree.SetBranchAddress("ntgcs", &ntgcs);
        tree.SetBranchAddress("ncscs", &ncscs);
        tree.SetBranchAddress("ntmdts", &ntmdts);
        tree.SetBranchAddress("ntrpcs", &ntrpcs);
        tree.SetBranchAddress("nttgcs", &nttgcs);
        tree.SetBranchAddress("ntcscs", &ntcscs);
    }

    void HitNtuple::initForReadseg(TTree& tree) {
        tree.SetBranchAddress("nsegs", &nsegs);
        tree.SetBranchAddress("sbarcode", &sbarcode);
        tree.SetBranchAddress("sposx", &sposx);
        tree.SetBranchAddress("sposy", &sposy);
        tree.SetBranchAddress("sposz", &sposz);
        tree.SetBranchAddress("sdirx", &sdirx);
        tree.SetBranchAddress("sdiry", &sdiry);
        tree.SetBranchAddress("sdirz", &sdirz);
        tree.SetBranchAddress("snPrecHits", &snPrecHits);
        tree.SetBranchAddress("snTrigHits", &snTrigHits);
        tree.SetBranchAddress("sSector", &sSector);
        tree.SetBranchAddress("sChIndex", &sChIndex);
    }

    void HitNtuple::fill(int sTgcPadAssociated, int sTgcPadNotAssociated) {
        if (netamaxima >= ETAMAXSIZE) return;

        nsTgcPadAssociated[netamaxima] = sTgcPadAssociated;
        nsTgcPadNotAssociated[netamaxima] = sTgcPadNotAssociated;
        ++netamaxima;
    }

    void HitNtuple::fill(std::vector<MuonDebugInfo>& muons) {
        for (std::vector<MuonDebugInfo>::iterator it = muons.begin(); it != muons.end(); ++it) fill(*it);
    }

    void HitNtuple::fill(MuonDebugInfo& muon) {
        if (nmuons >= MUONSIZE) return;

        tpdgId[nmuons] = muon.pdgId;
        tbarcode[nmuons] = muon.barcode;
        tmuonIndex[nmuons] = muon.muonIndex;
        pt[nmuons] = muon.pt;
        eta[nmuons] = muon.eta;
        phi[nmuons] = muon.phi;
        nmdts[nmuons] = muon.nmdts;
        nrpcs[nmuons] = muon.nrpcs;
        ntgcs[nmuons] = muon.ntgcs;
        ncscs[nmuons] = muon.ncscs;
        ntmdts[nmuons] = muon.ntmdts;
        ntrpcs[nmuons] = muon.ntrpcs;
        nttgcs[nmuons] = muon.nttgcs;
        ntcscs[nmuons] = muon.ntcscs;
        ++nmuons;
    }

    void HitNtuple::fill(const Hit& hit) {
        if (nhits >= HITSIZE) return;

        lay[nhits] = hit.layer;
        x[nhits] = hit.x;
        ymin[nhits] = hit.ymin;
        ymax[nhits] = hit.ymax;
        w[nhits] = hit.w;

        if (hit.debugInfo() && ndebug < HITSIZE) {
            const HitDebugInfo* debug = hit.debugInfo();
            type[ndebug] = debug->type;
            region[ndebug] = debug->region;
            sector[ndebug] = debug->sector;
            layer[ndebug] = debug->layer;
            sublayer[ndebug] = debug->sublayer;
            pdgId[ndebug] = debug->pdgId;
            barcode[ndebug] = debug->barcode;
            muonIndex[ndebug] = debug->muonIndex;
            clusterSize[ndebug] = debug->clusterSize;
            clusterLayers[ndebug] = debug->clusterLayers;
            isEtaPhi[ndebug] = debug->isEtaPhi;
            trigConfirm[ndebug] = debug->trigConfirm;
            time[ndebug] = debug->time;
            binpos[ndebug] = debug->binpos;
            bintheta[ndebug] = debug->bintheta;
            r[ndebug] = debug->r;
            rot[ndebug] = debug->rot;
            ph[ndebug] = debug->ph;
            phn[ndebug] = debug->phn;
            ph1[ndebug] = debug->ph1;
            ph2[ndebug] = debug->ph2;
            ++ndebug;
        }
        ++nhits;
    }
    void HitNtuple::fill(const PhiHit& hit) {
        if (pnhits >= PHIHITSIZE) return;

        play[pnhits] = hit.layer;
        pr[pnhits] = hit.r;
        phimin[pnhits] = hit.phimin;
        phimax[pnhits] = hit.phimax;
        pw[pnhits] = hit.w;

        if (hit.debugInfo() && pndebug < PHIHITSIZE) {
            const HitDebugInfo* debug = hit.debugInfo();
            ptype[pndebug] = debug->type;
            pregion[pndebug] = debug->region;
            psector[pndebug] = debug->sector;
            player[pndebug] = debug->layer;
            psublayer[pndebug] = debug->sublayer;
            ppdgId[pndebug] = debug->pdgId;
            pbarcode[pndebug] = debug->barcode;
            pmuonIndex[pndebug] = debug->muonIndex;
            pclusterSize[pndebug] = debug->clusterSize;
            pclusterLayers[pndebug] = debug->clusterLayers;
            ptime[pndebug] = debug->time;
            pbinpos[pndebug] = debug->binpos;
            pbintheta[pndebug] = debug->bintheta;
            ppr[pndebug] = debug->r;
            prot[pndebug] = debug->rot;
            pph[pndebug] = debug->ph;
            pph1[pndebug] = debug->ph1;
            pph2[pndebug] = debug->ph2;
            ++pndebug;
        }
        ++pnhits;
    }

    void HitNtuple::reset() {
        nmuons = 0;
        nsegs = 0;
        nhits = 0;
        ndebug = 0;
        pnhits = 0;
        pndebug = 0;
    }

    void HitNtuple::fill(const HitVec& hits) {
        for (unsigned int i = 0; i < hits.size(); ++i) { fill(*hits[i]); }
    }

    void HitNtuple::fill(const PhiHitVec& hits) {
        for (unsigned int i = 0; i < hits.size(); ++i) { fill(*hits[i]); }
    }

    bool HitNtuple::read(EventData& event, std::vector<MuonDebugInfo>& muons) {
        if (nhits == 0) {
            std::cout << " ntuple not initialized for reading " << std::endl;
            return false;
        }
        bool hasDebug = nhits == ndebug;

        for (int i = 0; i < nhits; ++i) {
            HitDebugInfo* debug = nullptr;
            if (hasDebug) {
                debug = new HitDebugInfo();
                debug->type = type[i];
                debug->region = static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region[i]);
                debug->sector = sector[i];
                debug->layer = static_cast<Muon::MuonStationIndex::LayerIndex>(layer[i]);
                debug->sublayer = sublayer[i];
                debug->pdgId = pdgId[i];
                debug->barcode = barcode[i];
                debug->muonIndex = muonIndex[i];
                debug->clusterSize = clusterSize[i];
                debug->clusterLayers = clusterLayers[i];
                debug->isEtaPhi = isEtaPhi[i];
                debug->trigConfirm = trigConfirm[i];
                debug->binpos = binpos[i];
                debug->bintheta = bintheta[i];
                debug->time = time[i];
                debug->r = r[i];
                debug->ph = ph[i];
                debug->phn = phn[i];
                debug->ph1 = ph1[i];
                debug->ph2 = ph2[i];
                debug->rot = rot[i];
            }
            std::shared_ptr<Hit> hit = std::make_shared<Hit>(lay[i], x[i], ymin[i], ymax[i], w[i], debug);

            DataIndex index(sector[i], static_cast<Muon::MuonStationIndex::DetectorRegionIndex>(region[i]),
                            static_cast<Muon::MuonStationIndex::LayerIndex>(layer[i]), type[i]);
            HitVec& HitVec = event.sectors[sector[i]][index];
            HitVec.push_back(hit);
        }

        for (int i = 0; i < nmuons; ++i) {
            MuonDebugInfo muon{};
            muon.pdgId = tpdgId[i];
            muon.barcode = tbarcode[i];
            muon.muonIndex = tmuonIndex[i];
            muon.pt = pt[i];
            muon.eta = eta[i];
            muon.phi = phi[i];
            muon.nmdts = nmdts[i];
            muon.nrpcs = nrpcs[i];
            muon.ntgcs = ntgcs[i];
            muon.ncscs = ncscs[i];
            muon.ntmdts = ntmdts[i];
            muon.ntrpcs = ntrpcs[i];
            muon.nttgcs = nttgcs[i];
            muon.ntcscs = ntcscs[i];
            muons.push_back(muon);
        }
        return true;
    }

    bool HitNtuple::readseg(std::vector<SegDebugInfo>& segments) {
        for (int i = 0; i < nsegs; ++i) {
            SegDebugInfo segment{};
            segment.sposx = sposx[i];
            segment.sposy = sposy[i];
            segment.sposz = sposz[i];
            segment.sdirx = sdirx[i];
            segment.sdiry = sdiry[i];
            segment.sdirz = sdirz[i];
            segment.snPrecHits = snPrecHits[i];
            segment.snTrigHits = snTrigHits[i];
            segment.sSector = sSector[i];
            segment.sChIndex = sChIndex[i];
            segments.push_back(segment);
        }
        return true;
    }

    void EventData::dump(const std::map<int, SectorData>& data) {
        std::cout << " dumping sectors " << data.size() << std::endl;
        std::map<int, SectorData>::const_iterator it = data.begin();
        std::map<int, SectorData>::const_iterator it_end = data.end();
        for (; it != it_end; ++it) {
            SectorData::const_iterator sit = it->second.begin();
            SectorData::const_iterator sit_end = it->second.end();
            for (; sit != sit_end; ++sit) {
                std::cout << " sector " << sit->first.sector() << " region " << sit->first.region() << " layer " << sit->first.layer()
                          << " type " << sit->first.type() << " hits  " << sit->second.size() << std::endl;
            }
        }
    }

    void EventData::dump() const {
        if (!sectors.empty()) {
            std::cout << " endcapC sectors with hits " << sectors.size() << std::endl;
            dump(sectors);
        }
    }

    void HitNtuple::init() {
        nmuons = 0;
        nsegs = 0;
        nhits = 0;
        ndebug = 0;
    }
}  // namespace MuonHough
