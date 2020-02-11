# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from ConfigUtils import setDefaults
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool

def getInDetTrackSelectionToolLoose(**kwargs) :
  '''
  Default InDetTrackSelectionTool for InDetTrackParticles
  '''
  kwargs = setDefaults(kwargs,CutLevel = "Loose" )
  return InDet__InDetTrackSelectionTool(**kwargs)

def getInDetTrackSelectionToolTightPrimary(**kwargs) :
  '''
  Default InDetTrackSelectionTool for InDetTrackParticles
  '''
  kwargs = setDefaults(kwargs, CutLevel = "TightPrimary" )
  return InDet__InDetTrackSelectionTool(**kwargs)
 

