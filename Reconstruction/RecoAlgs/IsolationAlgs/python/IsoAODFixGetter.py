# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Constants import INFO
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec

from egammaRec.Factories import ToolFactory, AlgFactory, getPropertyValue

#---------------------------------------

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,
                                     CaloClusterLocation = "CaloCalTopoClusters")

# configuration for ED computation
def configureEDCorrection(tool):
  """Configure tools and algorithm for energy density correction 
     (only if doEnergyDensityCorrection = True)"""
  # Return if doEnergyDensityCorrection is false
  if not getPropertyValue(tool, 'doEnergyDensityCorrection'):
    return
  if 1:
    print 'JB do not need that for my tests'
    return
  # Set OutputLevel to INFO or higher if tool has it too
  OutputLevel = min(getPropertyValue(tool, 'OutputLevel'), INFO)
  try:
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon.AlgSequence import AlgSequence
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from JetRec.JetRecStandard import jtm
    topSequence = AlgSequence()
    if not hasattr(topSequence,'EDtpIsoVeryForwardAlg'):
      tfcc = configEventDensityTool("EDtpIsoVeryForwardTool", jtm.emget,
                                    radius          = 0.5,
                                    AbsRapidityMin  = 2.5,
                                    AbsRapidityMax  = 4.5,
                                    OutputContainer = "TopoClusterIsoVeryForwardEventShape",
                                    OutputLevel     = OutputLevel
                                    )
      ToolSvc += tfcc
      topSequence += EventDensityAlg("EDtpIsoVeryForwardAlg", EventDensityTool = tfcc)

  except Exception:
    print '\nERROR: could not get handle to ED'
    raise

from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ict
leakTool = ict(name     = "LeakageCorrection",
               #CorrFile = "IsolationCorrections/isolation_ptcorrections_rel20_2.root") # in principle the default
               ) #cannot locate it...
ToolSvc += leakTool
  
from CaloIdentifier import SUBCALO 
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "CaloIsolationTool",
                                postInit                        = [configureEDCorrection],
                                CaloFillRectangularClusterTool  = None,
                                ClustersInConeTool              = CaloClustersInConeTool,
                                PFlowObjectsInConeTool          = None,
                                #IsoLeakCorrectionTool           = None,
                                IsoLeakCorrectionTool           = leakTool,
                                #saveOnlyRequestedCorrections    = True,
                                saveOnlyRequestedCorrections    = False,
                                EMCaloNums                      = [],
                                HadCaloNums                     = [],
                                UseEMScale                      = True,
                                addCaloExtensionDecoration      = False,
                                OutputLevel                     = 3)

TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrackIsolationTool')
from AthenaCommon import CfgMgr
tit = CfgMgr.xAOD__TrackIsolationTool('TrackIsolationTool')
tit.TrackSelectionTool.maxZ0SinTheta = 3
tit.TrackSelectionTool.minPt         = 1000
tit.TrackSelectionTool.CutLevel      = "Loose"

import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# The types to be computed
# a test
IsoTypesEl =  [
  [ isoPar.topoetcone20, 
    isoPar.topoetcone30,
    isoPar.topoetcone40 ]
  ]
IsoTypesPh =  [
  [ isoPar.topoetcone20, 
    isoPar.topoetcone30,
    isoPar.topoetcone40 ]
  ]
IsoTypesFe =  [
  [ isoPar.topoetcone20, 
    isoPar.topoetcone30,
    isoPar.topoetcone40 ]
  ]
IsoTypesMu =  [
  [ isoPar.topoetcone20, 
    isoPar.topoetcone30,
    isoPar.topoetcone40 ]
  ]

# And the corrections
IsoCorEg = [
  [ isoPar.coreTrackPtr ] 
  ]

IsoCorEl = [
  [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ]
]

IsoCorPh = [
  [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ]
]

IsoCorFe = [
  [ isoPar.coreCone, isoPar.pileupCorrection ] 
  ]

IsoCorMu = [
  [ isoPar.coreCone, isoPar.pileupCorrection ] 
  ]

#from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ict
#leakTool = ict(name     = "LeakageCorrection",
#               #CorrFile = "IsolationCorrections/isolation_ptcorrections_rel20_2.root") # in principle the default
#               ) #cannot locate it...
#ToolSvc += leakTool

from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
isoAODFixBuilderElectron = AlgFactory(IsolationBuilder,
                                      name                  = "IsolationBuilderElectron",
                                      ElectronCollectionContainerName    = "Electrons",
                                      PhotonCollectionContainerName      = "",
                                      MuonCollectionContainerName        = "",
                                      FwdElectronCollectionContainerName = "",
                                      CaloCellIsolationTool = None,
                                      #CaloTopoIsolationTool = None,
                                      CaloTopoIsolationTool = CaloIsolationTool,
                                      PFlowIsolationTool    = None,
                                      TrackIsolationTool    = None, 
                                      FeIsoTypes            = [[]] ,
                                      FeCorTypes            = IsoCorFe,
                                      EgIsoTypes            = IsoTypesEl,
                                      EgCorTypes            = IsoCorEl,
                                      MuIsoTypes            = [[]] ,
                                      MuCorTypes            = IsoCorMu,
                                      IsAODFix              = True,
                                      #LeakageTool           = leakTool,
                                      LeakageTool           = None,
                                      IsolateEl             = True,
                                      OutputLevel           = 3)

