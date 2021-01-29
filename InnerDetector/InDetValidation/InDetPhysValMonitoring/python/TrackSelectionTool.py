# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from InDetRecExample.TrackingCommon import setDefaults
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool


def getInDetTrackSelectionToolLoose(
        name="InDetTrackSelectionToolLoose",
        **kwargs):
    kwargs = setDefaults(kwargs, CutLevel="Loose")
    return InDet__InDetTrackSelectionTool(name, **kwargs)


def getInDetTrackSelectionToolTightPrimary(
        name="InDetTrackSelectionToolTightPrimary",
        **kwargs):
    kwargs = setDefaults(kwargs, CutLevel="TightPrimary")
    return InDet__InDetTrackSelectionTool(name, **kwargs)
