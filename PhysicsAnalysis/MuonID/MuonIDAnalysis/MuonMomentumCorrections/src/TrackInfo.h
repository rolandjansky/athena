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
        float m_Pt = 0.0F, m_Eta = 0.0F, m_Phi = 0.0F, m_QoverP = 0.0F, m_Chi2 = 0.0F;
        std::map<std::string, float> m_CalibPt;
        int m_Charge = 0, m_NDoF = 0;
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
