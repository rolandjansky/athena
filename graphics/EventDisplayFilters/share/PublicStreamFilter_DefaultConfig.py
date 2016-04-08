#####################################################################################
# Add a filter algorithm that cuts on IS parameters for the public stream
#####################################################################################
from EventDisplayFilters.PublicStreamFilter import *

PublicStreamFilter = PublicStreamFilter(
    name = "PublicStreamFilter",
    OutputLevel = DEBUG,
    CheckReadyForPhysics = True,
    CheckRunNumber = True,
    Tier0Project = "data10_7TeV"
)

topSequence.insert(0, PublicStreamFilter)
topSequence.StopOverride=False
