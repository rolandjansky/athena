# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: BunchCrossingConfProvider.py 586922 2014-03-10 14:56:39Z krasznaa $

## @package BunchCrossingConfProvider
#
# This python module holds the function which can be used to easily set up a tool
# that can provide the Trig::IBunchCrossingConfProvider interface in an analysis
# job. The code relies on the functions defined in BunchCrossingTool.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 586922 $
# $Date: 2014-03-10 15:56:39 +0100 (Mon, 10 Mar 2014) $

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
# $Revision: 586922 $
# $Date: 2014-03-10 15:56:39 +0100 (Mon, 10 Mar 2014) $
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
            __logger.info( "Forcing the usage of LHCBunchCrossingTool" )
            from TrigBunchCrossingTool.BunchCrossingTool import LHCBunchCrossingTool
            return LHCBunchCrossingTool()
        elif type == "MC":
            from TrigBunchCrossingTool.BunchCrossingTool import MCBunchCrossingTool
            __logger.info( "Forcing the usage of MCBunchCrossingTool" )
            return MCBunchCrossingTool()
        else:
            __logger.warning( "Type = " + type + " not recognized" )
            __logger.warning( "Will select tool type based on global flags" )

    # Decide which tool to use based on the global flags:
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay():
        __logger.info( "Selecting LHCBunchCrossingTool for overlay job" )
        from TrigBunchCrossingTool.BunchCrossingTool import LHCBunchCrossingTool
        return LHCBunchCrossingTool()
    if globalflags.DataSource() == "data":
        from RecExConfig.RecFlags import rec
        from RecExConfig.RecAlgsFlags import recAlgs
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if rec.doTrigger() or TriggerFlags.doTriggerConfigOnly() or recAlgs.doTrigger():
            from TrigBunchCrossingTool.BunchCrossingTool import TrigConfBunchCrossingTool
            __logger.info( "Selecting TrigConfBunchCrossingTool for this job" )
            return TrigConfBunchCrossingTool()
        else:
            __logger.info( "Trigger turned off, selecting LHCBunchCrossingTool for this job" )
            from TrigBunchCrossingTool.BunchCrossingTool import LHCBunchCrossingTool
            return LHCBunchCrossingTool()
    else:
        __logger.info( "Selecting MCBunchCrossingTool for this job" )
        from TrigBunchCrossingTool.BunchCrossingTool import MCBunchCrossingTool
        return MCBunchCrossingTool()
