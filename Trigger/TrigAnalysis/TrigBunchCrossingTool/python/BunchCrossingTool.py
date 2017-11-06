# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: BunchCrossingTool.py 784782 2016-11-17 09:42:26Z krasznaa $

## @package BunchCrossingTool
#
# This python module holds the functions which everybody should use to create
# a Trig::IBunchCrossingTool with a default configuration.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $

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
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
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
            __logger.info( "Forcing the usage of LHCBunchCrossingTool" )
            return LHCBunchCrossingTool()
        elif type == "MC":
            __logger.info( "Forcing the usage of MCBunchCrossingTool" )
            return MCBunchCrossingTool()
        elif type == "Web":
            __logger.info( "Forcing the usage of WebBunchCrossingTool" )
            return WebBunchCrossingTool()
        elif type == "D3PD":
            __logger.info( "Forcing the usage of D3PDBunchCrossingTool" )
            return D3PDBunchCrossingTool()
        else:
            __logger.warning( "Type = " + type + " not recognized" )
            __logger.warning( "Will select tool type based on global flags" )

    # Decide which tool to use based on the global flags:
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.isOverlay():
        __logger.info( "Selecting LHCBunchCrossingTool for overlay job" )
        return LHCBunchCrossingTool()
    if globalflags.DataSource() == "data":
        from RecExConfig.RecFlags import rec
        from RecExConfig.RecAlgsFlags import recAlgs
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        if rec.doTrigger() or TriggerFlags.doTriggerConfigOnly() or recAlgs.doTrigger():
            __logger.info( "Selecting TrigConfBunchCrossingTool for this job" )
            return TrigConfBunchCrossingTool()
        else:
            __logger.info( "Trigger turned off, selecting LHCBunchCrossingTool for "
                           "this job" )
            return LHCBunchCrossingTool()
    else:
        __logger.info( "Selecting MCBunchCrossingTool for this job" )
        return MCBunchCrossingTool()

##
# @short Function creating a default configuration for the Trig::TrigConfBunchCrossingTool
#
# This is a tricky function, because it behaves like one would call the default
# constructor of a fully fledged python configurable. But in reality one really
# just calls a function which returns a python configurable.
#
# To make configuration a bit more simple, the function assumes that it's called from
# a RecExCommon environment, so it assumes that ServiceMgr.TrigConfigSvc has already
# been instantiated. If it doesn't find the service, it will raise an exception.
#
# Note: As it turns out, the BunchGroup information in the trigger COOL folders is wrong
# for some runs. So this should not be the default tool for accessing information about
# data anymore.
#
# @returns The default configuration of a public Trig::TrigConfBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
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

    # Set up the trigger configuration service:
#    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
#    if not hasattr( svcMgr, "TrigConfigSvc" ):
#        __logger.error( "You have to make sure that TrigConfigSvc exists to use this "
#                        "tool!" )
#        __logger.error( "Try running in a RecExCommon environment..." )
#        raise NameError( "ServiceMgr.TrigConfigSvc not configured" )
#    __tool.ConfigSvc = svcMgr.TrigConfigSvc

    # Now make sure that DSConfigSvc has access to the BG COOL folders:
    from IOVDbSvc.CondDB import conddb
    __dbConnection = "TRIGGER"
    __folders = [ "LVL1/BunchGroupKey", "LVL1/BunchGroupDescription",
                  "LVL1/BunchGroupContent" ]
    for f in __folders:
        if not conddb.folderRequested( "/TRIGGER/%s" % f ):
            __logger.info( "Adding folder to IOVDbSvc: /TRIGGER/%s" % f ) 
            conddb.addFolderWithTag( __dbConnection, "/TRIGGER/%s" % f, "HEAD" )
            pass
        pass

    # Add the tool to ToolSvc and return it to the user:
    ToolSvc += __tool
    return getattr( ToolSvc, __defaultToolName )

