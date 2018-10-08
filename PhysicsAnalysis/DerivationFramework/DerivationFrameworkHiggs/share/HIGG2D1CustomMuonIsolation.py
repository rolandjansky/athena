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

import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
ptconeList = [[isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20]]

if not hasattr(DerivationFrameworkJob, "IsolationBuilderLoose500"):
    isoBuilderLoose500 = IsolationBuilder(
        name                   = "IsolationBuilderLoose500",
        CaloCellIsolationTool  = None,
        CaloTopoIsolationTool  = None,
        PFlowIsolationTool     = None,
        TrackIsolationTool     = TrackIsolationLoose500, 
        EgIsoTypes             = [[]],
        MuIsoTypes             = ptconeList,
        CustomConfigurationNameMu = "LooseTTVA_pt500",
        IsAODFix = False,
        LeakageTool = None)
    DerivationFrameworkJob += isoBuilderLoose500
