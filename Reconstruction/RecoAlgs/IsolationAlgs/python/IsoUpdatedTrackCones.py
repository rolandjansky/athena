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

  tighttrackvertexassotool=CfgMgr.CP__TrackVertexAssociationTool("TrackVertexAssociationTool_Iso", WorkingPoint="Loose") 
  ToolSvc+=tighttrackvertexassotool 

  from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
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

  TrackIsolationTightLooseCone1000 = xAOD__TrackIsolationTool(name = 'TrackIsolationToolTightLooseCone1000')
  TrackIsolationTightLooseCone1000.TrackSelectionTool.minPt         = 1000
  TrackIsolationTightLooseCone1000.TrackSelectionTool.CutLevel      = "Loose"
  TrackIsolationTightLooseCone1000.UseTTVAtool = True
  TrackIsolationTightLooseCone1000.CoreTrackEtaRange = 0.01
  TrackIsolationTightLooseCone1000.TrackVertexAssociationTool = tighttrackvertexassotool
  ToolSvc += TrackIsolationTightLooseCone1000
  TrackIsolationTightLooseCone500 = xAOD__TrackIsolationTool(name = 'TrackIsolationToolTightLooseCone500')
  TrackIsolationTightLooseCone500.TrackSelectionTool.minPt         = 500
  TrackIsolationTightLooseCone500.TrackSelectionTool.CutLevel      = "Loose"
  TrackIsolationTightLooseCone500.UseTTVAtool = True
  TrackIsolationTightLooseCone500.CoreTrackEtaRange = 0.01
  TrackIsolationTightLooseCone500.TrackVertexAssociationTool = tighttrackvertexassotool
  ToolSvc += TrackIsolationTightLooseCone500

  import ROOT, PyCintex
  PyCintex.loadDictionary('xAODCoreRflxDict')
  PyCintex.loadDictionary('xAODPrimitivesDict')
  isoPar = ROOT.xAOD.Iso
  ptconeList = [[isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20]]

  muIsoTypes = [[]]
  egIsoTypes = [[]]
  customNameMu_pt1000 = ""
  customNameEl_pt1000 = ""
  customNamePh_pt1000 = ""
  customNameElLooseCone_pt1000 = ""
  customNamePhLooseCone_pt1000 = ""
  customNameMu_pt500 = ""
  customNameEl_pt500 = ""
  customNamePh_pt500 = ""
  customNameElLooseCone_pt500 = ""
  customNamePhLooseCone_pt500 = ""
  if name == "" or name == "Electrons":
    egIsoTypes = ptconeList
    customNameEl_pt1000 = "TightTTVA_pt1000"
    customNameEl_pt500 = "TightTTVA_pt500"
    customNameElLooseCone_pt1000 = "TightTTVALooseCone_pt1000"
    customNameElLooseCone_pt500 = "TightTTVALooseCone_pt500"
  if name == "" or name == "Photons":
    egIsoTypes = ptconeList
    customNamePh_pt1000 = "TightTTVA_pt1000"
    customNamePh_pt500 = "TightTTVA_pt500"
    customNamePhLooseCone_pt1000 = "TightTTVALooseCone_pt1000"
    customNamePhLooseCone_pt500 = "TightTTVALooseCone_pt500"
  if name == "" or name == "Muons":
    muIsoTypes = ptconeList
    customNameMu_pt1000 = "TightTTVA_pt1000"
    customNameMu_pt500 = "TightTTVA_pt500"

  isoBuilderTight1000 = IsolationBuilder(
    name                   = "IsolationBuilderTight1000"+name,
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight1000, 
    EgIsoTypes             = egIsoTypes,
    MuIsoTypes             = muIsoTypes,
    CustomConfigurationNameMu = customNameMu_pt1000,
    CustomConfigurationNameEl = customNameEl_pt1000,
    CustomConfigurationNamePh = customNamePh_pt1000,
    CustomConfigurationName   = "TightTTVA_pt1000",
    IsAODFix = False,
    LeakageTool = None)
  algs += [isoBuilderTight1000]

  isoBuilderTight500 = IsolationBuilder(
    name                   = "IsolationBuilderTight500"+name,
    CaloCellIsolationTool  = None,
    CaloTopoIsolationTool  = None,
    PFlowIsolationTool     = None,
    TrackIsolationTool     = TrackIsolationTight500, 
    EgIsoTypes             = egIsoTypes,
    MuIsoTypes             = muIsoTypes,
    CustomConfigurationNameMu = customNameMu_pt500,
    CustomConfigurationNameEl = customNameEl_pt500,
    CustomConfigurationNamePh = customNamePh_pt500,
    CustomConfigurationName   = "TightTTVA_pt500",
    IsAODFix = False,
    LeakageTool = None)
  algs += [isoBuilderTight500]

  if name == "" or name == "Electrons" or name == "Photons":
    isoBuilderTightLooseCone1000 = IsolationBuilder(
      name                   = "IsolationBuilderTightLooseCone1000"+name,
      CaloCellIsolationTool  = None,
      CaloTopoIsolationTool  = None,
      PFlowIsolationTool     = None,
      TrackIsolationTool     = TrackIsolationTightLooseCone1000, 
      EgIsoTypes             = egIsoTypes,
      MuIsoTypes             = [[]],
      CustomConfigurationNameMu = "",
      CustomConfigurationNameEl = customNameElLooseCone_pt1000,
      CustomConfigurationNamePh = customNamePhLooseCone_pt1000,
      CustomConfigurationName   = "TightTTVALooseCone_pt1000",
      IsAODFix = False,
      LeakageTool = None)
    algs += [isoBuilderTightLooseCone1000]

    isoBuilderTightLooseCone500 = IsolationBuilder(
      name                   = "IsolationBuilderTightLooseCone500"+name,
      CaloCellIsolationTool  = None,
      CaloTopoIsolationTool  = None,
      PFlowIsolationTool     = None,
      TrackIsolationTool     = TrackIsolationTightLooseCone500, 
      EgIsoTypes             = egIsoTypes,
      MuIsoTypes             = [[]],
      CustomConfigurationNameMu = "",
      CustomConfigurationNameEl = customNameElLooseCone_pt500,
      CustomConfigurationNamePh = customNamePhLooseCone_pt500,
      CustomConfigurationName   = "TightTTVALooseCone_pt500",
      IsAODFix = False,
      LeakageTool = None)
    algs += [isoBuilderTightLooseCone500]

  return algs

