# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
import traceback
from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

def PAU_AtlasExtrapolator():
    from AthenaCommon.AppMgr import ToolSvc
    from AthenaCommon import CfgMgr

    # #########################################################################################################
    #  Configure the extrapolator
    # #########################################################################################################
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    theAtlasExtrapolator=AtlasExtrapolator(name = 'PAUegammaExtrapolator')
    theAtlasExtrapolator.DoCaloDynamic = False # this turns off dynamic calculation of eloss in calorimeters
    # all left to MaterialEffects/EnergyLossUpdators
    
    MaterialEffectsUpdator = CfgMgr.Trk__MaterialEffectsUpdator
    AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'AtlasMaterialEffectsUpdator')
    ToolSvc += AtlasMaterialEffectsUpdator #default material effects updator
    NoElossMaterialEffectsUpdator = MaterialEffectsUpdator(name = 'NoElossMaterialEffectsUpdator')
    NoElossMaterialEffectsUpdator.EnergyLoss = False
    ToolSvc += NoElossMaterialEffectsUpdator
    
    # setup MaterialEffectsUpdator arrays
    MyUpdators = []
    MyUpdators += [ToolSvc.AtlasMaterialEffectsUpdator] # for ID
    MyUpdators += [ToolSvc.NoElossMaterialEffectsUpdator] # for Calo
    # MyUpdators += [ToolSvc.NoElossMaterialEffectsUpdator] # for muon
    
    MySubUpdators = []
    MySubUpdators += [ToolSvc.AtlasMaterialEffectsUpdator.name()] # for ID
    MySubUpdators += [ToolSvc.NoElossMaterialEffectsUpdator.name()] # for Calo
    MySubUpdators += [ToolSvc.NoElossMaterialEffectsUpdator.name()] # for muon
    
    theAtlasExtrapolator.MaterialEffectsUpdators = MyUpdators
    theAtlasExtrapolator.SubMEUpdators = MySubUpdators
    ToolSvc+=theAtlasExtrapolator

    return theAtlasExtrapolator