isoAODFixBuilderPhoton = AlgFactory(IsolationBuilder,
                                    name                  = "IsolationBuilderPhoton",
                                    ElectronCollectionContainerName    = "",
                                    PhotonCollectionContainerName      = "Photons",
                                    MuonCollectionContainerName        = "",
                                    FwdElectronCollectionContainerName = "",
                                    CaloCellIsolationTool = None,
                                    #CaloTopoIsolationTool = None,
                                    CaloTopoIsolationTool = CaloIsolationTool,
                                    PFlowIsolationTool    = None,
                                    #TrackIsolationTool    = TrackIsolationTool,
                                    TrackIsolationTool    = None, 
                                    FeIsoTypes            = [[]] ,
                                    FeCorTypes            = IsoCorFe,
                                    EgIsoTypes            = IsoTypesPh,
                                    EgCorTypes            = IsoCorPh,
                                    MuIsoTypes            = [[]] ,
                                    MuCorTypes            = IsoCorMu,
                                    IsAODFix              = True,
                                    LeakageTool           = leakTool,
                                    IsolateEl             = False,
                                    OutputLevel           = 3)

isoAODFixBuilderMuon = AlgFactory(IsolationBuilder,
                                  name                  = "IsolationBuilderMuon",
                                  ElectronCollectionContainerName    = "",
                                  PhotonCollectionContainerName      = "",
                                  MuonCollectionContainerName        = "Muons",
                                  FwdElectronCollectionContainerName = "",
                                  CaloCellIsolationTool = None,
                                  #CaloTopoIsolationTool = CaloIsolationTool,
                                  CaloTopoIsolationTool = None,
                                  PFlowIsolationTool    = None,
                                  TrackIsolationTool    = None, 
                                  FeIsoTypes            = [[]] ,
                                  FeCorTypes            = IsoCorFe,
                                  EgIsoTypes            = [[]],
                                  EgCorTypes            = IsoCorEg,
                                  MuIsoTypes            = IsoTypesMu,
                                  MuCorTypes            = IsoCorMu,
                                  CustomConfigurationNameMu = 'Core0p05',
                                  IsAODFix              = True,
                                  LeakageTool           = None,
                                  IsolateEl             = False,
                                  OutputLevel           = 3)


isoAODFixBuilderFwdElectron = AlgFactory(IsolationBuilder,
                                         name                  = "IsolationBuilderForwardElectron",
                                         ElectronCollectionContainerName    = "",
                                         PhotonCollectionContainerName      = "",
                                         MuonCollectionContainerName        = "",
                                         FwdElectronCollectionContainerName = "ForwardElectrons",
                                         CaloCellIsolationTool = None,
                                         CaloTopoIsolationTool = CaloIsolationTool,
                                         #CaloTopoIsolationTool = None,
                                         PFlowIsolationTool    = None,
                                         TrackIsolationTool    = None, 
                                         FeIsoTypes            = IsoTypesFe,
                                         FeCorTypes            = IsoCorFe,
                                         EgIsoTypes            = [[]],
                                         EgCorTypes            = IsoCorEg,
                                         MuIsoTypes            = [[]],
                                         MuCorTypes            = IsoCorMu,
                                         IsAODFix              = True,
                                         LeakageTool           = None,
                                         IsolateEl             = False,
                                         OutputLevel           = 3)


from RecExConfig.Configured import Configured
class isoAODFixGetter ( Configured ) :
    
  def __init__(self, intype = ""):
    super( isoAODFixGetter, self ).__init__()
    self.type = intype

    mlog = logging.getLogger ('isoAODFixGetter.py::configure:')
    mlog.info('entering')
    
    # configure iso here:
    try:
      if self.type == "Electrons":
        self._isoBuilderHandle = isoAODFixBuilderElectron()
      elif self.type == "Photons":
        self._isoBuilderHandle = isoAODFixBuilderPhoton()
      elif self.type == "ForwardElectrons":
        self._isoBuilderHandle = isoAODFixBuilderFwdElectron()  
      elif self.type == "Muons":
        self._isoBuilderHandle = isoAODFixBuilderMuon()  
      else:
        mlog.error("wrong object for IsolationBuilder")
    except Exception:
      mlog.error("could not get handle to IsolationBuilder")
      import traceback
      print traceback.format_exc()
      return False
         
  def isoBuilderHandle(self):
    return self._BuilderHandle

