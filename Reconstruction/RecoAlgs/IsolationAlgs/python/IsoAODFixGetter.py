# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.Logging import logging

from egammaRec.Factories import ToolFactory, AlgFactory

#---------------------------------------

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,
                                     CaloClusterLocation = "CaloCalTopoClusters")

# tool for the leakage correction (std calo iso)
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ict
leakTool = ict(name     = "LeakageCorrection")
ToolSvc += leakTool

# too to compute the calo iso
from CaloIdentifier import SUBCALO 
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "CaloIsolationTool",
                                CaloFillRectangularClusterTool  = None,
                                ClustersInConeTool              = CaloClustersInConeTool,
                                PFlowObjectsInConeTool          = None,
                                IsoLeakCorrectionTool           = leakTool,
                                saveOnlyRequestedCorrections    = False,
                                EMCaloNums                      = [],
                                HadCaloNums                     = [],
                                UseEMScale                      = True,
                                addCaloExtensionDecoration      = False)

import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# The types to be computed
# For this fix, we recompute topoetconeXX for El, Ph, Fwd...
IsoTypesEG =  [
  [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ]
]

# ...but the corrections are different for El/Ph vs Fwd
IsoCorEG = [
  [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ]
]

IsoCorFe = [
  [ isoPar.coreCone, isoPar.pileupCorrection ] 
]

from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
isoAODFixBuilderElectron = AlgFactory(IsolationBuilder,
                                      name                  = "IsolationBuilderElectron",
                                      ElectronCollectionContainerName    = "Electrons",
                                      PhotonCollectionContainerName      = "",
                                      MuonCollectionContainerName        = "",
                                      FwdElectronCollectionContainerName = "",
                                      CaloCellIsolationTool = None,
                                      CaloTopoIsolationTool = CaloIsolationTool,
                                      PFlowIsolationTool    = None,
                                      TrackIsolationTool    = None, 
                                      FeIsoTypes            = [[]] ,
                                      FeCorTypes            = [[]],
                                      EgIsoTypes            = IsoTypesEG,
                                      EgCorTypes            = IsoCorEG,
                                      MuIsoTypes            = [[]],
                                      MuCorTypes            = [[]],
                                      IsAODFix              = True,
                                      LeakageTool           = None,
                                      IsolateEl             = True)

isoAODFixBuilderPhoton = AlgFactory(IsolationBuilder,
                                    name                  = "IsolationBuilderPhoton",
                                    ElectronCollectionContainerName    = "",
                                    PhotonCollectionContainerName      = "Photons",
                                    MuonCollectionContainerName        = "",
                                    FwdElectronCollectionContainerName = "",
                                    CaloCellIsolationTool = None,
                                    CaloTopoIsolationTool = CaloIsolationTool,
                                    PFlowIsolationTool    = None,
                                    TrackIsolationTool    = None, 
                                    FeIsoTypes            = [[]] ,
                                    FeCorTypes            = [[]],
                                    EgIsoTypes            = IsoTypesEG,
                                    EgCorTypes            = IsoCorEG,
                                    MuIsoTypes            = [[]],
                                    MuCorTypes            = [[]],
                                    IsAODFix              = True,
                                    LeakageTool           = leakTool,
                                    IsolateEl             = False)

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
                                         FeIsoTypes            = IsoTypesEG,
                                         FeCorTypes            = IsoCorFe,
                                         EgIsoTypes            = [[]],
                                         EgCorTypes            = [[]],
                                         MuIsoTypes            = [[]],
                                         MuCorTypes            = [[]],
                                         IsAODFix              = True,
                                         LeakageTool           = None,
                                         IsolateEl             = False)

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
      else:
        mlog.error("wrong object for IsolationBuilder")
    except Exception:
      mlog.error("could not get handle to IsolationBuilder")
      import traceback
      print traceback.format_exc()
      return False
         
  def isoBuilderHandle(self):
    return self._BuilderHandle

