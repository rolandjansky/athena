# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getLArG4TBECBeamChSDTool(name="LArG4TBECBeamChSensitiveDetector", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::TBEC::BeamChamber0","LArMgr::LAr::TBEC::BeamChamber1","LArMgr::LAr::TBEC::BeamChamber2","LArMgr::LAr::TBEC::BeamChamber3"])
    return CfgMgr.LArG4TBECBeamChSDTool(name, **kwargs)

def getFrontLArG4H6BeamSDTool(name="LArG4H6BeamSD::Front", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::TBH62004::FrontBeam::Scintillator"])
    return CfgMgr.LArG4H6BeamSDTool(name, **kwargs)

def getMovableLArG4H6BeamSDTool(name="LArG4H6BeamSD::Movable", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::TBH62004::MovableTable::Scintillator","LArMgr::LAr::TB::BPCOLD::bpco_div","LArMgr::LAr::TB::BPC::bpc_xdiv","LArMgr::LAr::TB::BPC::bpc_ydiv","LArMgr::LAr::TB::MWPC::XDiv","LArMgr::LAr::TB::MWPC::YDiv"])
    return CfgMgr.LArG4H6BeamSDTool(name, **kwargs)

def getLArG4H6WarmTC_X(name="LArG4H6WarmTC::X", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::WarmTC::Sci::X"])
    return CfgMgr.getLArG4H6WarmTC(name, **kwargs)

def getLArG4H6WarmTC_Y(name="LArG4H6WarmTC::Y", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::WarmTC::Sci::Y"])
    return CfgMgr.getLArG4H6WarmTC(name, **kwargs)

def getLArG4H6WarmTC_Abs(name="LArG4H6WarmTC::Abs", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::WarmTC::Absorber"])
    return CfgMgr.getLArG4H6WarmTC(name, **kwargs)

def getTBECInnerModuleTool(name="TBECInnerModuleMult", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::EMEC::InnerModule"])
    return CfgMgr.getTBECInnerModuleTool(name,**kwargs)

def getTBECOuterModuleTool(name="TBECOuterModuleMult", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::EMEC::OuterModule"])
    return CfgMgr.getTBECOuterModuleTool(name,**kwargs)

def getTBECBackOuterBarretteTool(name="TBECBackOuterBarretteMult", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv"])
    return CfgMgr.getTBECBackOuterBarretteTool(name,**kwargs)

def getLArG4H62004HECSDTool(name="LArG4H62004HECSDTool", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::HEC::Module::Depth::Slice"])
    return CfgMgr.getLArG4H62004HECSDTool(name,**kwargs)

def getLArG4H62004EMECSDTool(name="LArG4H62004EMECSDTool", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ["LArMgr::LAr::EMEC::InnerWheel"])
    return CfgMgr.getLArG4H62004EMECSDTool(name,**kwargs)

def getLArG4H62004FCALSDTool(name="LArG4H62004FCALSDTool", **kwargs):
    kwargs.setdefault("FCAL1Volumes", ["LArMgr::LAr::FCAL::Module1::Gap"])
    kwargs.setdefault("FCAL2Volumes", ["LArMgr::LAr::FCAL::Module2::Gap"])
    kwargs.setdefault("FCALColdVolumes", ["LArMgr::LAr::FCAL::ColdTC::Gap"])
    return CfgMgr.getLArG4H62004FCALSDTool(name,**kwargs)

def getLArG4H62004ActiveSDTool(name="LArG4H62004ActiveSDTool", **kwargs):
    from G4AtlasApps.SimFlags import simFlags
    if simFlags.LArTB_H6Hec.get_Value():
        kwargs.setdefault("HECVolumes", ["LArMgr::LAr::HEC::Module::Depth::Slice"])
    if simFlags.LArTB_H6Emec.get_Value():
        kwargs.setdefault("EMECVolumes", ["LArMgr::LAr::EMEC::InnerWheel"])
    if simFlags.LArTB_H6Fcal.get_Value():
        kwargs.setdefault("FCAL1Volumes", ["LArMgr::LAr::FCAL::Module1::Gap"])
        kwargs.setdefault("FCAL2Volumes", ["LArMgr::LAr::FCAL::Module2::Gap"])
        kwargs.setdefault("FCALColdVolumes", ["LArMgr::LAr::FCAL::ColdTC::Gap"])
    return CfgMgr.getLArG4H62004ActiveSDTool(name,**kwargs)

def getLArG4H62004DeadSDTool(name="LArG4H62004DeadSDTool", **kwargs):
    theVolumes = []
    if simFlags.LArTB_H6Hec.get_Value():
        theVolumes += [ "LArMgr::LAr::HEC::Module","LArMgr::LAr::HEC::Module::Depth","LArMgr::LAr::HEC::Module::Depth::Slice","LArMgr::LAr::HEC::Module::Depth::Slice::TieRod","LArMgr::LAr::HEC::Module::Depth::FirstAbsorber" ]
    if simFlags.LArTB_H6Emec.get_Value():
        theVolumes += [ "LArMgr::LAr::EMEC::Mother*","LArMgr::LAr::EMEC::Front*","LArMgr::LAr::EMEC::BackSupport*","LArMgr::LAr::EMEC::Back*Barrettes","LArMgr::LAr::EMEC::Back*Barrette::Module","LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv","LArMgr::LAr::EMEC::Back*Abs","LArMgr::LAr::EMEC::Back*Ele","LArMgr::LAr::EMEC::Back*Ring","LArMgr::LAr::EMEC::Back*Bar","LArMgr::LAr::EMEC::Back*Hole","LArMgr::LAr::EMEC::Back*GTen","LArMgr::LAr::EMEC::Top*","LArMgr::LAr::EMEC::*Stretchers","LArMgr::LAr::EMEC::InnerAluCone*","LArMgr::LAr::EMEC::InnerTransversalBars" ]
    if simFlags.LArTB_H6Fcal.get_Value():
        theVolumes += [ "LArMgr::LAr::FCAL::LiquidArgonC","LArMgr::LAr::FCAL::ColdTC::Absorber","LArMgr::LAr::FCAL::ColdTC::Active","LArMgr::LAr::FCAL::ColdTC::Electrode" ]
    theVolumes += [ "LArMgr::LAr::WarmTC::Sci::X","LArMgr::LAr::WarmTC::Sci::Y","LArMgr::LAr::WarmTC::Absorber","LArMgr::LAr::TBH62004::FrontBeam::Scintillator","LArMgr::LAr::TBH62004::MovableTable::Scintillator","LArMgr::LAr::TB::BPCOLD::bpco_div","LArMgr::LAr::TB::BPC::bpc_xdiv","LArMgr::LAr::TB::BPC::bpc_ydiv","LArMgr::LAr::TB::MWPC::XDiv","LArMgr::LAr::TB::MWPC::YDiv","LARTBWORLD::LARTBWORLD","LArMgr::ExpHallLog","LArMgr::LArGeoTB::LeakageDet::ColdNose","LArMgr::LAr::H6::Cryostat::MotherVolume","LArMgr::LArTB::H6::Cryostat::WarmWall","LArMgr::LArTB::H6::Cryostat::Vacuum","LArMgr::LArTB::H6::Cryostat::ColdWall","LArMgr::LArTB::H6::Cryostat::LAr","LArMgr::LArGeoTB::LeakageDet::LAr","LArMgr::LArGeoTB::EMEC::Excluder","LArMgr::LArGeoTB::FCAL::Excluder","LArMgr::LArGeoTB::Front::Excluder","LArMgr::LArGeoTB::Back::Excluder","LArMgr::LAr::WarmTC::MuonWall","LArMgr::LAr::TBH62004::FrontBeam","LArMgr::LAr::TB::MWPC","LArMgr::LAr::TB::MWPC::Mylar","LArMgr::LAr::TB::MWPC::AlFoil","LArMgr::LAr::TB::MWPC::XPlane","LArMgr::LAr::TB::MWPC::YPlane","LArMgr::LAr::TB::MWPC::Wire","LArMgr::LAr::TB::BPC","LArMgr::LAr::TB::BPC::bpcalmylar","LArMgr::LAr::TB::BPC::bpc_mylar","LArMgr::LAr::TB::BPC::bpc_xplane","LArMgr::LAr::TB::BPC::bpc_wire","LArMgr::LAr::TB::BPC::bpc_cwire","LArMgr::LAr::TB::BPC::bpc_yplane","LArMgr::LAr::TBH62004::MiddleBeam","LArMgr::LAr::TB::BPCOLD","LArMgr::LAr::TB::BPCOLD::bpcalmylar","LArMgr::LAr::TB::BPCOLD::bpc_mylar","LArMgr::LAr::TB::BPCOLD::bpco_plane","LArMgr::LAr::TB::BPCOLD::bpco_wire","LArMgr::LAr::TB::BPCOLD::bpco_cwire","LArMgr::LAr::TBH62004::MovableTable","LArMgr::WarmTC_Mother" ]
    kwargs.setdefault("LogicalVolumeNames",theVolumes)
    return CfgMgr.getLArG4H62004DeadSDTool(name,**kwargs)

def getLArG4H62004InactiveSDTool(name="LArG4H62004InactiveSDTool", **kwargs):
    if simFlags.LArTB_H6Hec.get_Value():
        kwargs.setdefault("HECVolumes", ["LArMgr::LAr::HEC::Module::Depth::Absorber","LArMgr::LAr::HEC::Module::Depth::Slice::Electrode","LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper","LArMgr::LAr::HEC::Module::Depth::Slice::TieRod"])
    if simFlags.LArTB_H6Emec.get_Value():
        kwargs.setdefault("EMECVolumes", ["LArMgr::LAr::EMEC::InnerWheel::Absorber","LArMgr::LAr::EMEC::InnerWheel::Electrode"])
    if simFlags.LArTB_H6Fcal.get_Value():
        kwargs.setdefault("FCAL1Volumes", ["LArMgr::LAr::FCAL::Module1::Absorber","LArMgr::LAr::FCAL::Module1::CableTrough"])
        kwargs.setdefault("FCAL2Volumes", ["LArMgr::LAr::FCAL::Module2::Rod","LArMgr::LAr::FCAL::Module2::Absorber","LArMgr::LAr::FCAL::Module2::CableTrough"])
    return CfgMgr.getLArG4H62004InactiveSDTool(name,**kwargs)
