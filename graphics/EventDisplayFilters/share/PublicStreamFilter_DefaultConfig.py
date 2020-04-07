# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#####################################################################################
# Add a filter algorithm that cuts on IS parameters for the public stream
#####################################################################################
from EventDisplayFilters.PublicStreamFilter import PublicStreamFilter

PublicStreamFilter = PublicStreamFilter(
    name = "PublicStreamFilter",
    OutputLevel = DEBUG,
    CheckReadyForPhysics = True,
    CheckRunNumber = True,
    Tier0Project = "data10_7TeV"
)

topSequence.insert(0, PublicStreamFilter)
topSequence.StopOverride=False
