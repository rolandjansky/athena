/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class handling the input of the MissingMassCalculator
// author Michael Huebner <michael.huebner@no.spam.cern.ch>

#ifndef DITAUMASSTOOLS_MISSINGMASSINPUT_H
#define DITAUMASSTOOLS_MISSINGMASSINPUT_H

// EDM include(s):
#include "xAODMissingET/MissingET.h"

// Local include(s):
#include "DiTauMassTools/HelperFunctions.h"

namespace DiTauMassTools{

class MissingMassInput {

  public:
    MissingMassInput();
    ~MissingMassInput();

    void ClearInput();
    void PrintInputInfo();

    void SetBeamEnergy(double val) { m_beamEnergy=val; } // beam energy
    void SetMETresSyst(int val) { m_METresSyst=val; } // MET resolution systematics: +/-1 sigma
    void SetMetScanParamsUE(double sumEt, double phi_scan=0.0, int data_code=0);
    void SetNjet25(int val);
    void SetSumEt(double sumEt);
    void SetVisTauVec(int i, const TLorentzVector & vec);
    void SetVisTauType(int i, int tautype);
    void SetMetVec(const TVector2 & vec);
    void SetNprong(int i, int nprong);
    void SetIsData(int val);
    void SetMHtSigma1(double val) { m_MHtSigma1=val; }
    void SetMHtSigma2(double val) { m_MHtSigma2=val; } 
    void SetMHtGaussFr(double val) { m_MHtGaussFr=val; }
    void SetMETScanScheme(int val) { m_METScanScheme=val; }
    void SetUseDefaults(int val) { m_fUseDefaults=val; }
    void SetUseTailCleanup(bool val) { m_fUseTailCleanup=val; }
    void SetUseVerbose(bool val) { m_fUseVerbose=val; }
    void SetLFVmode(int val) { m_LFVmode=val; }

    friend class MissingMassCalculatorV2;
    friend class MissingMassProb;

  private:
    TVector2 m_MetVec;
    TLorentzVector m_vistau1;
    TLorentzVector m_vistau2;
    int m_type_visTau1; // 0: l, 1:1-prong, 3:3-prong
    int m_type_visTau2;  // 0: l, 1:1-prong, 3:3-prong
    int m_Nprong_tau1;
    int m_Nprong_tau2;
    int m_dataType;
    double m_METcovphi;
    double m_METsigmaP;
    double m_METsigmaL;
    double m_SumEt;
    std::vector<TLorentzVector> m_jet4vecs;
    int m_Njet25;
    double m_DelPhiTT;
    double m_MHtSigma1; // sigma of 1st Gaussian in missing Ht resolution
    double m_MHtSigma2; // sigma of 2nd Gaussian in missing Ht resolution
    double m_MHtGaussFr; // relative fraction of 2nd Gaussian
    double m_HtOffset; // HT offset
    double m_beamEnergy; // beam energy (Tevatron=980, LHC-1=3500.0) 
    int m_InputReorder; // flag for input re-order
    int m_METScanScheme; // MET-scan scheme: 0- use JER; 1- use simple sumEt & missingHt for Njet=0 events in (lep-had) 
    int m_METresSyst; // switch to turn on/off MET resolution systematics
    bool m_fUseDefaults; // switch to control defaults: 1== use defaults, 0== don't use defaults (useful for studies) 
    bool m_fUseTailCleanup; // switch to apply tail clean-up
    bool m_fUseVerbose; // code to turn ON printouts for debugging
    TVector2 m_metVec;
    TLorentzVector m_tlv_tmp;
    double m_inputMEtX,m_inputMEtY,m_inputMEtT;
    double m_MEtX, m_MEtY,m_MEtT;
    double m_htOffset;
    int m_LFVmode; // flag to determine which LFV decay to be reconstructed: 0=H->e+tau(mu) or 1=H->mu+tau(e) 
    TauTypes::e m_tauTypes;
};
} // namespace DiTauMassTools

#endif
