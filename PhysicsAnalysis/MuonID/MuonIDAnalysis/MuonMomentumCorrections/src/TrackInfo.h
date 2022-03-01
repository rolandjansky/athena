/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrackInfo_h__
#define __TrackInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TLorentzVector.h>
#include <TTree.h>

//::: Framework includes
#include "GaudiKernel/SystemOfUnits.h"

//::: EDM includes
#include <xAODTracking/TrackParticlexAODHelpers.h>

namespace MMCTest {
    class TrackInfo {
    private:
        std::string m_Type;
        std::vector<std::string> m_Systs;
        float m_Pt, m_Eta, m_Phi, m_QoverP, m_Chi2;
        std::map<std::string, float> m_CalibPt;
        int m_Charge, m_NDoF;
        std::vector<float> m_TrackPars;
        std::vector<float> m_TrackCovMatrix;

    public:
        TrackInfo(std::string type, std::vector<std::string> systs);
        ~TrackInfo() {}

        void Reset();
        void Register(TTree* t);
        void Fill(const xAOD::TrackParticle* tp);
        TLorentzVector GetFourMomentum(bool calib = false);
        int GetCharge() { return m_Charge; }
        void SetCalibPt(float calib_pt, std::string syst) { m_CalibPt[syst] = calib_pt * Gaudi::Units::perThousand; }
    };
}  // namespace MMCTest
#endif
