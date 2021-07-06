/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class handling the output of the MissingMassCalculator
// author Michael Huebner <michael.huebner@no.spam.cern.ch>

#ifndef DITAUMASSTOOLS_MISSINGMASSOUTPUT_H
#define DITAUMASSTOOLS_MISSINGMASSOUTPUT_H

// local include(s):
#include "DiTauMassTools/HelperFunctions.h"

// ROOT include(s):
#include <TH1.h>

namespace DiTauMassTools{

class MissingMassCalculatorV2;

class MissingMassOutput {

  public:
    MissingMassOutput();
    ~MissingMassOutput();

    int GetFitStatus() const; // return fit status
    double GetFittedMass(int fitcode) const; // returns fitted Mass
    double GetFittedMassErrorUp(int fitcode) const; // returns upper error on fitted Mass
    double GetFittedMassErrorLow(int fitcode) const; // returns lower error on fitted Mass
    std::shared_ptr<TH1F> GetMassHistogram() const; // return mass histogram
    std::shared_ptr<TH1F> GetMassHistogramNoWeight() const; // return mass histogram without weights
    int GetNTrials() const;// total number of point scanned
    int GetNSuccesses() const;// total number of point with at least 1 solutions
    int GetNSolutions() const;// total number of solutions
    double GetSumW() const; // sum of weights
    double GetAveSolRMS() const; // ave RMS of solutions (for one event)

    double GetRms2Mpv() const; // returns RMS/MPV according to histogram method
    TLorentzVector GetNeutrino4vec(int fitcode, int ind) const; // returns neutrino 4-vec
    double GetFitSignificance(int fitcode) const; // returns fit significance
    TLorentzVector GetTau4vec(int fitcode, int ind) const; // returns full tau 4-vec
    TLorentzVector GetResonanceVec(int fitcode) const; // returns 4-vec for resonance
    TVector2 GetFittedMetVec(int fitcode) const; // returns 2-vec for fitted MET

    friend class MissingMassCalculatorV2;

  private:
    void ClearOutput(bool fUseVerbose);
    int m_FitStatus;
    double m_FitSignificance[MMCFitMethodV2::MAX];
    double m_FittedMass[MMCFitMethodV2::MAX];
    double m_FittedMassUpperError[MMCFitMethodV2::MAX];
    double m_FittedMassLowerError[MMCFitMethodV2::MAX];
    TLorentzVector m_nuvec1[MMCFitMethodV2::MAX];
    TLorentzVector m_objvec1[MMCFitMethodV2::MAX];
    TLorentzVector m_nuvec2[MMCFitMethodV2::MAX];
    TLorentzVector m_objvec2[MMCFitMethodV2::MAX];
    TLorentzVector m_totalvec[MMCFitMethodV2::MAX];
    TVector2 m_FittedMetVec[MMCFitMethodV2::MAX];
    double m_RMS2MPV;
    std::shared_ptr<TH1F> m_hMfit_all;
    std::shared_ptr<TH1F> m_hMfit_allNoWeight;
    int m_NTrials;
    int m_NSuccesses;
    int m_NSolutions;
    double m_SumW;
    double m_AveSolRMS;
};
} // namespace DiTauMassTools

#endif
