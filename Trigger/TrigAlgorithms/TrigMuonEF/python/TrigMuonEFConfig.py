# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# TrigMuonEF configurables
#
from TrigMuonEF import TrigMuonEFConf
from AthenaCommon.SystemOfUnits import mm

def TMEF_TrackIsolationTool(name='TMEF_isolationTool',**kwargs):
    kwargs.setdefault('deltaZCut', 2.0*mm)
    kwargs.setdefault('removeSelf',True)
    kwargs.setdefault('useAnnulus',False)
    kwargs.setdefault('useVarIso',True)
    # Get the track selection tool
    from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
    trkseltool = InDet__InDetTrackSelectionTool()
    kwargs.setdefault('TrackSelectionTool',trkseltool)
    return TrigMuonEFConf.TrigMuonEFTrackIsolationTool(name, **kwargs)



class TrigMuonEFTrackIsolationMTConfig (TrigMuonEFConf.TrigMuonEFTrackIsolationAlgMT):
    __slots__ = ()

    def __init__( self, name="TrigMuonEFTrackIsolationMTConfig" ):
        super( TrigMuonEFTrackIsolationMTConfig, self ).__init__( name )

        # configure the isolation tool
        TMEF_IsolationTool = TMEF_TrackIsolationTool('TMEF_IsolationTool',useVarIso=True)

        # Isolation tool
        self.OnlineIsolationTool = TMEF_IsolationTool

        # ID tracks
        self.IdTrackParticles = "InDetTrigTrackingxAODCnv_Muon_IDTrig"

        # Only run algo on combined muons
        self.requireCombinedMuon = True

        # Use offline isolation variables
        self.useVarIso = True
        self.MuonContName = "Muons"
        self.ptcone02Name = "Muons.ptcone02"
        self.ptcone03Name = "Muons.ptcone03"
