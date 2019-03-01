# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Constants import INFO
from AthenaCommon.AppMgr import ToolSvc
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
IsoCorrectionTool = ToolFactory(ICT,
                                name = "NewLeakageCorrTool")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

doPFlow = False
PFlowObjectsInConeTool = None
from RecExConfig.RecAlgsFlags import recAlgs
from AthenaCommon.BeamFlags import jobproperties
useVertices = True
if jobproperties.Beam.beamType == 'cosmics':
  useVertices = False

if recAlgs.doEFlow() :

  doPFlow = True

  # tool to collect pflow objects in cone
  from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__PFlowObjectsInConeTool
  PFlowObjectsInConeTool = ToolFactory(xAOD__PFlowObjectsInConeTool,
                                       name = "PFlowObjectsInConeTool")

  from JetRec.JetRecStandard import jtm
  from JetRec.JetRecConf import PseudoJetGetter
  emnpflowget = PseudoJetGetter(
    name               = "emnpflowget",
    Label              = "EMNPFlow",
    InputContainer = "CHSNeutralParticleFlowObjects",
    OutputContainer = "PseudoJetEMNPFlow",
    SkipNegativeEnergy = True,
    )
  jtm += emnpflowget
  # PseudoJetGetters are now run in their own dedicated algs
  from JetRec.JetRecConf import PseudoJetAlgorithm
  # EMTopo (non-origin corrected) clusters
  if not hasattr(topSequence, "pjalg_"+jtm.emget.Label):
    topSequence += PseudoJetAlgorithm("pjalg_"+jtm.emget.Label,PJGetter=jtm.emget)
  # EM Neutral PFOs
  topSequence += PseudoJetAlgorithm("pjalg_"+emnpflowget.Label,PJGetter=emnpflowget)

# tool to collect topo clusters in cone
from ParticlesInConeTools.ParticlesInConeToolsConf import xAOD__CaloClustersInConeTool
CaloClustersInConeTool = ToolFactory(xAOD__CaloClustersInConeTool,
                                     CaloClusterLocation = "CaloCalTopoClusters")

