# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


## @package BunchCrossingTool
#
# This python module holds the functions which everybody should use to create
# a Trig::IBunchCrossingTool with a default configuration.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#

##
# @short Function instantiating the correct type of bunch crossing tool
#
# This function can be used in a general analysis jobOption to instantiate a correctly
# configured Trig::IBunchCrossingTool implementation. Dependign on the global flags
# the function instantiates either a Trig::TrigConfBunchCrossingTool (for data input)
# or a Trig::MCBunchCrossingTool (for MC input).
#
# It's also possible to request a specific tool type explicitly by specifying one of
# the following types to the function: "TrigConf", "LHC", "MC", "Web". But in general
# it should just be called without specifying anything.
#
# @returns The default configuration of the job-appropriate Trig::IBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
def BunchCrossingTool( type = "" ):

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "BunchCrossingTool" )

    # If the user requested some instance directly:
    if type != "":
        if type == "TrigConf":
            __logger.info( "Forcing the usage of TrigConfBunchCrossingTool" )
            return TrigConfBunchCrossingTool()
        elif type == "LHC":
            __logger.error( "The LHCBunchCrossingTool is no longer supported" )
            return None
        elif type == "MC":
            __logger.error( "The MCBunchCrossingTool is no longer supported" )
            return None
        elif type == "Web":
            __logger.info( "Forcing the usage of WebBunchCrossingTool" )
            return WebBunchCrossingTool()
        else:
            __logger.warning( "Type = " + type + " not recognized" )
            __logger.warning( "Will select tool type based on global flags" )

    # Decide which tool to use based on the global flags:
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource() == "data":
        from RecExConfig.RecFlags import rec
        if rec.doTrigger():
            __logger.info( "Selecting TrigConfBunchCrossingTool for this job" )
            return TrigConfBunchCrossingTool()

    return None

##
# @short Function creating a default configuration for the Trig::TrigConfBunchCrossingTool
#
# This is a tricky function, because it behaves like one would call the default
# constructor of a fully fledged python configurable. But in reality one really
# just calls a function which returns a python configurable.
#
# Note: As it turns out, the BunchGroup information in the trigger COOL folders is wrong
# for some runs. So this should not be the default tool for accessing information about
# data anymore.
#
# @returns The default configuration of a public Trig::TrigConfBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
def TrigConfBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "TrigConfBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        __logger.debug( "Tool already exists, not configuring it again" )
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__TrigConfBunchCrossingTool
    __tool = Trig__TrigConfBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the TrigConfBunchCrossingTool "
                   "configuration" )

    # Check for xAODConfigSvc:
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if not hasattr( svcMgr, "xAODConfigSvc" ):
        from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigSvc
        svcMgr += TrigConf__xAODConfigSvc('xAODConfigSvc')

    # Add the tool to ToolSvc and return it to the user:
    ToolSvc += __tool
    return getattr( ToolSvc, __defaultToolName )


##
# @short Function creating the default configuration for Trig::WebBunchCrossingTool
#
# The function instantiates a tool that reads the bunch pattern from the
# http://atlas-trigconf.cern.ch webpages. This tool should only be used for data
# files, and only when TrigConfBunchCrossingTool and LHCBunchCrossingTool have
# failed for some reason. (Those should be much more GRID-friendly.)
#
# @returns The default configuration of a public Trig::WebBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
def WebBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "WebBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        __logger.debug( "Tool already exists, not configuring it again" )
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__WebBunchCrossingTool
    __tool = Trig__WebBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the WebBunchCrossingTool configuration" )

    # Add the tool to ToolSvc and return it to the user:
    ToolSvc += __tool
    return getattr( ToolSvc, __defaultToolName )

##
# @short Function creating a default configuration for the Trig::StaticBunchCrossingTool
#
# This function should only be used if either you don't have access to COOL
# where you're trying to run your Athena job, or you want to test something.
# By default one should always use TrigConfBunchCrossingTool for data.
#
# Don't forget to set the BGKey parameter of the tool by hand!
#
# @returns The default configuration of a public Trig::StaticBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
def StaticBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "StaticBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__StaticBunchCrossingTool
    __tool = Trig__StaticBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the StaticBunchCrossingTool configuration" )

    # Add the tool to ToolSvc and return it to the user:
    ToolSvc += __tool
    return getattr( ToolSvc, __defaultToolName )
