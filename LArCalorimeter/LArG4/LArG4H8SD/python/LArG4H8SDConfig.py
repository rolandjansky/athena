# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getLArH8CalibSensitiveDetector(name="LArH8CalibSensitiveDetector", **kwargs):
    kwargs.setdefault("BarrelCryDeadVolumes",
                      ["LArMgr::LAr::TBBarrel::Cryostat::*"])
    kwargs.setdefault("BarrelPreInactiveVolumes",
                      ["LArMgr::LAr::Barrel::Presampler::Prep*",
                       "LArMgr::LAr::Barrel::Presampler::Anode*",
                       "LArMgr::LAr::Barrel::Presampler::Cathode*"])
    kwargs.setdefault("BarrelPreDeadVolumes",
                      ["LArMgr::LAr::Barrel::Presampler",
                       "LArMgr::LAr::Barrel::Presampler::Sector",
                       "LArMgr::LAr::Barrel::Presampler::MotherBoard",
                       "LArMgr::LAr::Barrel::Presampler::ProtectionShell",
                       "LArMgr::LAr::Barrel::Presampler::MotherBoard",
                       "LArMgr::LAr::Barrel::Presampler::Connectics",
                       "LArMgr::LAr::Barrel::Presampler::Rail",
                       "LArMgr::LAr::Barrel::Presampler::ProtectionPlate"])
    kwargs.setdefault("BarrelPreCalibVolumes",
                      ["TBBarrel::Presampler::Module::Calibration"]);
    kwargs.setdefault("BarrelInVolumes",
                      ["LArMgr::LAr::EMB::*Abs::*",
                       "LArMgr::LAr::EMB::Electrode::*"])
    kwargs.setdefault("BarrelDeadVolumes",
                      ["LArMgr::LAr::EMB::ECAM",
                       "LArMgr::LAr::EMB::TELF",
                       "LArMgr::LAr::EMB::TELFB",
                       "LArMgr::LAr::EMB::MOAC",
                       "LArMgr::LAr::EMB::CAAC",
                       "LArMgr::LAr::EMB::SUMB",
                       "LArMgr::LAr::EMB::GTENF",
                       "LArMgr::LAr::EMB::GTENB",
                       "LArMgr::LAr::EMB::FrontBack::Electrode",
                       "LArMgr::LAr::EMB::FrontBack::G10",
                       "LArMgr::LAr::EMB::FrontBack::Steel",
                       "LArMgr::LAr::EMB::FrontBack::Absorber2",
                       "LArMgr::LAr::EMB::FrontBack::Absorber"])
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", ["LArHitH8"])
    return CfgMgr.LArG4__H8CalibSDTool(name, **kwargs)

def getLArTBCryostatCalibrationCalculator(name="LArTBCryostatCalibrationCalculator", **kwargs):
    return CfgMgr.LArTBCryostatCalibrationCalculator(name, kwargs)

def getH8CalibrationDefaultCalculator(name="H8CalibrationDefaultCalculator", **kwargs):
    return CfgMgr.H8CalibrationDefaultCalculator(name, **kwargs)
