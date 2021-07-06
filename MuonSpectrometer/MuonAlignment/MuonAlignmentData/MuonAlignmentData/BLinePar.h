/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNMENTDATA_BLINEPAR_H
#define MUONALIGNMENTDATA_BLINEPAR_H

#include "MuonAlignmentData/MuonAlignmentPar.h"

class BLinePar : public MuonAlignmentPar {
public:
    // Default constructor
    BLinePar();
    // destructor
    virtual ~BLinePar() override = default;

    void setParameters(float bz, float bp, float bn, float sp, float sn, float tw, float pg, float tr, float eg, float ep, float en);

    void getParameters(float& bz, float& bp, float& bn, float& sp, float& sn, float& tw, float& pg, float& tr, float& eg, float& ep,
                       float& en) const;
    float bz() const { return m_bz; }
    float bp() const { return m_bp; }
    float bn() const { return m_bn; }
    float sp() const { return m_sp; }
    float sn() const { return m_sn; }
    float tw() const { return m_tw; }
    float pg() const { return m_pg; }
    float tr() const { return m_tr; }
    float eg() const { return m_eg; }
    float ep() const { return m_ep; }
    float en() const { return m_en; }

private:
    float m_bz;  // tube bow in plane
    float m_bp;  // tube bow out of plane
    float m_bn;  // tube bow out of plane
    float m_sp;  // cross plate sag out of plane
    float m_sn;  // cross plate sag out of plane
    float m_tw;  // twist
    float m_pg;  // parallelogram
    float m_tr;  // trapezoid
    float m_eg;  // global expansion
    float m_ep;  // local expansion
    float m_en;  // local expansion
};

#endif  // MUONALIGNMENTDATA_BLINEPAR_H
