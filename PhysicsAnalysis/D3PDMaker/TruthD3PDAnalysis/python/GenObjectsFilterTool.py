# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# $Id: GenObjectsFilterTool.py 351835 2011-03-15 11:32:14Z ssnyder $
#
# File holding various configurations for the GenObjectsFilterTool

##
# @short Function returning a pre-configured instance of GenObjectsFilterTool
#
# This configuration of the tool lets the user select stable charged particles
# from the truth record.
#
def TruthTrackFilterTool():

    # Name for this tool instance:
    __toolName = "TruthTrackFilterTool"

    # Instantiate the tool if necessary:
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, __toolName ):
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
        ToolSvc += D3PD__GenObjectsFilterTool( __toolName )

    # Get hold of the tool:
    tool = getattr( ToolSvc, __toolName )

    # Set all of the tool's properties:
    tool.PtMin = 100.0
    tool.EtaMax = 5.0
    tool.RemoveEmptyEvents = True
    tool.RemoveDummyEvents = False
    tool.RemoveInTimePileUp = False
    tool.Remove2BCPileUp = False
    tool.Remove800nsPileUp = False
    tool.RemoveCavernBkg = False
    tool.SelectTruthTracks = True

    # Return this tool to the user:
    return tool

##
# @short Function returning a pre-configured instance of GenObjectsFilterTool
#
# This configuration of the tool lets the user select almost all particles
# from the truth record.
#
def AllTrackFilterTool():

    # Name for this tool instance:
    __toolName = "AllTrackFilterTool"

    # Instantiate the tool if necessary:
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr( ToolSvc, __toolName ):
        from TruthD3PDAnalysis.TruthD3PDAnalysisConf import D3PD__GenObjectsFilterTool
        ToolSvc += D3PD__GenObjectsFilterTool( __toolName )

    # Get hold of the tool:
    tool = getattr( ToolSvc, __toolName )

    # Set all of the tool's properties:
    tool.PtMin = 100.0
    tool.EtaMax = 5.0
    tool.RemoveEmptyEvents = True
    tool.RemoveDummyEvents = False
    tool.RemoveInTimePileUp = False
    tool.Remove2BCPileUp = False
    tool.Remove800nsPileUp = False
    tool.RemoveCavernBkg = False
    tool.SelectTruthTracks = False

    # Return this tool to the user:
    return tool
