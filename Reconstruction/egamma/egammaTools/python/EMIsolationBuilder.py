# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate all EMIsolationBuilder with default configuration"
__author__ = "Bruno Lenzi"

from AthenaCommon.Constants import INFO # OutputLevel
from egammaTools import egammaToolsConf
from egammaRec.Factories import ToolFactory, getPropertyValue
from egammaRec import egammaKeys
from egammaTools.egammaToolsFactories import EMTrackIsolationTool
from IsolationTool.IsolationToolFactories import CaloIsolationTool
from egammaCaloTools.egammaCaloToolsFactories import  egammaIso, \
    egammaIsoPtCorrection

import ROOT, PyCintex
# Need to be sure base dict is loaded first.
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
EgammaParameters = ROOT.xAOD.Iso

from CaloIdentifier import SUBCALO        
from RecExConfig.RecFlags import rec

# TODO: not needed ?
# from CaloDetDescr.CaloDepthToolBase import CaloDepthToolFactory
# theCaloDepthTool=CaloDepthToolFactory(depth='showerdefault')

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
    from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg
    from JetRec.JetRecStandard import jtm
    tc=configEventDensityTool("EDEgammaCentralTool", jtm.emget,
                              radius          = 0.5,
                              AbsRapidityMin  = 0.0,
                              AbsRapidityMax  = 1.5,
                              OutputContainer = "DensityForEgammaCentral",
                              OutputLevel     = OutputLevel
                              )
    ToolSvc += tc
    tf=configEventDensityTool("EDEgammaForwardTool", jtm.emget,
                              radius          = 0.5,
                              AbsRapidityMin  = 1.5,
                              AbsRapidityMax  = 3.0,
                              OutputContainer = "DensityForEgammaForward",
                              OutputLevel     = OutputLevel
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


EMIsolationBuilder = ToolFactory( egammaToolsConf.EMIsolationBuilder,
        postInit = [configureEDCorrection],
        CellsName = egammaKeys.caloCellKey(),
        CaloNums  = [SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.TILE],
        CaloIsolationTool  = CaloIsolationTool,
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
           EgammaParameters.topoetcone20_core57cells, 
           EgammaParameters.topoetcone30_core57cells,
           EgammaParameters.topoetcone40_core57cells
        ],
        doIsolForTopoSeeded = False,
        Print = False)

