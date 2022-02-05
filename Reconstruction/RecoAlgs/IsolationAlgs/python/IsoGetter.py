# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon.Constants import INFO
from AthenaCommon.Logging import logging
from RecExConfig.RecFlags import rec

from egammaRec.Factories import ToolFactory, AlgFactory, getPropertyValue

#---------------------------------------
# egamma specifics          
from egammaCaloTools.egammaCaloToolsFactories import CaloFillRectangularCluster
from MuonRecExample.MuonRecFlags import muonRecFlags

from AthenaCommon.GlobalFlags import globalflags
isMC = not globalflags.DataSource()=='data'
from IsolationCorrections.IsolationCorrectionsConf import CP__IsolationCorrectionTool as ICT
IsoCorrectionTool = ToolFactory(ICT, name = "LeakageCorrTool",
                                LogLogFitForLeakage = True)

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

doPFlow = False
FlowElementsInConeTool = None
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.BeamFlags import jobproperties
useVertices = True
if jobproperties.Beam.beamType == 'cosmics':
  useVertices = False

if recAlgs.doEFlow() :

  doPFlow = True

  # tool to collect pflow objects in cone
  from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__FlowElementsInConeTool
  FlowElementsInConeTool = ToolFactory(xAOD__FlowElementsInConeTool,
                                       name = "FlowElementsInConeTool")

  from JetRec.JetRecStandard import jtm
  from JetRec.JetRecConf import PseudoJetAlgorithm
  emnpflowget = PseudoJetAlgorithm(
    name               = "emnpflowget",
    Label              = "EMNPFlow",
    InputContainer = "CHSNeutralParticleFlowObjects",
    OutputContainer = "PseudoJetEMNPFlow",
    SkipNegativeEnergy = True,
    )
  jtm += emnpflowget
  # EMTopo (non-origin corrected) clusters
  if not hasattr(topSequence, jtm.emget.name()):
    topSequence += jtm.emget
  # EM Neutral PFOs
  topSequence += emnpflowget

# tool to extrapolate to the calo
from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
#this is just regular extrapolator, but in ToolFactory form
from egammaTools.egammaExtrapolators import egammaExtrapolator
CaloExtensionTool =  ToolFactory (Trk__ParticleCaloExtensionTool,
                                  Extrapolator = egammaExtrapolator)

# configuration for ED computation
# For the time being, it uses all pflow objects (neutral@EM + charged) for pflow
def configureEDCorrection(tool):
  """Configure tools and algorithm for energy density correction 
     (only if doEnergyDensityCorrection = True)"""
  # Return if doEnergyDensityCorrection is false
  if not getPropertyValue(tool, 'doEnergyDensityCorrection'):
    return
  # Set OutputLevel to INFO or higher if tool has it too
  OutputLevel = min(getPropertyValue(tool, 'OutputLevel'), INFO)
  try:
    from AthenaCommon.AppMgr import ToolSvc
    from JetRec.JetRecStandard import jtm
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAthAlg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    # EMTopo (non-origin corrected) clusters
    if not hasattr(topSequence, jtm.emget.name()):
      topSequence += jtm.emget
    if not hasattr(topSequence,'EDtpIsoCentralAlg'):
      tccc = configEventDensityTool("EDtpIsoCentralTool",
                                    inputlabel = jtm.emget.Label,
                                    radius          = 0.5,
                                    AbsRapidityMin  = 0.0,
                                    AbsRapidityMax  = 1.5,
                                    OutputContainer = "TopoClusterIsoCentralEventShape",
                                    OutputLevel     = OutputLevel
                                    )
      ToolSvc += tccc
      topSequence += EventDensityAthAlg("EDtpIsoCentralAlg", EventDensityTool = tccc)

    if not hasattr(topSequence,'EDtpIsoForwardAlg'):
      tfcc = configEventDensityTool("EDtpIsoForwardTool",
                                    inputlabel = jtm.emget.Label,
                                    radius          = 0.5,
                                    AbsRapidityMin  = 1.5,
                                    AbsRapidityMax  = 3.0,
                                    OutputContainer = "TopoClusterIsoForwardEventShape",
                                    OutputLevel     = OutputLevel
                                    )
      ToolSvc += tfcc
      topSequence += EventDensityAthAlg("EDtpIsoForwardAlg", EventDensityTool = tfcc)

    if doPFlow:
      ## Try a neutral density
      if not hasattr(topSequence,'EDnpfIsoCentralAlg'):
        tcnpf = configEventDensityTool("EDnpfIsoCentralTool",
                                       inputlabel = jtm.emnpflowget.Label,
                                       radius          = 0.5,
                                       AbsRapidityMin  = 0.0,
                                       AbsRapidityMax  = 1.5,
                                       OutputContainer = "NeutralParticleFlowIsoCentralEventShape",
                                       OutputLevel     = OutputLevel
                                       )
        ToolSvc += tcnpf
        topSequence += EventDensityAthAlg("EDnpfIsoCentralAlg", EventDensityTool = tcnpf)

      if not hasattr(topSequence,'EDnpfIsoForwardAlg'):
        tfnpf = configEventDensityTool("EDnpfIsoForwardTool",
                                       inputlabel = jtm.emnpflowget.Label,
                                       radius          = 0.5,
                                       AbsRapidityMin  = 1.5,
                                       AbsRapidityMax  = 3.0,
                                       OutputContainer = "NeutralParticleFlowIsoForwardEventShape",
                                       OutputLevel     = OutputLevel
                                       )
        ToolSvc += tfnpf
        topSequence += EventDensityAthAlg("EDnpfIsoForwardAlg", EventDensityTool = tfnpf)

  except Exception:
    print ('\nERROR: could not get handle to ED')
    raise

