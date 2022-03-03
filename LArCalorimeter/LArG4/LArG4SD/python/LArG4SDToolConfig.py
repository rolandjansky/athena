# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from G4AtlasApps.SimEnums import CalibrationRun, LArParameterization
from ISF_Algorithms.CollectionMergerConfig import CollectionMergerCfg

LArG4__EMBSDTool=CompFactory.LArG4.EMBSDTool
LArG4__EMECSDTool=CompFactory.LArG4.EMECSDTool
LArG4__FCALSDTool=CompFactory.LArG4.FCALSDTool
LArG4__HECSDTool=CompFactory.LArG4.HECSDTool
LArG4__MiniFCALSDTool=CompFactory.LArG4.MiniFCALSDTool
LArG4__DeadSDTool=CompFactory.LArG4.DeadSDTool
LArG4__ActiveSDTool=CompFactory.LArG4.ActiveSDTool
LArG4__InactiveSDTool=CompFactory.LArG4.InactiveSDTool
LArG4__CalibrationDefaultCalculator=CompFactory.LArG4.CalibrationDefaultCalculator

#to be migrated: getCalibrationDefaultCalculator, getDeadMaterialCalibrationHitMerger

def LArActiveSensitiveDetectorToolCfg(ConfigFlags, name="LArActiveSensitiveDetector", **kwargs):
    result = ComponentAccumulator()

    ## Main configuration
    if ConfigFlags.GeoModel.AtlasVersion not in ["tb_LArH6_2003","tb_LArH6_2002"]:
        kwargs.setdefault("StacVolumes",["LArMgr::LAr::EMB::STAC"])
        kwargs.setdefault("PresamplerVolumes",["LArMgr::LAr::Barrel::Presampler::Module"])
        kwargs.setdefault("NegIWVolumes",["LArMgr::LAr::EMEC::Neg::InnerWheel"])
        kwargs.setdefault("NegOWVolumes",["LArMgr::LAr::EMEC::Neg::OuterWheel"])
        kwargs.setdefault("NegBOBarretteVolumes",["LArMgr::LAr::EMEC::Neg::BackOuterBarrette::Module::Phidiv"])
        kwargs.setdefault("MiniVolumes",["LArMgr::MiniFCAL::Wafer"])
    if ConfigFlags.GeoModel.AtlasVersion!="tb_LArH6_2003":
        kwargs.setdefault("PosIWVolumes",["LArMgr::LAr::EMEC::Pos::InnerWheel"])
        kwargs.setdefault("PosOWVolumes",["LArMgr::LAr::EMEC::Pos::OuterWheel"])
        kwargs.setdefault("PosBOBarretteVolumes",["LArMgr::LAr::EMEC::Pos::BackOuterBarrette::Module::Phidiv"])
        kwargs.setdefault("PresVolumes", ["LArMgr::LAr::Endcap::Presampler::LiquidArgon"])
        kwargs.setdefault("SliceVolumes",["LArMgr::LAr::HEC::Module::Depth::Slice"])
    if ConfigFlags.GeoModel.AtlasVersion not in ["tb_LArH6_2002"]:
        kwargs.setdefault("FCAL1Volumes",["LArMgr::LAr::FCAL::Module1::Gap"])
        kwargs.setdefault("FCAL2Volumes",["LArMgr::LAr::FCAL::Module2::Gap"])
        kwargs.setdefault("FCAL3Volumes",["LArMgr::LAr::FCAL::Module3::Gap"])
    # Running PID calibration hits?
    kwargs.setdefault("ParticleID",ConfigFlags.Sim.ParticleID)
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", ["LArCalibrationHitActive"])

    from LArG4Barrel.LArG4BarrelConfigNew import BarrelCalibrationCalculatorCfg, BarrelPresamplerCalibrationCalculatorCfg
    kwargs.setdefault("EMBPSCalibrationCalculator",
                      result.getPrimaryAndMerge(BarrelPresamplerCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCalibrationCalculator",
                      result.getPrimaryAndMerge(BarrelCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4EC.LArG4ECConfigNew import EMECPosInnerWheelCalibrationCalculatorCfg, EMECNegInnerWheelCalibrationCalculatorCfg, EMECPosOuterWheelCalibrationCalculatorCfg, EMECNegOuterWheelCalibrationCalculatorCfg, EMECPosBackOuterBarretteCalibrationCalculatorCfg, EMECNegBackOuterBarretteCalibrationCalculatorCfg, EMECPresamplerCalibrationCalculatorCfg
    kwargs.setdefault("EMECPosIWCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECPosInnerWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegIWCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECNegInnerWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPosOWCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECPosOuterWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegOWCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECNegOuterWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPSCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECPresamplerCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPosBOBCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECPosBackOuterBarretteCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegBOBCalibrationCalculator",
                      result.getPrimaryAndMerge(EMECNegBackOuterBarretteCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4HEC.LArG4HECConfigNew import HECCalibrationWheelActiveCalculatorCfg
    kwargs.setdefault("HECWActiveCalculator",
                      result.getPrimaryAndMerge(HECCalibrationWheelActiveCalculatorCfg(ConfigFlags)).name)

    from LArG4FCAL.LArG4FCALConfigNew import FCAL1CalibCalculatorCfg, FCAL2CalibCalculatorCfg, FCAL3CalibCalculatorCfg
    kwargs.setdefault("FCAL1CalibCalculator",
                      result.getPrimaryAndMerge(FCAL1CalibCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL2CalibCalculator",
                      result.getPrimaryAndMerge(FCAL2CalibCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL3CalibCalculator",
                      result.getPrimaryAndMerge(FCAL3CalibCalculatorCfg(ConfigFlags)).name)

    from LArG4MiniFCAL.LArG4MiniFCALConfigNew import MiniFCALActiveCalibrationCalculatorCfg
    kwargs.setdefault("MiniFCALActiveCalibrationCalculator",
                      result.getPrimaryAndMerge (MiniFCALActiveCalibrationCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__ActiveSDTool(name, **kwargs))
    return result


def LArDeadSensitiveDetectorToolCfg(ConfigFlags, name="LArDeadSensitiveDetector", **kwargs):
    ## Main configuration
    kwargs.setdefault("BarrelCryVolumes",   ["LArMgr::LAr::Barrel::Cryostat::InnerWall::Vis",
                                             "LArMgr::LAr::Barrel::Cryostat::Sector::*",
                                             "LArMgr::LAr::Barrel::Cryostat::InnerWall",
                                             "LArMgr::LAr::Barrel::Cryostat::Cylinder::*"])
    kwargs.setdefault("BarrelCryLArVolumes",["LArMgr::LAr::Barrel::Cryostat::ExtraMat*",
                                             "LArMgr::LAr::Barrel::Cryostat::HalfLAr*",
                                             "LArMgr::LAr::Barrel::Cryostat::TotalLAr",
                                             "LArMgr::LAr::Barrel::Cryostat::MotherVolume"])
    kwargs.setdefault("BarrelCryMixVolumes",["LArMgr::LAr::Barrel::Cryostat::InnerEndWall",
                                             "LArMgr::LAr::Barrel::Cryostat::OuterWall",
                                             "LArMgr::LAr::Barrel::Cryostat::Mixed::Cylinder::*"])
    kwargs.setdefault("DeadMaterialVolumes",["LArMgr::LAr::DM::*"])
    kwargs.setdefault("BarrelPresVolumes",  ["LArMgr::LAr::Barrel::Presampler",
                                             "LArMgr::LAr::Barrel::Presampler::Sector",
                                             "LArMgr::LAr::Barrel::Presampler::ProtectionShell",
                                             "LArMgr::LAr::Barrel::Presampler::MotherBoard",
                                             "LArMgr::LAr::Barrel::Presampler::Connectics",
                                             "LArMgr::LAr::Barrel::Presampler::Rail",
                                             "LArMgr::LAr::Barrel::Presampler::ProtectionPlate"])
    kwargs.setdefault("BarrelVolumes",      ["LArMgr::LAr::EMB::ExtraMat*",
                                             "LArMgr::LAr::EMB::FrontBack::Absorber",
                                             "LArMgr::LAr::EMB::FrontBack::Absorber2",
                                             "LArMgr::LAr::EMB::FrontBack::Steel",
                                             "LArMgr::LAr::EMB::FrontBack::G10",
                                             "LArMgr::LAr::EMB::FrontBack::Electrode",
                                             "LArMgr::LAr::EMB::GTENB",
                                             "LArMgr::LAr::EMB::GTENF",
                                             "LArMgr::LAr::EMB::SUMB",
                                             "LArMgr::LAr::EMB::CAAC",
                                             "LArMgr::LAr::EMB::MOAC",
                                             "LArMgr::LAr::EMB::TELB",
                                             "LArMgr::LAr::EMB::TELF",
                                             "LArMgr::LAr::EMB::ECAM"])
    kwargs.setdefault("ECCryVolumes",       ["LArMgr::LAr::Endcap::Cryostat::FcalLAr::Cylinder",
                                             "LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector",
                                             "LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Cylinder",
                                             "LArMgr::LAr::Endcap::Cryostat::Sector",
                                             "LArMgr::LAr::Endcap::Cryostat::Cone",
                                             "LArMgr::LAr::Endcap::Cryostat::Cylinder"])
    kwargs.setdefault("ECCryLArVolumes",    ["LArMgr::Moderator*",
                                             "LArMgr::LAr::Endcap::Cryostat::FcalLAr",
                                             "LArMgr::LAr::Endcap::Cryostat::EmecHecLAr",
                                             "LArMgr::LAr::Endcap::Cryostat::MotherVolume"])
    kwargs.setdefault("ECCryMixVolumes",    ["LArMgr::LAr::FCAL::LiquidArgonC",
                                             "LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector::Mixed",
                                             "LArMgr::LAr::Endcap::Cryostat::Sector::Mixed",
                                             "LArMgr::LAr::Endcap::Cryostat::Cone::Mixed",
                                             "LArMgr::LAr::Endcap::Cryostat::ExtraCyl_beforePS",
                                             "LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed"])
    kwargs.setdefault("ECSupportVolumes",   ["LArMgr::LAr::EMEC::ExtraCyl_afterPS",
                                             "LArMgr::LAr::EMEC::InnerTransversalBars",
                                             "LArMgr::LAr::EMEC::InnerAluCone::*",
                                             "LArMgr::LAr::EMEC::OuterTransversalBars",
                                             "LArMgr::LAr::EMEC::OuterSupportMother",
                                             "LArMgr::LAr::EMEC::*Stretchers",
                                             "LArMgr::LAr::EMEC::Top*",
                                             "LArMgr::LAr::EMEC::Back*GTen",
                                             "LArMgr::LAr::EMEC::Back*Hole",
                                             "LArMgr::LAr::EMEC::Back*Bar",
                                             "LArMgr::LAr::EMEC::Back*Ring",
                                             "LArMgr::LAr::EMEC::Back*Ele",
                                             "LArMgr::LAr::EMEC::Back*Abs",
                                             "LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv",
                                             "LArMgr::LAr::EMEC::Back*Barrette::Module",
                                             "LArMgr::LAr::EMEC::Back*Barrettes",
                                             "LArMgr::LAr::EMEC::BackSupport*",
                                             "LArMgr::LAr::EMEC::Front*",
                                             "LArMgr::LAr::EMEC::Mother*"])
    kwargs.setdefault("HECWheelVolumes",    ["LArMgr::LAr::HEC::Mother",
                                             "LArMgr::LAr::HEC::LiquidArgon",
                                             "LArMgr::LAr::HEC::Clamp",
                                             "LArMgr::LAr::HEC::Clamp::LiquidArgon",
                                             "LArMgr::LAr::HEC::Module",
                                             "LArMgr::LAr::HEC::FrontModule",
                                             "LArMgr::LAr::HEC::RearModule",
                                             "LArMgr::LAr::HEC::Module::Depth",
                                             "LArMgr::LAr::HEC::Module::Depth::FirstAbsorber",
                                             "LArMgr::LAr::HEC::Module::Depth::FirstAbsorber::TieRod"])
    # Running PID calibration hits?
    kwargs.setdefault("ParticleID", ConfigFlags.Sim.ParticleID)
    kwargs.setdefault("doEscapedEnergy", ConfigFlags.Sim.CalibrationRun is not CalibrationRun.DeadLAr)
    # No effect currently
    outputCollectionName = "LArCalibrationHitDeadMaterial"
    if ConfigFlags.Sim.CalibrationRun in [CalibrationRun.LAr, CalibrationRun.LArTile]:
        outputCollectionName = "LArCalibrationHitDeadMaterial_DEAD"
    kwargs.setdefault("HitCollectionName", outputCollectionName)

    result = ComponentAccumulator()

    from LArG4Barrel.LArG4BarrelConfigNew import BarrelCryostatCalibrationCalculatorCfg, BarrelCryostatCalibrationLArCalculatorCfg, BarrelCryostatCalibrationMixedCalculatorCfg, DMCalibrationCalculatorCfg,   BarrelCalibrationCalculatorCfg, BarrelPresamplerCalibrationCalculatorCfg
    kwargs.setdefault("EMBCryoCalibrationCalculator", result.getPrimaryAndMerge(BarrelCryostatCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCryoLArCalibrationCalculator", result.getPrimaryAndMerge(BarrelCryostatCalibrationLArCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("DefaultCalibrationCalculator", result.getPrimaryAndMerge(CalibrationDefaultCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCryoMixCalibrationCalculator", result.getPrimaryAndMerge(BarrelCryostatCalibrationMixedCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("DMCalibrationCalculator", result.getPrimaryAndMerge(DMCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBPSCalibrationCalculator", result.getPrimaryAndMerge(BarrelPresamplerCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCalibrationCalculator", result.getPrimaryAndMerge(BarrelCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4EC.LArG4ECConfigNew import EndcapCryostatCalibrationCalculatorCfg, EndcapCryostatCalibrationLArCalculatorCfg, EndcapCryostatCalibrationMixedCalculatorCfg, EMECSupportCalibrationCalculatorCfg
    kwargs.setdefault("ECCryoCalibrationCalculator", result.getPrimaryAndMerge(EndcapCryostatCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("ECCryoLArCalibrationCalculator", result.getPrimaryAndMerge(EndcapCryostatCalibrationLArCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("ECCryoMixCalibrationCalculator", result.getPrimaryAndMerge(EndcapCryostatCalibrationMixedCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECSuppCalibrationCalculator", result.getPrimaryAndMerge(EMECSupportCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4HEC.LArG4HECConfigNew import HECCalibrationWheelDeadCalculatorCfg
    kwargs.setdefault("HECWheelDeadCalculator", result.getPrimaryAndMerge(HECCalibrationWheelDeadCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools(LArG4__DeadSDTool(name, **kwargs))
    return result


def LArEMBSensitiveDetectorCfg(ConfigFlags,name="LArEMBSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    bare_collection_name = "LArHitEMB"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "LArEMBHits"
    region = "CALO"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)

    result.merge(acc)
    ## Main configuration
    kwargs.setdefault("StacVolumes",["LArMgr::LAr::EMB::STAC"])
    kwargs.setdefault("PresamplerVolumes",["LArMgr::LAr::Barrel::Presampler::Module"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    # Hook for fast simulation
    kwargs.setdefault("UseFrozenShowers", ConfigFlags.Sim.LArParameterization is not LArParameterization.NoFrozenShowers)

    from LArG4Barrel.LArG4BarrelConfigNew import EMBPresamplerCalculatorCfg, EMBCalculatorCfg
    kwargs.setdefault("EMBPSCalculator", result.getPrimaryAndMerge(EMBPresamplerCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCalculator", result.getPrimaryAndMerge(EMBCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__EMBSDTool(name, **kwargs) )
    return result


def LArEMECSensitiveDetectorCfg(ConfigFlags, name="LArEMECSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    bare_collection_name = "LArHitEMEC"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "LArEMECHits"
    region = "CALO"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    result.merge(acc)

    if ConfigFlags.GeoModel.AtlasVersion not in ["tb_LArH6_2002","tb_LArH6EC_2002"]:
        kwargs.setdefault("NegIWVolumes",["LArMgr::LAr::EMEC::Neg::InnerWheel"])
        kwargs.setdefault("NegOWVolumes",["LArMgr::LAr::EMEC::Neg::OuterWheel"])
        kwargs.setdefault("NegBOBarretteVolumes",["LArMgr::LAr::EMEC::Neg::BackOuterBarrette::Module::Phidiv"])
    if ConfigFlags.GeoModel.AtlasVersion !="tb_LArH6EC_2002":
        kwargs.setdefault("PosIWVolumes",["LArMgr::LAr::EMEC::Pos::InnerWheel"])
        kwargs.setdefault("PosOWVolumes",["LArMgr::LAr::EMEC::Pos::OuterWheel"])
        kwargs.setdefault("PosBOBarretteVolumes",["LArMgr::LAr::EMEC::Pos::BackOuterBarrette::Module::Phidiv"])
    kwargs.setdefault("PresVolumes", ["LArMgr::LAr::Endcap::Presampler::LiquidArgon"])
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    # Hook for fast simulation
    kwargs.setdefault("UseFrozenShowers", ConfigFlags.Sim.LArParameterization is not LArParameterization.NoFrozenShowers)

    from LArG4EC.LArG4ECConfigNew import EMECPosInnerWheelCalculatorCfg, EMECNegInnerWheelCalculatorCfg, EMECPosOuterWheelCalculatorCfg, EMECNegOuterWheelCalculatorCfg, EMECPresamplerCalculatorCfg, EMECPosBackOuterBarretteCalculatorCfg, EMECNegBackOuterBarretteCalculatorCfg
    kwargs.setdefault("EMECPosIWCalculator", result.getPrimaryAndMerge(EMECPosInnerWheelCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegIWCalculator", result.getPrimaryAndMerge(EMECNegInnerWheelCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPosOWCalculator", result.getPrimaryAndMerge(EMECPosOuterWheelCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegOWCalculator", result.getPrimaryAndMerge(EMECNegOuterWheelCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPSCalculator", result.getPrimaryAndMerge(EMECPresamplerCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPosBOBCalculator", result.getPrimaryAndMerge(EMECPosBackOuterBarretteCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegBOBCalculator", result.getPrimaryAndMerge(EMECNegBackOuterBarretteCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__EMECSDTool(name, **kwargs) )
    return result


def LArFCALSensitiveDetectorCfg(ConfigFlags, name="LArFCALSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    bare_collection_name = "LArHitFCAL"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "LArFCALHits"
    region = "CALO"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    result.merge(acc)

    kwargs.setdefault("FCAL1Volumes",["LArMgr::LAr::FCAL::Module1::Gap"])
    kwargs.setdefault("FCAL2Volumes",["LArMgr::LAr::FCAL::Module2::Gap"])
    kwargs.setdefault("FCAL3Volumes",["LArMgr::LAr::FCAL::Module3::Gap"])
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    # Hook for fast simulation
    kwargs.setdefault("UseFrozenShowers", ConfigFlags.Sim.LArParameterization is not LArParameterization.NoFrozenShowers)

    from LArG4FCAL.LArG4FCALConfigNew import FCAL1CalculatorCfg, FCAL2CalculatorCfg, FCAL3CalculatorCfg
    kwargs.setdefault("FCAL1Calculator", result.getPrimaryAndMerge(FCAL1CalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL2Calculator", result.getPrimaryAndMerge(FCAL2CalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL3Calculator", result.getPrimaryAndMerge(FCAL3CalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__FCALSDTool(name, **kwargs) )
    return result


def LArHECSensitiveDetectorCfg(ConfigFlags, name="LArHECSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    bare_collection_name = "LArHitHEC"
    mergeable_collection_suffix = "_G4"
    merger_input_property = "LArHECHits"
    region = "CALO"
    acc, hits_collection_name = CollectionMergerCfg(ConfigFlags,
                                                    bare_collection_name,
                                                    mergeable_collection_suffix,
                                                    merger_input_property,
                                                    region)
    result.merge(acc)

    kwargs.setdefault("WheelVolumes",["LArMgr::LAr::HEC::Module::Depth::Slice"])
    #kwargs.setdefault("SliceVolumes",["LAr::HEC::Module::Depth::Slice"])
    #kwargs.setdefault("LocalVolumes",["LAr::HEC::Module::Depth::Slice::Local"])
    #  You might think this should go here, but we don't think so!  LAr::HEC::Module::Depth::Slice::Wheel"])
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", [hits_collection_name])

    from LArG4HEC.LArG4HECConfigNew import HECWheelCalculatorCfg
    kwargs.setdefault("HECWheelCalculator", result.getPrimaryAndMerge(HECWheelCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__HECSDTool(name, **kwargs) )
    return result


def LArInactiveSensitiveDetectorToolCfg(ConfigFlags, name="LArInactiveSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    ## Main configuration
    if ConfigFlags.GeoModel.AtlasVersion not in ["tb_LArH6_2003","tb_LArH6_2002"]:
        kwargs.setdefault("BarrelPreVolumes",["LArMgr::LAr::Barrel::Presampler::Cathode*",
                                              "LArMgr::LAr::Barrel::Presampler::Anode*",
                                              "LArMgr::LAr::Barrel::Presampler::Prep*"])
        kwargs.setdefault("BarrelVolumes",["LArMgr::LAr::EMB::*::Straight",
                                           "LArMgr::LAr::EMB::*::*Fold"])
        kwargs.setdefault("ECPosInVolumes", ["LArMgr::LAr::EMEC::Pos::InnerWheel::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::InnerWheel::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::InnerWheel::Glue",
                                             "LArMgr::LAr::EMEC::Pos::InnerWheel::Lead",
                                             "LArMgr::LAr::EMEC::Pos::InnerCone::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::InnerCone::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::InnerCone::Glue",
                                             "LArMgr::LAr::EMEC::Pos::InnerCone::Lead",
                                             "LArMgr::LAr::EMEC::Pos::InnerSlice*::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::InnerSlice*::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::InnerSlice*::Glue",
                                             "LArMgr::LAr::EMEC::Pos::InnerSlice*::Lead"])
        kwargs.setdefault("ECPosOutVolumes",["LArMgr::LAr::EMEC::Pos::OuterWheel::Lead",
                                             "LArMgr::LAr::EMEC::Pos::OuterWheel::Glue",
                                             "LArMgr::LAr::EMEC::Pos::OuterWheel::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::OuterWheel::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::Outer*Cone::Lead",
                                             "LArMgr::LAr::EMEC::Pos::Outer*Cone::Glue",
                                             "LArMgr::LAr::EMEC::Pos::Outer*Cone::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::Outer*Cone::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::OuterSlice*::Lead",
                                             "LArMgr::LAr::EMEC::Pos::OuterSlice*::Glue",
                                             "LArMgr::LAr::EMEC::Pos::OuterSlice*::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::OuterSlice*::Absorber"])
        kwargs.setdefault("ECNegInVolumes", ["LArMgr::LAr::EMEC::Neg::InnerWheel::Absorber",
                                             "LArMgr::LAr::EMEC::Neg::InnerWheel::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::InnerWheel::Glue",
                                             "LArMgr::LAr::EMEC::Neg::InnerWheel::Lead",
                                             "LArMgr::LAr::EMEC::Neg::InnerCone::Absorber",
                                             "LArMgr::LAr::EMEC::Neg::InnerCone::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::InnerCone::Glue",
                                             "LArMgr::LAr::EMEC::Neg::InnerCone::Lead",
                                             "LArMgr::LAr::EMEC::Neg::InnerSlice*::Absorber",
                                             "LArMgr::LAr::EMEC::Neg::InnerSlice*::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::InnerSlice*::Glue",
                                             "LArMgr::LAr::EMEC::Neg::InnerSlice*::Lead"])
        kwargs.setdefault("ECNegOutVolumes",["LArMgr::LAr::EMEC::Neg::OuterWheel::Lead",
                                             "LArMgr::LAr::EMEC::Neg::OuterWheel::Glue",
                                             "LArMgr::LAr::EMEC::Neg::OuterWheel::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::OuterWheel::Absorber",
                                             "LArMgr::LAr::EMEC::Neg::Outer*Cone::Lead",
                                             "LArMgr::LAr::EMEC::Neg::Outer*Cone::Glue",
                                             "LArMgr::LAr::EMEC::Neg::Outer*Cone::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::Outer*Cone::Absorber",
                                             "LArMgr::LAr::EMEC::Neg::OuterSlice*::Lead",
                                             "LArMgr::LAr::EMEC::Neg::OuterSlice*::Glue",
                                             "LArMgr::LAr::EMEC::Neg::OuterSlice*::Electrode",
                                             "LArMgr::LAr::EMEC::Neg::OuterSlice*::Absorber"])
        #kwargs.setdefault("HECVolumes",["LAr::HEC::Inactive"])
        #kwargs.setdefault("HECLocalVolumes",["LAr::HEC::Local::Inactive"])
        kwargs.setdefault("HECWheelVolumes",["LArMgr::LAr::HEC::Module::Depth::Absorber::TieRod",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::TieRodDead",
                                             "LArMgr::LAr::HEC::Module::Depth::Absorber",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::TieRod",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::Electrode"])
        kwargs.setdefault("MiniMomVolumes",["LArMgr::MiniFCALMother"])
        kwargs.setdefault("MiniVolumes",["LArMgr::MiniFCAL"])
        kwargs.setdefault("MiniLayVolumes",["LArMgr::MiniFCAL::Layer"])
    if ConfigFlags.GeoModel.AtlasVersion=="tb_LArH6_2002":
        kwargs.setdefault("ECPosInVolumes", ["LArMgr::LAr::EMEC::Pos::InnerWheel::Absorber",
                                             "LArMgr::LAr::EMEC::Pos::InnerWheel::Electrode"])
        kwargs.setdefault("ECPosOutVolumes",["LArMgr::LAr::EMEC::Pos::OuterWheel::Electrode",
                                             "LArMgr::LAr::EMEC::Pos::OuterWheel::Absorber"])
        kwargs.setdefault("HECWheelVolumes",["LArMgr::LAr::HEC::Module::Depth::Absorber",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::Electrode",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper",
                                             "LArMgr::LAr::HEC::Module::Depth::Slice::TieRod"])
    if ConfigFlags.GeoModel.AtlasVersion!="tb_LArH6_2002":
        kwargs.setdefault("FCAL1Volumes",["LArMgr::LAr::FCAL::Module1::CableTrough",
                                          "LArMgr::LAr::FCAL::Module1::Absorber"])
        kwargs.setdefault("FCAL2Volumes",["LArMgr::LAr::FCAL::Module2::CableTrough",
                                          "LArMgr::LAr::FCAL::Module2::Absorber",
                                          "LArMgr::LAr::FCAL::Module2::Rod"])
        kwargs.setdefault("FCAL3Volumes",["LArMgr::LAr::FCAL::Module3::CableTrough",
                                          "LArMgr::LAr::FCAL::Module3::Absorber",
                                          "LArMgr::LAr::FCAL::Module3::Rod"])
    # Running PID calibration hits?
    kwargs.setdefault("ParticleID",ConfigFlags.Sim.ParticleID)
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", ["LArCalibrationHitInactive"])

    from LArG4Barrel.LArG4BarrelConfigNew import BarrelCalibrationCalculatorCfg, BarrelPresamplerCalibrationCalculatorCfg
    kwargs.setdefault("EMBPSCalibrationCalculator", result.getPrimaryAndMerge(BarrelPresamplerCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMBCalibrationCalculator", result.getPrimaryAndMerge(BarrelCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4EC.LArG4ECConfigNew import EMECPosInnerWheelCalibrationCalculatorCfg, EMECNegInnerWheelCalibrationCalculatorCfg, EMECPosOuterWheelCalibrationCalculatorCfg, EMECNegOuterWheelCalibrationCalculatorCfg
    kwargs.setdefault("EMECPosIWCalibrationCalculator", result.getPrimaryAndMerge(EMECPosInnerWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegIWCalibrationCalculator", result.getPrimaryAndMerge(EMECNegInnerWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECPosOWCalibrationCalculator", result.getPrimaryAndMerge(EMECPosOuterWheelCalibrationCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("EMECNegOWCalibrationCalculator", result.getPrimaryAndMerge(EMECNegOuterWheelCalibrationCalculatorCfg(ConfigFlags)).name)

    from LArG4HEC.LArG4HECConfigNew import HECCalibrationWheelInactiveCalculatorCfg
    kwargs.setdefault("HECWheelInactiveCalculator", result.getPrimaryAndMerge(HECCalibrationWheelInactiveCalculatorCfg(ConfigFlags)).name)

    from LArG4FCAL.LArG4FCALConfigNew import FCAL1CalibCalculatorCfg, FCAL2CalibCalculatorCfg, FCAL3CalibCalculatorCfg
    kwargs.setdefault("FCAL1CalibCalculator", result.getPrimaryAndMerge(FCAL1CalibCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL2CalibCalculator", result.getPrimaryAndMerge(FCAL2CalibCalculatorCfg(ConfigFlags)).name)
    kwargs.setdefault("FCAL3CalibCalculator", result.getPrimaryAndMerge(FCAL3CalibCalculatorCfg(ConfigFlags)).name)

    from LArG4MiniFCAL.LArG4MiniFCALConfigNew import MiniFCALInactiveCalibrationCalculatorCfg
    kwargs.setdefault("MiniFCALInactiveCalibrationCalculator", result.getPrimaryAndMerge(MiniFCALInactiveCalibrationCalculatorCfg(ConfigFlags)).name)

    result.setPrivateTools( LArG4__InactiveSDTool(name, **kwargs) )
    return result


def LArMiniFCALSensitiveDetectorToolCfg(ConfigFlags, name="LArMiniFCALSensitiveDetector", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("MiniVolumes",["LArMgr::MiniFCAL::Wafer"])
    # No effect currently
    kwargs.setdefault("OutputCollectionNames", ["LArHitMiniFCAL"])

    result.setPrivateTools(LArG4__MiniFCALSDTool(name, **kwargs))
    return result


def CalibrationDefaultCalculatorCfg(ConfigFlags, name="CalibrationDefaultCalculator", **kwargs):
    result = ComponentAccumulator()
    result.addService( LArG4__CalibrationDefaultCalculator(name, **kwargs), primary = True)
    return result

#todo -> migrate this
#def getDeadMaterialCalibrationHitMerger(name="DeadMaterialCalibrationHitMerger", **kwargs):
#    kwargs.setdefault("InputHits",["LArCalibrationHitDeadMaterial_DEAD","LArCalibrationHitActive_DEAD","LArCalibrationHitInactive_DEAD"])
#    kwargs.setdefault("OutputHits","LArCalibrationHitDeadMaterial")
#    return CfgMgr.LArG4__CalibrationHitMerger(name, **kwargs)
