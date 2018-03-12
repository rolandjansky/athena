#Custom isolation configurations for testing new recommendations
# taken from DerivationFrameworkMuons/share/MUON1.py
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool
from AthenaCommon import CfgMgr

tighttrackvertexassotool=CfgMgr.CP__TightTrackVertexAssociationTool("TightTrackVertexAssociationTool", dzSinTheta_cut=3, doPV=True) 
ToolSvc+=tighttrackvertexassotool 

from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
TrackIsolationLoose500 = xAOD__TrackIsolationTool(name = 'TrackIsolationToolLoose500')
TrackIsolationLoose500.TrackSelectionTool.minPt         = 500
TrackIsolationLoose500.TrackSelectionTool.CutLevel      = "Loose"
TrackIsolationLoose500.TrackSelectionTool.maxZ0SinTheta = 3.
TrackIsolationLoose500.UseTTVAtool = False
ToolSvc += TrackIsolationLoose500
TrackIsolationTight1000 = xAOD__TrackIsolationTool(name = 'TrackIsolationToolTight1000')
TrackIsolationTight1000.TrackSelectionTool.minPt         = 1000
TrackIsolationTight1000.TrackSelectionTool.CutLevel      = "Loose"
TrackIsolationTight1000.UseTTVAtool = True
TrackIsolationTight1000.TrackVertexAssociationTool = tighttrackvertexassotool
ToolSvc += TrackIsolationTight1000
TrackIsolationTight500 = xAOD__TrackIsolationTool(name = 'TrackIsolationToolTight500')
TrackIsolationTight500.TrackSelectionTool.minPt         = 500
TrackIsolationTight500.TrackSelectionTool.CutLevel      = "Loose"
TrackIsolationTight500.UseTTVAtool = True
TrackIsolationTight500.TrackVertexAssociationTool = tighttrackvertexassotool
ToolSvc += TrackIsolationTight500

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
ptconeList = [[isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20]]

isoBuilderLoose500 = IsolationBuilder(
    name                   = "IsolationBuilderLoose500",
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationLoose500, 
    EgIsoTypes             = [[]],
    MuIsoTypes             = ptconeList,
    CustomConfigurationNameMu = "LooseTTVA_pt500",
    IsAODFix = True,
    LeakageTool = None,
    OutputLevel            = 3)
DerivationFrameworkJob += isoBuilderLoose500
isoBuilderTight1000 = IsolationBuilder(
    name                   = "IsolationBuilderTight1000",
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight1000, 
    EgIsoTypes             = [[]],
    MuIsoTypes             = ptconeList,
    CustomConfigurationNameMu = "TightTTVA_pt1000",
    IsAODFix = True,
    LeakageTool = None,
    OutputLevel            = 3)
DerivationFrameworkJob += isoBuilderTight1000
isoBuilderTight500 = IsolationBuilder(
    name                   = "IsolationBuilderTight500",
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight500, 
    EgIsoTypes             = [[]],
    MuIsoTypes             = ptconeList,
    CustomConfigurationNameMu = "TightTTVA_pt500",
    IsAODFix = True,
    LeakageTool = None,
    OutputLevel            = 3)
DerivationFrameworkJob += isoBuilderTight500
