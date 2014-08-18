# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.BeamFlags import jobproperties
import traceback
import EventKernel.ParticleDataType
	
from RecExConfig.Configured import Configured
from InDetRecExample.InDetKeys import InDetKeys
from AthenaCommon.DetFlags import DetFlags




mlog = logging.getLogger ('MCTruthCalssifierBase.py::configure:')
mlog.info('entering')

from AthenaCommon.AppMgr import ToolSvc


# configure tools for ID on
#if DetFlags.detdescr.ID_on() :
	
# Configure the extrapolator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator(name = 'egammaExtrapolator')
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
	           
# add tool ExtrapolateTrackToCalo
from TrackToCalo.TrackToCaloConf import ExtrapolateToCaloTool
exToCalo = ExtrapolateToCaloTool(name         = "exToCalo",
                                 Extrapolator = theAtlasExtrapolator)
ToolSvc+=exToCalo

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
MCTruthClassifier = MCTruthClassifier(name = 'MCTruthClassifier',
                                ExtrapolateToCaloTool = exToCalo)
  
ToolSvc += MCTruthClassifier
