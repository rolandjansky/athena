/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonMomentumCorrections/MuonResolutionAndMomentumScaleFactors.h"

#include <TString.h>  // for Form

#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace Analysis;

//******************************************************************************

//:::::::::::::::::
//:: CONSTRUCTOR ::
//:::::::::::::::::

MuonResolutionAndMomentumScaleFactors::MuonResolutionAndMomentumScaleFactors(const std::string& data_filename) {
    /////////////////////
    // OPEN INPUT FILE //
    /////////////////////

    std::ifstream infile(data_filename.c_str());
    if (infile.fail()) {
        throw std::runtime_error(Form(
            "File: %s, Line: %d\nMuonResolutionAndMomentumScaleFactors::MuonResolutionAndMomentumScaleFactors() - Could not open file %s.",
            __FILE__, __LINE__, data_filename.c_str()));
    }

    ///////////////////
    // READ THE FILE //
    ///////////////////

    double dummy;
    while (!infile.eof()) {
        infile >> dummy;
        if (infile.eof()) { break; }
        m_eta_min.push_back(dummy);
        infile >> dummy;
        m_eta_max.push_back(dummy);

        infile >> dummy;
        m_res_SF_CB.push_back(dummy);
        infile >> dummy;
        m_res_SF_CB_err.push_back(dummy);
        infile >> dummy;
        m_mom_SF_CB.push_back(dummy);
        infile >> dummy;
        m_mom_SF_CB_err.push_back(dummy);

        infile >> dummy;
        m_res_SF_ID.push_back(dummy);
        infile >> dummy;
        m_res_SF_ID_err.push_back(dummy);
        infile >> dummy;
        m_mom_SF_ID.push_back(dummy);
        infile >> dummy;
        m_mom_SF_ID_err.push_back(dummy);

        infile >> dummy;
        m_res_SF_SA.push_back(dummy);
        infile >> dummy;
        m_res_SF_SA_err.push_back(dummy);
        infile >> dummy;
        m_mom_SF_SA.push_back(dummy);
        infile >> dummy;
        m_mom_SF_SA_err.push_back(dummy);
    }
}

//******************************************************************************

//:::::::::::::::::::::::::::::::::::::
//:: METHOD getResolutionScaleFactor ::
//:::::::::::::::::::::::::::::::::::::

double MuonResolutionAndMomentumScaleFactors::getResolutionScaleFactor(const TLorentzVector& p, int type) {
    if (type == 1) { return m_res_SF_CB[getEtaBin(p)]; }
    if (type == 2) { return m_res_SF_ID[getEtaBin(p)]; }

    return m_res_SF_SA[getEtaBin(p)];
}

//******************************************************************************

//::::::::::::::::::::::::::::::::::::::::::
//:: METHOD getResolutionScaleFactorError ::
//::::::::::::::::::::::::::::::::::::::::::

double MuonResolutionAndMomentumScaleFactors::getResolutionScaleFactorError(const TLorentzVector& p, int type) {
    if (type == 1) { return m_res_SF_CB_err[getEtaBin(p)]; }
    if (type == 2) { return m_res_SF_ID_err[getEtaBin(p)]; }

    return m_res_SF_SA_err[getEtaBin(p)];
}

//******************************************************************************

//:::::::::::::::::::::::::::::::::::
//:: METHOD getMomentumScaleFactor ::
//:::::::::::::::::::::::::::::::::::

double MuonResolutionAndMomentumScaleFactors::getMomentumScaleFactor(const TLorentzVector& p, int type) {
    if (type == 1) { return m_mom_SF_CB[getEtaBin(p)]; }
    if (type == 2) { return m_mom_SF_ID[getEtaBin(p)]; }

    return m_mom_SF_SA[getEtaBin(p)];
}

//******************************************************************************

//::::::::::::::::::::::::::::::::::::::::::
//:: METHOD getMomentumScaleFactorError ::
//::::::::::::::::::::::::::::::::::::::::::

double MuonResolutionAndMomentumScaleFactors::getMomentumScaleFactorError(const TLorentzVector& p, int type) {
    if (type == 1) { return m_mom_SF_CB_err[getEtaBin(p)]; }
    if (type == 2) { return m_mom_SF_ID_err[getEtaBin(p)]; }

    return m_mom_SF_SA_err[getEtaBin(p)];
}

//******************************************************************************

//::::::::::::::::::::::
//:: METHOD getEtaBin ::
//::::::::::::::::::::::

unsigned int MuonResolutionAndMomentumScaleFactors::getEtaBin(const TLorentzVector& p) {
    if (p.Eta() < m_eta_min[0]) { return 0; }
    if (p.Eta() >= m_eta_max[m_eta_max.size() - 1]) { return m_eta_max.size() - 1; }

    for (unsigned int k = 0; k < m_eta_min.size(); k++) {
        if (p.Eta() >= m_eta_min[k] && p.Eta() < m_eta_max[k]) { return k; }
    }

    return 0;
}