##
# @short Function creating the default configuration for Trig::MCBunchCrossingTool
#
# This tool reads the bunch pattern information from the MetaData of pileup MC
# files. It probably doesn't work for overlay files, but I couldn't test that
# yet...
#
# @returns The default configuration of a public Trig::MCBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
def MCBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "MCBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        __logger.debug( "Tool already exists, not configuring it again" )
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__MCBunchCrossingTool
    __tool = Trig__MCBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the MCBunchCrossingTool configuration" )

    # Add the tool to ToolSvc:
    ToolSvc += __tool

    # If this is a BS reading job (it *can* happen) then stop at
    # this point, as the metadata folder will not be available.
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.InputFormat() != "pool":
        return getattr( ToolSvc, __defaultToolName )

    # Now make sure the tool has access to the metadata folders:
    from IOVDbSvc.CondDB import conddb
    __dbConnection = ""
    __folders = [ "/Digitization/Parameters" ]
    for f in __folders:
        if not conddb.folderRequested( f ):
            __logger.info( "Adding folder to IOVDbSvc: %s" % f ) 
            conddb.addFolderWithTag( __dbConnection, f, "HEAD" )
            pass
        pass

    # Return the tool to the user:
    return getattr( ToolSvc, __defaultToolName )

##
# @short Function creating the default configuration for Trig::LHCBunchCrossingTool
#
# The function instantiates a tool that reads the bunch pattern from the LHC folders
# of the TDAQ conditions database. It is considered a bit more reliable than the
# trigger configuration information for some runs. But in general one should just
# use TrigConfBunchCrossingTool.
#
# @returns The default configuration of a public Trig::LHCBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
def LHCBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "LHCBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        __logger.debug( "Tool already exists, not configuring it again" )
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__LHCBunchCrossingTool
    __tool = Trig__LHCBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the LHCBunchCrossingTool configuration" )

    # Now make sure the tool has access to the metadata folders:
    from IOVDbSvc.CondDB import conddb
    __dbConnection = "TDAQ"
    __folders = [ "FILLPARAMS", "BUNCHDATA" ]
    for f in __folders:
        if not conddb.folderRequested( "/TDAQ/OLC/LHC/%s" % f ):
            __logger.info( "Adding folder to IOVDbSvc: /TDAQ/OLC/LHC/%s" % f ) 
            conddb.addFolderWithTag( __dbConnection, "/TDAQ/OLC/LHC/%s" % f, "HEAD" )
            pass
        pass

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

##
# @short Function creating a default configuration for the Trig::D3PDBunchCrossingTool
#
# This function should be used when you read a D3PD in an Athena job. It configures
# the D3PDBunchCrossingTool implementation to read the bunch configuration from
# the D3PD metadata.
#
# @returns The default configuration of a public Trig::D3PDBunchCrossingTool
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 784782 $
# $Date: 2016-11-17 10:42:26 +0100 (Thu, 17 Nov 2016) $
def D3PDBunchCrossingTool():

    # The default name of the tool:
    __defaultToolName = "BunchCrossingTool"

    # Get ourselves a logger:
    from AthenaCommon.Logging import logging
    __logger = logging.getLogger( "D3PDBunchCrossingTool" )

    # Check if the tool already exists. If it does, let's just return it without
    # any modifications:
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr( ToolSvc, __defaultToolName ):
        return getattr( ToolSvc, __defaultToolName )

    # Create a new instance of the tool if it doesn't exist yet:
    from TrigBunchCrossingTool.TrigBunchCrossingToolConf import \
         Trig__D3PDBunchCrossingTool
    __tool = Trig__D3PDBunchCrossingTool( __defaultToolName )

    # Create a default configuration for it:
    __logger.info( "Set the default values for the D3PDBunchCrossingTool configuration" )

    # Add the tool to ToolSvc and return it to the user:
    ToolSvc += __tool
    return getattr( ToolSvc, __defaultToolName )
