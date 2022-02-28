/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "MuonLayerHough/HitNtuple.h"

namespace MuonHough {

    HitDebugInfo::HitDebugInfo(int type_, int sector_, Muon::MuonStationIndex::DetectorRegionIndex region_,
                               Muon::MuonStationIndex::LayerIndex layer_, int sublayer_) :
        type(type_), sector(sector_), region(region_), layer(layer_), sublayer(sublayer_) {}

    Hit::Hit(int layer_, float x_, float ymin_, float ymax_, float w_, HitDebugInfo* d_, const Trk::PrepRawData* prd_,
             const Muon::TgcClusterObj3D* tgc_) :
        layer(layer_), x(x_), ymin(ymin_), ymax(ymax_), w(w_), prd(prd_), tgc(tgc_), m_debug(d_) {}

    Hit::~Hit() = default;

    Hit::Hit(const Hit& h_) { copy(h_); }

    Hit& Hit::operator=(const Hit& h_) {
        if (&h_ != this) {          
            copy(h_);
        }
        return *this;
    }

    void Hit::copy(const Hit& hit) {
        layer = hit.layer;
        x = hit.x;
        ymin = hit.ymin;
        ymax = hit.ymax;
        w = hit.w;
        if (hit.m_debug)
            m_debug = std::make_unique< HitDebugInfo>(*hit.m_debug);
        else
            m_debug.reset();
        prd = hit.prd;
        tgc = hit.tgc;
    }

    PhiHit::PhiHit(int layer_, float r_, float phimin_, float phimax_, float w_, HitDebugInfo* d_, const Trk::PrepRawData* prd_,
                   const Muon::TgcClusterObj3D* tgc_) :
        layer(layer_), r(r_), phimin(phimin_), phimax(phimax_), w(w_), prd(prd_), tgc(tgc_), m_debug(d_) {}

    PhiHit::~PhiHit() = default;

    PhiHit::PhiHit(const PhiHit& h_) { copy(h_); }

    PhiHit& PhiHit::operator=(const PhiHit& h_) {
        if (&h_ != this) {
            copy(h_);
        }
        return *this;
    }

    void PhiHit::copy(const PhiHit& hit) {
        layer = hit.layer;
        r = hit.r;
        phimin = hit.phimin;
        phimax = hit.phimax;
        w = hit.w;
        if (hit.m_debug)
            m_debug = std::make_unique< HitDebugInfo>(*hit.m_debug);
        else
            m_debug.reset();
        prd = hit.prd;
        tgc = hit.tgc;
    }
}  // namespace MuonHough
