/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#define __TrackInfo_cxx__
#include "TrackInfo.h"
namespace MMCTest {
    TrackInfo::TrackInfo(std::string type, std::vector<std::string> systs) {
        m_Type = type;
        m_Systs = systs;
        Reset();
    }

    void TrackInfo::Reset() {
        m_Pt = -999.;
        for (auto s : m_Systs) { m_CalibPt[s] = -999.; }
        m_Eta = -999.;
        m_Phi = -999.;
        m_QoverP = -999.;
        m_Chi2 = -999.;
        m_Charge = -999;
        m_NDoF = 0;
        m_TrackPars.clear();
        for (unsigned int i = 0; i < 5; i++) m_TrackPars.push_back(0.);
        m_TrackCovMatrix.clear();
        for (unsigned int i = 0; i < 5; i++)
            for (unsigned int j = 0; j < 5; j++) m_TrackCovMatrix.push_back(0.);
    }

    void TrackInfo::Register(TTree* t) {
        t->Branch(("Muon_" + m_Type + "_Pt").c_str(), &m_Pt);
        t->Branch(("Muon_" + m_Type + "_Eta").c_str(), &m_Eta);
        t->Branch(("Muon_" + m_Type + "_Phi").c_str(), &m_Phi);
        t->Branch(("Muon_" + m_Type + "_Charge").c_str(), &m_Charge);
        t->Branch(("Muon_" + m_Type + "_QoverP").c_str(), &m_QoverP);
        t->Branch(("Muon_" + m_Type + "_Chi2").c_str(), &m_Chi2);
        t->Branch(("Muon_" + m_Type + "_NDoF").c_str(), &m_NDoF);
        t->Branch(("Muon_" + m_Type + "_TrackPars").c_str(), &m_TrackPars);
        t->Branch(("Muon_" + m_Type + "_TrackCovMatrix").c_str(), &m_TrackCovMatrix);
        for (auto s : m_Systs) { t->Branch(("Muon_" + m_Type + "_" + s + "CalibPt").c_str(), &m_CalibPt[s]); }
    }

    void TrackInfo::Fill(const xAOD::TrackParticle* tp) {
        m_Pt = tp->pt() * Gaudi::Units::perThousand;
        m_Eta = tp->eta();
        m_Phi = tp->phi();
        m_Charge = (int)tp->charge();
        m_QoverP = tp->qOverP() / Gaudi::Units::perThousand;
        try {
            m_Chi2 = tp->chiSquared();
        } catch (SG::ExcBadAuxVar& b) { m_Chi2 = -999; }

        try {
            m_NDoF = (int)tp->numberDoF();
        } catch (SG::ExcBadAuxVar& b) { m_NDoF = -999; }
        AmgVector(5) pars = tp->definingParameters();
        AmgSymMatrix(5) cov = tp->definingParametersCovMatrix();

        m_TrackPars.clear();
        for (unsigned int i = 0; i < 5; i++) m_TrackPars.push_back(pars[i]);
        m_TrackCovMatrix.clear();

        for (unsigned int i = 0; i < 5; i++)
            for (unsigned int j = 0; j < 5; j++) m_TrackCovMatrix.push_back(cov(i, j));
    }

    TLorentzVector TrackInfo::GetFourMomentum(bool calib) {
        TLorentzVector result;
        if (calib)
            result.SetPtEtaPhiM(m_CalibPt[""], m_Eta, m_Phi, 0.105658);
        else
            result.SetPtEtaPhiM(m_Pt, m_Eta, m_Phi, 0.105658);
        return result;
    }
}  // namespace MMCTest