# The calo isolation tool
from CaloIdentifier import SUBCALO 
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool
MuCaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "MuCaloIsolationTool",
                                  postInit                        = [configureEDCorrection],
                                  FlowElementsInConeTool          = FlowElementsInConeTool,
                                  ParticleCaloExtensionTool       = CaloExtensionTool,
                                  ParticleCaloCellAssociationTool = None,
                                  EMCaloNums                      = [SUBCALO.LAREM],
                                  HadCaloNums                     = [SUBCALO.LARHEC, SUBCALO.TILE],
                                  UseEtaDepPUCorr                 = False)

EgCaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "EgCaloIsolationTool",
                                  postInit                        = [configureEDCorrection],
                                  CaloFillRectangularClusterTool  = CaloFillRectangularCluster,
                                  IsoLeakCorrectionTool           = IsoCorrectionTool,
                                  ParticleCaloExtensionTool       = None,
                                  ParticleCaloCellAssociationTool = None,
                                  isMC                            = rec.doTruth())

# The track isolation tool
from AthenaCommon import CfgMgr
from TrackVertexAssociationTool.getTTVAToolForReco import getTTVAToolForReco
TTVATool = getTTVAToolForReco(name         = 'NAMWTTVA',
                              WorkingPoint = 'Nonprompt_All_MaxWeight')

ElTrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'ElTrackIsolationTool',
                                   TTVATool          = TTVATool,
                                   CoreTrackEtaRange = 0.01)
eltit = CfgMgr.xAOD__TrackIsolationTool('ElTrackIsolationTool')
eltit.TrackSelectionTool.minPt    = 1000
eltit.TrackSelectionTool.CutLevel = "Loose"

TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrackIsolationTool',
                                 TTVATool = TTVATool)
tit = CfgMgr.xAOD__TrackIsolationTool('TrackIsolationTool')
tit.TrackSelectionTool.minPt    = 1000
tit.TrackSelectionTool.CutLevel = "Loose"

if not useVertices:
  tit.VertexLocation = ''
  eltit.VertexLocation = ''

# Import the xAOD isolation parameters.
from xAODPrimitives.xAODIso import xAODIso as isoPar

# List of isolation to be computed for each collection
tIsoTypesPh = [ [ isoPar.ptcone30, isoPar.ptcone20 ] ] ## these ones will also trigger the ptvarconeXX. Be carefull : store them in decreasing dR
cIsoTypesPh = [ [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ] ]

tIsoTypesEl = [ [ isoPar.ptcone30, isoPar.ptcone20 ] ]
cIsoTypesEl = [ [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ] ]

tIsoTypesMu = [ [ isoPar.ptcone40, isoPar.ptcone30, isoPar.ptcone20 ] ]
cIsoTypesMu = [ [ isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ] ]

