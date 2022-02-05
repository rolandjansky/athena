/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibRpcSectorLogic.h"

#include <iostream>

namespace MuonCalib {

    int MuonCalibRpcSectorLogic::sectorId() const { return m_params.sectorId; }
    int MuonCalibRpcSectorLogic::fel1Id() const { return m_params.fel1Id; }
    int MuonCalibRpcSectorLogic::bcid() const { return m_params.bcid; }
    int MuonCalibRpcSectorLogic::errorCode() const { return m_params.errorCode; }
    int MuonCalibRpcSectorLogic::crc() const { return m_params.crc; }
    int MuonCalibRpcSectorLogic::hasMoreThan2TriggerCand() const { return m_params.hasMoreThan2TriggerCand; }
    int MuonCalibRpcSectorLogic::slIndex() const { return m_params.slIndex; }
    float MuonCalibRpcSectorLogic::triggerRates() const { return m_params.triggerRates; }
    int MuonCalibRpcSectorLogic::counters() const { return m_params.counters; }
    int MuonCalibRpcSectorLogic::slIndex2() const { return m_params.slIndex2; }
    int MuonCalibRpcSectorLogic::rowinBcid() const { return m_params.rowinBcid; }
    int MuonCalibRpcSectorLogic::padid() const { return m_params.padid; }
    int MuonCalibRpcSectorLogic::ptid() const { return m_params.ptid; }
    int MuonCalibRpcSectorLogic::roi() const { return m_params.roi; }
    int MuonCalibRpcSectorLogic::outerPlane() const { return m_params.outerPlane; }
    int MuonCalibRpcSectorLogic::overlapPhi() const { return m_params.overlapPhi; }
    int MuonCalibRpcSectorLogic::overlapEta() const { return m_params.overlapEta; }
    int MuonCalibRpcSectorLogic::triggerBcid() const { return m_params.triggerBcid; }
    int MuonCalibRpcSectorLogic::isInput() const { return m_params.isInput; }

    MuonCalibRpcSectorLogic::MuonCalibRpcSectorLogic(MuonCalibRpcSectorLogic::defineParams pars) : m_params{pars} {}

}  // namespace MuonCalib
