import re
import sys
from AthenaCommon.Logging import logging

mlog = logging.getLogger ('isoAODFixGetter.py::configure:')


def GetUpdatedIsoTrackCones(name=""):

  algs  = []

  from AthenaCommon.AppMgr import ToolSvc

  #Custom isolation configurations for testing new recommendations
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

  muIsoTypes = [[]]
  egIsoTypes = [[]]
  customNameMu_pt1000 = ""
  customNameEl_pt1000 = ""
  customNameMu_pt500 = ""
  customNameEl_pt500 = ""
  if name == "" or name == "Electrons":
    egIsoTypes = ptconeList
    customNameEl_pt1000 = "TightTTVA_pt1000"
    customNameEl_pt500 = "TightTTVA_pt500"
  if name == "" or name == "Muons":
    muIsoTypes = ptconeList
    customNameMu_pt1000 = "TightTTVA_pt1000"
    customNameMu_pt500 = "TightTTVA_pt500"

  isoBuilderTight1000 = IsolationBuilder(
    name                   = "IsolationBuilderTight1000",
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight1000, 
    EgIsoTypes             = egIsoTypes,
    MuIsoTypes             = muIsoTypes,
    CustomConfigurationNameMu = customNameMu_pt1000,
    CustomConfigurationNameEl = customNameEl_pt1000,
    CustomConfigurationName   = "TightTTVA_pt1000",
    IsAODFix = True,
    LeakageTool = None,
    OutputLevel            = 3)
  algs += [isoBuilderTight1000]
  isoBuilderTight500 = IsolationBuilder(
    name                   = "IsolationBuilderTight500",
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight500, 
    EgIsoTypes             = egIsoTypes,
    MuIsoTypes             = muIsoTypes,
    CustomConfigurationNameMu = customNameMu_pt500,
    CustomConfigurationNameEl = customNameEl_pt500,
    CustomConfigurationName   = "TightTTVA_pt500",
    IsAODFix = True,
    LeakageTool = None,
    OutputLevel            = 3)
  algs += [isoBuilderTight500]
  return algs

def GetExtraIsoVariablesForDxAOD(name=""):
    iso_lep_vars = []
    iso_vars = ["ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptcone30_TightTTVA_pt500.ptcone30_TightTTVA_pt1000.ptcone40_TightTTVA_pt500.ptcone40_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt500.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone40_TightTTVA_pt500.ptvarcone40_TightTTVA_pt1000"]
    if name == "" or name == "Electrons":
        iso_lep_vars += ["Electrons.%s" %iso_vars]
    if name == "" or name == "Muons":
        iso_lep_vars += ["Muons.%s" %iso_vars] 
    return iso_lep_vars

