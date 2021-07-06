/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAlignmentData/BLinePar.h"

BLinePar::BLinePar() :
    MuonAlignmentPar(), m_bz(0.), m_bp(0.), m_bn(0.), m_sp(0.), m_sn(0.), m_tw(0.), m_pg(0.), m_tr(0.), m_eg(0.), m_ep(0.), m_en(0.) {}

void BLinePar::setParameters(float bz, float bp, float bn, float sp, float sn, float tw, float pg, float tr, float eg, float ep, float en) {
    m_bz = bz;
    m_bp = bp;
    m_bn = bn;
    m_sp = sp;
    m_sn = sn;
    m_tw = tw;
    m_pg = pg;
    m_tr = tr;
    m_eg = eg;
    m_ep = ep;
    m_en = en;
}

void BLinePar::getParameters(float& bz, float& bp, float& bn, float& sp, float& sn, float& tw, float& pg, float& tr, float& eg, float& ep,
                             float& en) const {
    bz = m_bz;
    bp = m_bp;
    bn = m_bn;
    sp = m_sp;
    sn = m_sn;
    tw = m_tw;
    pg = m_pg;
    tr = m_tr;
    eg = m_eg;
    ep = m_ep;
    en = m_en;
}