def GetExtraIsoVariablesForDxAOD(name=""):
    iso_lep_vars = []
    iso_vars = "ptcone20_TightTTVA_pt500.ptcone20_TightTTVA_pt1000.ptcone30_TightTTVA_pt500.ptcone30_TightTTVA_pt1000.ptcone40_TightTTVA_pt500.ptcone40_TightTTVA_pt1000.ptvarcone20_TightTTVA_pt500.ptvarcone20_TightTTVA_pt1000.ptvarcone30_TightTTVA_pt500.ptvarcone30_TightTTVA_pt1000.ptvarcone40_TightTTVA_pt500.ptvarcone40_TightTTVA_pt1000"
    iso_varsLooseCone = "ptcone20_TightTTVALooseCone_pt500.ptcone20_TightTTVALooseCone_pt1000.ptcone30_TightTTVALooseCone_pt500.ptcone30_TightTTVALooseCone_pt1000.ptcone40_TightTTVALooseCone_pt500.ptcone40_TightTTVALooseCone_pt1000.ptvarcone20_TightTTVALooseCone_pt500.ptvarcone20_TightTTVALooseCone_pt1000.ptvarcone30_TightTTVALooseCone_pt500.ptvarcone30_TightTTVALooseCone_pt1000.ptvarcone40_TightTTVALooseCone_pt500.ptvarcone40_TightTTVALooseCone_pt1000"
    if name == "" or name == "Electrons":
        iso_lep_vars += ["Electrons.%s" %iso_vars]
        iso_lep_vars += ["Electrons.%s" %iso_varsLooseCone]
    if name == "" or name == "Photons":
        iso_lep_vars += ["Photons.%s" %iso_vars]
        iso_lep_vars += ["Photons.%s" %iso_varsLooseCone]
    if name == "" or name == "Muons":
        iso_lep_vars += ["Muons.%s" %iso_vars] 
    return iso_lep_vars

