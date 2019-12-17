# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from InDetRecExample.TrackingCommon import setDefaults
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool

def getInDetTrackSelectionToolLoose(**kwargs) :
  kwargs=setDefaults(kwargs, CutLevel = "Loose" )
  return InDet__InDetTrackSelectionTool(**kwargs)

def getInDetTrackSelectionToolTightPrimary(**kwargs) :
  kwargs=setDefaults(kwargs, CutLevel = "TightPrimary" )
  return InDet__InDetTrackSelectionTool(**kwargs)
