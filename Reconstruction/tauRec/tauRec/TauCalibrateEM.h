/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUREC_TAUCALIBRATEEM_H
#define	TAUREC_TAUCALIBRATEEM_H

#include <string>
#include <tauRec/TauToolBase.h>

class TF1;

/** implementation of tau EM energy scale (depreciated) */
class TauCalibrateEM : public TauToolBase {
public:
    TauCalibrateEM(const std::string& type, const std::string& name, const IInterface* parent);
    ~TauCalibrateEM();

    virtual StatusCode initialize();
    virtual StatusCode execute(TauCandidateData *data);
    virtual StatusCode finalize();

private:
    // private methods
    double evaluate_new_pt(double pt, double eta, int ntrack, double emfrac);

    // configurables
    std::string m_response_functions_file;

    // private data
    TF1* m_f1_1p_lem;
    TF1* m_f1_1p_hem_barrel;
    TF1* m_f1_1p_hem_crack;
    TF1* m_f1_1p_hem_endcap;
    TF1* m_f1_mp_barrel;
    TF1* m_f1_mp_crack;
    TF1* m_f1_mp_endcap;

    std::pair<double, double> m_min_1p_lem;
    std::pair<double, double> m_min_1p_hem_barrel;
    std::pair<double, double> m_min_1p_hem_crack;
    std::pair<double, double> m_min_1p_hem_endcap;
    std::pair<double, double> m_min_mp_barrel;
    std::pair<double, double> m_min_mp_crack;
    std::pair<double, double> m_min_mp_endcap;

};

#endif	/* TAUCALIBRATEEM_H */

