/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibMUCTPI.h"

#include <iostream>

namespace MuonCalib {

    int MuonCalibMUCTPI::roiWord() const { return m_params.roiWord; }
    int MuonCalibMUCTPI::bcID() const { return m_params.bcID; }
    int MuonCalibMUCTPI::sysID() const { return m_params.sysID; }
    int MuonCalibMUCTPI::subsysID() const { return m_params.subsysID; }
    int MuonCalibMUCTPI::sectorID() const { return m_params.sectorID; }
    int MuonCalibMUCTPI::thresholdNumber() const { return m_params.thresholdNumber; }
    int MuonCalibMUCTPI::thresholdValue() const { return m_params.thresholdValue; }
    int MuonCalibMUCTPI::roINumber() const { return m_params.roINumber; }
    int MuonCalibMUCTPI::overlap() const { return m_params.overlap; }
    int MuonCalibMUCTPI::firstCandidate() const { return m_params.firstCandidate; }
    int MuonCalibMUCTPI::sectorOverflow() const { return m_params.sectorOverflow; }
    int MuonCalibMUCTPI::padOverflow() const { return m_params.padOverflow; }
    float MuonCalibMUCTPI::phi() const { return m_params.phi; }
    float MuonCalibMUCTPI::eta() const { return m_params.eta; }

    MuonCalibMUCTPI::MuonCalibMUCTPI(MuonCalibMUCTPI::defineParams pars) : m_params{pars} {}

}  // namespace MuonCalib
