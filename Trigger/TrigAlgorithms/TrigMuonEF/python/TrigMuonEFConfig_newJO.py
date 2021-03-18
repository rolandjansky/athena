#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def TrigMuonEFTrackIsolationToolCfg(flags, name = "TrigMuonTrackIsoTool", **kwargs):

    acc = ComponentAccumulator()
    trackIsolation = CompFactory.TrigMuonEFTrackIsolationTool
    from AthenaCommon.SystemOfUnits import mm
    kwargs.setdefault('deltaZCut', 2.0*mm)
    kwargs.setdefault('removeSelf',True)
    kwargs.setdefault('useAnnulus',False)
    kwargs.setdefault('useVarIso',True)
    # Get the track selection tool
    trkseltool = CompFactory.InDet.InDetTrackSelectionTool("TrigMuonIsoTrackSelTool")
    kwargs.setdefault('TrackSelectionTool',trkseltool)
    isoTool = trackIsolation(name, **kwargs)
    acc.setPrivateTools(isoTool)
    return acc


def TrigMuonEFTrackIsolationAlgCfg(flags, name = "TrigMuonEFTrackIsolation", **kwargs):

    trackIsolationAlg = CompFactory.TrigMuonEFTrackIsolationAlgMT
    acc = TrigMuonEFTrackIsolationToolCfg(flags)

    kwargs.setdefault('OnlineIsolationTool', acc.popPrivateTools())
    kwargs.setdefault('IdTrackParticles', 'InDetTrigTrackingxAODCnv_Muon_IDTrig')
    kwargs.setdefault('requireCombinedMuon', True)
    kwargs.setdefault('useVarIso', True)
    kwargs.setdefault('MuonContName', 'MuonsIso')
    kwargs.setdefault('ptcone02Name', 'MuonsIso.ptcone02')
    kwargs.setdefault('ptcone02Name', 'MuonsIso.ptcone03')

    isoAlg = trackIsolationAlg(name, **kwargs)
    acc.addEventAlgo(isoAlg)

    return acc

def MuonFilterAlgCfg(flags, name="FilterZeroMuons", **kwargs):

    filterZeroMuons = CompFactory.MuonFilterAlg
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonContainerLocation", "MuonsCB")

    acc.addEventAlgo(filterZeroMuons(name, **kwargs))

    return acc

def MergeEFMuonsAlgCfg(flags, name="MergeEFMuonsAlg", **kwargs):

    mergeMuons = CompFactory.MergeEFMuonsAlg
    acc = ComponentAccumulator()
    kwargs.setdefault("MuonCBContainerLocation", "MuonsCB_outsideIn")
    kwargs.setdefault("MuonInsideOutContainerLocation", "MuonsCB_insideOut")
    kwargs.setdefault("MuonOutputLocation", "MuonsCB")

    acc.addEventAlgo(mergeMuons(name, **kwargs))
    return acc
