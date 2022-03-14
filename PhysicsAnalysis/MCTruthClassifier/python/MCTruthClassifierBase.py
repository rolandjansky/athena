# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.BeamFlags import jobproperties
import traceback
import EventKernel.ParticleDataType
from RecExConfig.Configured import Configured
from InDetRecExample.InDetKeys import InDetKeys
from AthenaCommon.DetFlags import DetFlags
import AthenaCommon.CfgMgr as CfgMgr


mlog = logging.getLogger ('MCTruthClassifierBase.py::configure:')
mlog.info('entering')

from AthenaCommon.AppMgr import ToolSvc
	
# Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator(name = 'MCTruthClassifieExtrapolator')
theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic calculation of eloss in calorimeters
# all left to MaterialEffects/EnergyLossUpdators
	           
from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator as MaterialEffectsUpdator
AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
NoElossMaterialEffectsUpdator.EnergyLoss = False
ToolSvc += NoElossMaterialEffectsUpdator
	           
# setup MaterialEffectsUpdator arrays
MyUpdators = []
# This is truth particles  so 
#    MyUpdators += [AtlasMaterialEffectsUpdator] # for ID
MyUpdators += [NoElossMaterialEffectsUpdator] # for ID
MyUpdators += [NoElossMaterialEffectsUpdator] # for Calo
MyUpdators += [NoElossMaterialEffectsUpdator] # for muon
# MyUpdators += [NoElossMaterialEffectsUpdator] # for muon
	           
MySubUpdators = []
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for ID
#    MySubUpdators += [AtlasMaterialEffectsUpdator.name()] # for ID
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for Calo
MySubUpdators += [NoElossMaterialEffectsUpdator.name()] # for muon
	           
theAtlasExtrapolator.MaterialEffectsUpdators = MyUpdators
theAtlasExtrapolator.SubMEUpdators = MySubUpdators
ToolSvc+=theAtlasExtrapolator
	           
from TrackToCalo.TrackToCaloConf import Trk__ParticleCaloExtensionTool
ClassifierParticleCaloExtensionTool= Trk__ParticleCaloExtensionTool(name="ClassifierParticleCaloExtensionTool",
                                                                    Extrapolator = theAtlasExtrapolator)
ToolSvc+=ClassifierParticleCaloExtensionTool

from MCTruthClassifier.MCTruthClassifierConfig import firstSimCreatedBarcode
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
MCTruthClassifier = MCTruthClassifier(name = 'MCTruthClassifier',
                                      barcodeG4Shift = firstSimCreatedBarcode(),
                                      ParticleCaloExtensionTool=ClassifierParticleCaloExtensionTool)
from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
if commonGeoFlags.Run()=="RUN4":
    MCTruthClassifier.FwdElectronUseG4Sel = False
  
ToolSvc += MCTruthClassifier