# The Default corrections : have to follow exactly the order of the iso var.
tIsoCorEg = [ [ isoPar.coreTrackPtr ] ]
cIsoCorEg = [ [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ] ]

tIsoCorMu = [ [ isoPar.coreTrackPtr ] ]
cIsoCorMu = [ [ isoPar.coreCone, isoPar.pileupCorrection ] ]

# Extra correction
# nothing for Eg
IsoCorEgExtra = [[]]
IsoCorPhExtra = [[]]
IsoCorElExtra = [[]]

# for muons
tIsoCorMuExtra = [[]]
cIsoCorMuExtra = [[]]

if doPFlow:
  cIsoTypesMu.append([ isoPar.neflowisol20, isoPar.neflowisol30, isoPar.neflowisol40 ] )
  cIsoCorMu.append([ isoPar.coreCone, isoPar.pileupCorrection ])
  cIsoCorMuExtra.append([])

outLevel = 3

# Calorimeter iso for EGamma : one for photons and electrons
from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
egcisoBuilder = AlgFactory(IsolationBuilder, name = "EgCaloIsolationBuilder",
                           CaloTopoIsolationTool = EgCaloIsolationTool,
                           #storepileupCorrection = True,
                           ElIsoTypes            = [] if not rec.doEgamma() else cIsoTypesEl,
                           ElCorTypes            = cIsoCorEg,
                           ElCorTypesExtra       = IsoCorElExtra,
                           PhIsoTypes            = [] if not rec.doEgamma() else cIsoTypesPh,
                           PhCorTypes            = cIsoCorEg,
                           PhCorTypesExtra       = IsoCorEgExtra,
                           OutputLevel           = outLevel)

# Track iso for electrons
eltisoBuilder = AlgFactory(IsolationBuilder, name = "ElectronTrackIsolationBuilder",
                           TrackIsolationTool    = ElTrackIsolationTool,
                           ElIsoTypes            = [] if not rec.doEgamma() else tIsoTypesEl,
                           ElCorTypes            = tIsoCorEg,
                           ElCorTypesExtra       = IsoCorEgExtra,
                           OutputLevel           = outLevel)

# Track iso for photons : different from above because of the LooseCone selection for track in electron track iso
phtisoBuilder = AlgFactory(IsolationBuilder, name = "PhotonTrackIsolationBuilder",
                           TrackIsolationTool    = TrackIsolationTool,
                           PhIsoTypes            = [] if not rec.doEgamma() else tIsoTypesPh,
                           PhCorTypes            = tIsoCorEg,
                           PhCorTypesExtra       = IsoCorEgExtra,
                           OutputLevel           = outLevel)

# Calorimeter and Track iso for muons
muisoBuilder = AlgFactory(IsolationBuilder, name = "MuonIsolationBuilder",
                          CaloCellIsolationTool = None,
                          CaloTopoIsolationTool = MuCaloIsolationTool,
                          PFlowIsolationTool    = MuCaloIsolationTool,
                          TrackIsolationTool    = TrackIsolationTool,
                          #storepileupCorrection = True,
                          MuIsoTypes            = tIsoTypesMu+cIsoTypesMu if rec.doMuon() and muonRecFlags.doMuonIso() else [],
                          MuCorTypes            = tIsoCorMu+cIsoCorMu,
                          MuCorTypesExtra       = tIsoCorMuExtra+cIsoCorMuExtra,
                          OutputLevel           = outLevel)

from RecExConfig.Configured import Configured
class isoGetter ( Configured ) :
 
  def configure(self):
    mlog = logging.getLogger ('isoGetter.py::configure:')
    mlog.info('entering')        
    
    # configure iso here:
    self._isoBuilderHandle = []
    try:
      if rec.doEgamma():
        self._isoBuilderHandle.extend([egcisoBuilder(),eltisoBuilder(),phtisoBuilder()])
      if rec.doMuonCombined():
        self._isoBuilderHandle.append(muisoBuilder())
    except Exception:
      mlog.error("could not get handle to IsolationBuilder")
      import traceback
      traceback.print_exc()
      return False
    
    #print self._isoBuilderHandle
    return True

  def isoBuilderHandle(self):
    return self._BuilderHandle

