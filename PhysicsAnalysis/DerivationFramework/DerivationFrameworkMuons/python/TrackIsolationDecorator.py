# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Add a track isolation updater tool

from AthenaCommon.AppMgr import ToolSvc

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool
IsoCorrectionTool = CP__IsolationCorrectionTool ("NewLeakageCorrTool",IsMC = isMC)
ToolSvc += IsoCorrectionTool

 # tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
ToolSvc += xAOD__CaloClustersInConeTool("MyCaloClustersInConeTool",CaloClusterLocation = "CaloCalTopoClusters")

from CaloIdentifier import SUBCALO

from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool
CaloIsoTool = xAOD__CaloIsolationTool("CaloIsoTool")
CaloIsoTool.IsoLeakCorrectionTool = ToolSvc.NewLeakageCorrTool
CaloIsoTool.ClustersInConeTool = ToolSvc.MyCaloClustersInConeTool
CaloIsoTool.EMCaloNums  = [SUBCALO.LAREM]
CaloIsoTool.HadCaloNums = [SUBCALO.LARHEC, SUBCALO.TILE]
CaloIsoTool.UseEMScale  = True
CaloIsoTool.UseCaloExtensionCaching = False
CaloIsoTool.saveOnlyRequestedCorrections = True
# CaloIsoTool.OutputLevel = 2
ToolSvc += CaloIsoTool


import ROOT, cppyy
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

deco_ptcones = [isoPar.ptcone40, isoPar.ptcone30]
deco_topoetcones = [isoPar.topoetcone40, isoPar.topoetcone20]
deco_prefix = 'MUON1_'

from DerivationFrameworkMuons.DerivationFrameworkMuonsConf import DerivationFramework__isolationDecorator
MUON1IDTrackDecorator = DerivationFramework__isolationDecorator(name = "MUON1IDTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                CaloIsolationTool = CaloIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
#                                                                 SelectionString = "DIMU_trkStatus>0",
                                                                SelectionString = "",
                                                                SelectionFlag = "MUON1DIMU_Status",
                                                                SelectionFlagValue = 1000,
                                                                ptcones = deco_ptcones,
                                                                topoetcones = deco_topoetcones,
                                                                Prefix = deco_prefix
                                                               )

MUON1MSTrackDecorator = DerivationFramework__isolationDecorator(name = "MUON1MSTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                CaloIsolationTool = CaloIsoTool,
                                                                TargetContainer = "ExtrapolatedMuonTrackParticles",
                                                                SelectionString = "",
                                                                SelectionFlag = "",
                                                                SelectionFlagValue = 1000,
                                                                ptcones = deco_ptcones,
                                                                topoetcones = deco_topoetcones,
                                                                Prefix = deco_prefix
                                                               )

deco_prefix2 = 'MUON2_'
MUON2IDTrackDecorator = DerivationFramework__isolationDecorator(name = "MUON2IDTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                CaloIsolationTool = CaloIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
#                                                                 SelectionString = "DIMU_trkStatus>0",
                                                                SelectionString = "",
                                                                SelectionFlag = "MUON2DIMU_Status",
                                                                SelectionFlagValue = 1000,
                                                                ptcones = deco_ptcones,
                                                                topoetcones = deco_topoetcones,
                                                                Prefix = deco_prefix2
                                                               )

MUON2MSTrackDecorator = DerivationFramework__isolationDecorator(name = "MUON2MSTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                CaloIsolationTool = CaloIsoTool,
                                                                TargetContainer = "ExtrapolatedMuonTrackParticles",
                                                                SelectionString = "",
                                                                SelectionFlag = "",
                                                                SelectionFlagValue = 1000,
                                                                ptcones = deco_ptcones,
                                                                topoetcones = deco_topoetcones,
                                                                Prefix = deco_prefix2
                                                               )