# tool to extrapolate to the calo
import AthenaCommon.CfgMgr as CfgMgr
#this is just regular extrapolator, but in ToolFactory form
from egammaTools.InDetTools import egammaExtrapolator
CaloExtensionTool =  ToolFactory (CfgMgr.Trk__ParticleCaloExtensionTool,
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
    from JetRec.JetRecConf import PseudoJetAlgorithm
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAthAlg
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    # EMTopo (non-origin corrected) clusters
    if not hasattr(topSequence, "pjalg_"+jtm.emget.Label):
      topSequence += PseudoJetAlgorithm("pjalg_"+jtm.emget.Label,PJGetter=jtm.emget)
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

    if not hasattr(topSequence,'EDtpIsoVeryForwardAlg'):
      tvfcc = configEventDensityTool("EDtpIsoVeryForwardTool",
                                     inputlabel = jtm.emget.Label,
                                     radius          = 0.5,
                                     AbsRapidityMin  = 2.5,
                                     AbsRapidityMax  = 4.5,
                                     OutputContainer = "TopoClusterIsoVeryForwardEventShape",
                                     OutputLevel     = OutputLevel
                                   )
      ToolSvc += tvfcc
      topSequence += EventDensityAthAlg("EDtpIsoVeryForwardAlg", EventDensityTool = tvfcc)

    if doPFlow:
      if not hasattr(topSequence,'EDpfIsoCentralAlg'):
        tcpf = configEventDensityTool("EDpfIsoCentralTool",
                                      inputlabel = jtm.empflowget.Label,
                                      radius          = 0.5,
                                      AbsRapidityMin  = 0.0,
                                      AbsRapidityMax  = 1.5,
                                      OutputContainer = "ParticleFlowIsoCentralEventShape",
                                      OutputLevel     = OutputLevel
                                      )
        ToolSvc += tcpf
        topSequence += EventDensityAthAlg("EDpfIsoCentralAlg", EventDensityTool = tcpf)

      if not hasattr(topSequence,'EDpfIsoForwardAlg'):
        tfpf = configEventDensityTool("EDpfIsoForwardTool",
                                      inputlabel = jtm.empflowget.Label,
                                      radius          = 0.5,
                                      AbsRapidityMin  = 1.5,
                                      AbsRapidityMax  = 3.0,
                                      OutputContainer = "ParticleFlowIsoForwardEventShape",
                                      OutputLevel     = OutputLevel
                                      )
        ToolSvc += tfpf
        topSequence += EventDensityAthAlg("EDpfIsoForwardAlg", EventDensityTool = tfpf)

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
    print '\nERROR: could not get handle to ED'
    raise

from CaloIdentifier import SUBCALO 
from IsolationTool.IsolationToolConf import xAOD__CaloIsolationTool, xAOD__TrackIsolationTool
CaloIsolationTool = ToolFactory(xAOD__CaloIsolationTool,name = "CaloIsolationTool",
                                postInit                        = [configureEDCorrection],
                                CaloFillRectangularClusterTool  = CaloFillRectangularCluster,
                                ClustersInConeTool              = CaloClustersInConeTool,
                                PFlowObjectsInConeTool          = PFlowObjectsInConeTool,
                                ParticleCaloExtensionTool       = CaloExtensionTool,
                                IsoLeakCorrectionTool           = IsoCorrectionTool,
                                EMCaloNums                      = [SUBCALO.LAREM],
                                HadCaloNums                     = [SUBCALO.LARHEC, SUBCALO.TILE],
                                UseEMScale                      = True)

TrackIsolationTool = ToolFactory(xAOD__TrackIsolationTool, name = 'TrackIsolationTool')
#from AthenaCommon import CfgMgr
tit = CfgMgr.xAOD__TrackIsolationTool('TrackIsolationTool')
tit.TrackSelectionTool.maxZ0SinTheta = 3
tit.TrackSelectionTool.minPt         = 1000
tit.TrackSelectionTool.CutLevel      = "Loose"
if not useVertices:
  tit.VertexLocation = ''

import ROOT, cppyy
# Need to be sure base dict is loaded first.
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso

# In fact the default isolations are the same for eg and muons : prepare the list here
IsoTypes =  [
  [ isoPar.etcone40, ## Be carefull : store them in decreasing dR
    isoPar.etcone30,
    isoPar.etcone20 ],
  [ isoPar.topoetcone20,
    isoPar.topoetcone30,
    isoPar.topoetcone40 ],
  [ isoPar.ptcone40, ## these ones will also trigger the ptvarconeXX. Be carefull : store them in decreasing dR
    isoPar.ptcone30,
    isoPar.ptcone20 ]
  ]
IsoTypesFe =  [
  [ isoPar.topoetcone20, 
    isoPar.topoetcone30,
    isoPar.topoetcone40 ]
  ]

# The Default corrections : have to follow exactly the order of the iso var.
IsoCorEg = [
  [ isoPar.core57cells, isoPar.ptCorrection ],
  [ isoPar.core57cells, isoPar.ptCorrection, isoPar.pileupCorrection ],
  [ isoPar.coreTrackPtr ]
  ]

IsoCorEgExtra = [
  [ ],
  [ isoPar.coreCone, isoPar.coreConeSC],
  [ ]
  ]

IsoCorMu = [
  [ isoPar.coreMuon ],
  [ isoPar.coreCone, isoPar.pileupCorrection ],
  [ isoPar.coreTrackPtr ]
  ]

IsoCorMuExtra = [
  [ ],
  [ ],
  [ ]
  ]

IsoCorFe = [
  [ isoPar.coreCone, isoPar.pileupCorrection ] 
  ]

IsoCorFeExtra = [[]]

if doPFlow:
  IsoTypes.append(  
    [ isoPar.neflowisol20,
      isoPar.neflowisol30,
      isoPar.neflowisol40 ] )
  IsoCorEg.append([ isoPar.coreCone, isoPar.pileupCorrection ])
  IsoCorEgExtra.append([isoPar.ptCorrection])
  IsoCorMu.append([ isoPar.coreCone, isoPar.pileupCorrection ])
  IsoCorMuExtra.append([])

from IsolationAlgs.IsolationAlgsConf import IsolationBuilder
isoBuilder = AlgFactory(IsolationBuilder,
                        name                  = "IsolationBuilder",
                        CaloCellIsolationTool = CaloIsolationTool,
                        CaloTopoIsolationTool = CaloIsolationTool,
                        PFlowIsolationTool    = CaloIsolationTool,
                        TrackIsolationTool    = TrackIsolationTool, 
                        FeIsoTypes            = [] if not rec.doEgamma() else IsoTypesFe,
                        FeCorTypes            = IsoCorFe,
                        FeCorTypesExtra       = IsoCorFeExtra,
			ElIsoTypes            = [] if not rec.doEgamma() else IsoTypes,
                        ElCorTypes            = IsoCorEg,
                        ElCorTypesExtra       = IsoCorEgExtra,
			PhIsoTypes            = [] if not rec.doEgamma() else IsoTypes,
                        PhCorTypes            = IsoCorEg,
                        PhCorTypesExtra       = IsoCorEgExtra,
			MuIsoTypes            = IsoTypes if rec.doMuon() and muonRecFlags.doMuonIso() else [],
                        MuCorTypes            = IsoCorMu,
                        MuCorTypesExtra       = IsoCorMuExtra
                        )

from RecExConfig.Configured import Configured
class isoGetter ( Configured ) :
 
  def configure(self):
    mlog = logging.getLogger ('isoGetter.py::configure:')
    mlog.info('entering')        
    
    # configure iso here:
    try:
      self._isoBuilderHandle = isoBuilder()
    except Exception:
      mlog.error("could not get handle to IsolationBuilder")
      import traceback
      print traceback.format_exc()
      return False
    
    #print self._isoBuilderHandle
    return True

  def isoBuilderHandle(self):
    return self._BuilderHandle

