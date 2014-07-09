# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: GenObjectsFilterTool.py 348268 2011-02-28 16:12:50Z krasznaa $
#

##
# @short Function returning a pre-configured instance of GenObjectsFilterTool
#
# This configuration of the tool that does no filtering at all.
#
def NullTruthFilterTool():

    # Name for this tool instance:
    toolName = "NullTruthFilterTool"

    # Instantiate the tool if necessary:
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, toolName ):
        from TruthD3PDAnalysis import GenObjectsFilterTool
        ToolSvc += GenObjectsFilterTool( toolName )

    # Get hold of the tool:
    tool = getattr( ToolSvc, toolName )

    # Set all of the tool's properties:
    tool.PtMin = -1000000
    tool.EtaMax = -1
    tool.RemoveEmptyEvents = False
    tool.RemoveDummyEvents = False
    tool.RemoveInTimePileUp = False
    tool.Remove2BCPileUp = False
    tool.Remove800nsPileUp = False
    tool.RemoveCavernBkg = False
    tool.SelectTruthTracks = False

    # Return this tool to the user:
    return tool
