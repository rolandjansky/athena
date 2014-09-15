# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate all EMShowerBuilder with default configuration"
__author__ = "Bruno Lenzi"

from AthenaCommon.Constants import INFO # OutputLevel
from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory, getPropertyValue
from egammaRec import egammaKeys
from egammaTools.egammaToolsFactories import EMTrackIsolationTool
from egammaCaloTools.egammaCaloToolsFactories import egammaShowerShape, egammaTopoIso, \
  egammaIso, egammaIsoPtCorrection

import ROOT, PyCintex
# Need to be sure base dict is loaded first.
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODEgammaDict')
EgammaParameters = ROOT.xAOD.EgammaParameters

from CaloIdentifier import SUBCALO        
from RecExConfig.RecFlags import rec

# TODO: not needed ?
# from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
# theCaloDepthTool=CaloDepthToolFactory(depth='showerdefault')

def configureEDCorrection(tool):
  """SL+JBdV: configure tools and algorithm for energy density correction 
     (only if doEnergyDensityCorrection = True)"""
  # Return if doEnergyDensityCorrection is false
  if not getPropertyValue(tool, 'doEnergyDensityCorrection'):
    return
  # Set OutputLevel to INFO or higher if tool has it too (not working...)
  OutputLevel = min(getPropertyValue(tool, 'doEnergyDensityCorrection'), INFO)
  try:
    from AthenaCommon.AppMgr import ToolSvc
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from JetRec.JetRecStandard import jtm
    tc=configEventDensityTool("EDEgammaCentralTool", jtm.emget,
                              0.5,
                              EtaRange            = [-1.5,1.5],
                              UseAbsoluteEta      = True,
                              UseAreaFourMomentum = True,
                              VariableName = "DensityForEgammaCentral",
#                               OutputLevel = OutputLevel,
                              )
                                                           
    ToolSvc += tc
    tf=configEventDensityTool("EDEgammaForwardTool", jtm.emget,
                              0.5,
                              EtaRange            = [1.5,3.0],
                              UseAbsoluteEta      = True,
                              UseAreaFourMomentum = True,
                              VariableName = "DensityForEgammaForward",
#                               OutputLevel = OutputLevel,
                              )
    ToolSvc += tf
  except Exception:
    print '\nERROR: could not get handle to EDEgammaXTool'
    raise
 
  # add to topsequence 
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()
  topSequence += EventDensityAlg("EDEgammaCentralAlg", EventDensityTool = tc)
  topSequence += EventDensityAlg("EDEgammaForwardAlg", EventDensityTool = tf)


EMShowerBuilder = ToolFactory( egammaToolsConf.EMShowerBuilder,
        postInit = [configureEDCorrection],
        TopoCaloClusterInputName="CaloCalTopoCluster",
        CellsName = egammaKeys.caloCellKey(),
        CaloNums  = [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
        ShowerShapeTool      = egammaShowerShape,
        EMTopoCaloIsolationTool  = egammaTopoIso,
        EMCaloIsolationTool  = egammaIso,
        EMCaloIsoPtCorrectionTool = egammaIsoPtCorrection,
        EMTrackIsolationTool = EMTrackIsolationTool,
        IsoTypes = [
            EgammaParameters.etcone20,
            EgammaParameters.etcone30,
            EgammaParameters.etcone40, 
            ],
       TopoIsoTypes = [] if rec.doHeavyIon() else \
        [
           EgammaParameters.topoetcone20, 
           EgammaParameters.topoetcone30,
           EgammaParameters.topoetcone40
        ],
        doIsolForTopoSeeded = False,
        Print = False)

