# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


## @package BunchCrossingConfProvider
#
# This python module holds the function which can be used to easily set up a tool
# that can provide the Trig::IBunchCrossingConfProvider interface in an analysis
# job. The code relies on the functions defined in BunchCrossingTool.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#

##
# @short Function creating an instance of Trig::IBunchCrossingConfProvider
#
# This function is pretty much the same as BunchCrossingTool.BunchCrossingTool.
# The only reason for having it separately is that not all the tools provided
# by the BunchCrossingTool(...) function provide the Trig::IBunchCrossingConfProvider
# interface.
#
# @returns The default configuration of the job-appropriate Trig::IBunchCrossingConfProvider
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
def BunchCrossingConfProvider( type = "" ):

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "BunchCrossingConfProvider" )

    # If the user requested some instance directly:
    if type != "":
        if type == "TrigConf":
            __logger.info( "Forcing the usage of TrigConfBunchCrossingTool" )
            from TrigBunchCrossingTool.BunchCrossingTool import TrigConfBunchCrossingTool
            return TrigConfBunchCrossingTool()
        elif type == "LHC":
            __logger.error( "The LHCBunchCrossingTool is no longer supported" )
            return None
        elif type == "MC":
            from TrigBunchCrossingTool.BunchCrossingTool import MCBunchCrossingTool
            __logger.info( "Forcing the usage of MCBunchCrossingTool" )
            return MCBunchCrossingTool()
        else:
            __logger.warning( "Type = " + type + " not recognized" )
            __logger.warning( "Will select tool type based on global flags" )

    # Decide which tool to use based on the global flags:
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource() == "data":
        from RecExConfig.RecFlags import rec
        if rec.doTrigger():
            from TrigBunchCrossingTool.BunchCrossingTool import TrigConfBunchCrossingTool
            __logger.info( "Selecting TrigConfBunchCrossingTool for this job" )
            return TrigConfBunchCrossingTool()
    else:
        __logger.info( "Selecting MCBunchCrossingTool for this job" )
        from TrigBunchCrossingTool.BunchCrossingTool import MCBunchCrossingTool
        return MCBunchCrossingTool()

    return None
