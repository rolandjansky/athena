#ifndef __TrackInfo_h__
#define __TrackInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TLorentzVector.h>
#include <TTree.h>

//::: Framework includes
#include <CLHEP/Units/SystemOfUnits.h>
//::: EDM includes
#include <xAODTracking/TrackParticlexAODHelpers.h>

class TrackInfo {
  private:
    std::string m_Type;
    float m_Pt, m_CalibPt, m_Eta, m_Phi, m_QoverP, m_Chi2;
    int m_Charge, m_NDoF;
    std::vector<float> TrackPars;
    std::vector<float> TrackCovMatrix;

  public:
    TrackInfo(std::string type);
    ~TrackInfo() {}

    void Reset();
    void Register(TTree* t);
    void Fill(const xAOD::TrackParticle* tp);
    TLorentzVector GetFourMomentum(bool calib=false);
    int GetCharge() { return m_Charge; }
    void SetCalibPt(float calib_pt) { m_CalibPt = calib_pt / CLHEP::GeV; }
};

#endif
