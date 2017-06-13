# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

####################################################################
# TrigThinningHelper.py
# James.Catmore@cern.ch
# Ryan.White@cern.ch
# This is a helper class to hide the details of the thinning service
# configuration from the derivation job options. 
# The code does three things:
# (1) establishes a thinning service for a given stream provided by
# the user
# (2) sets up trigger navigation thinning 
# (3) passes the thinning service back to the job options on request 
# For use for trigger studies for AOD
####################################################################

# Needed import(s):
import AthenaCommon.CfgMgr as CfgMgr
from AthenaServices.Configurables import ThinningSvc as AthThinningSvc

## Custom specialisation of the core ThinningSvc configurable
#
# Unfortunately the ThinningSvc configurable in AthenaServices was not written
# with extendability in mind. So in order to change its behaviour, one must
# inherit from it. As most of the settings made by that class are only made
# after the jobOption code is already done executing...
#
class WorkerThinningSvc( AthThinningSvc ):

    def __init__( self, name, athStream, navThinSvc, **kwargs ):

        # Remember the object's properties:
        self.athStream = athStream
        self.navThinSvc = navThinSvc
        # Set up a logger object:
        from AthenaCommon.Logging import logging
        self.log = logging.getLogger( name )
        # Call the initialisation of the base class:
        AthThinningSvc.__init__( self, name, **kwargs )
        return

    ## Function overriding the one defined in Configurable
    #
    # This is probably not the very best design, but because of how weirdly
    # ThinningSvc.setDefaults(...) is implemented, this seemed to be the best
    # way to tweak the configuration made by that function.
    #
    def setup( self ):

        # Let the base class do all of its setup functions
        super( WorkerThinningSvc, self ).setup()

        # Make the stream talk to this service:
        toolFound = False
        for tool in self.athStream.HelperTools:
            # Check whether there's a tool of this type already attached
            # to the stream. If there is, it should start talking to the
            # navigation thinning service from now on:
            if not isinstance( tool, CfgMgr.ThinningOutputTool ):
                continue
            tool.ThinningSvc = self.navThinSvc
            toolFound = True
            self.log.info( "Updated the existing ThinningOutputTool to point at"
                           " the TrigNavigationThinningSvc instance" )
            pass
        if not toolFound:
            # If there was no such tool yet, let's create one from scratch:
            stream.HelperTools.insert( 0,
                    CfgMgr.ThinningOutputTool( formatName +
                                               "ThinningOutputTool",
                                               ThinningSvc = self.navThinSvc ) )
            self.log.warning( "No ThinningOutputTool found for the output "
                              "stream" )
            self.log.warning( "Configured one now, job will likely be "
                              "faulty..." )
            pass

        return

## Class helping to set up (navigation) thinning in derivation jobs
#
# In order to hide the complexity of using navigation thinning from the users,
# this helper class should be used in derivation jobs instead of the code
# provided by AthenaServices, to set up a functional thinning service for a
# given output stream.
#
# The usage is that the user creates an instance of the class, configures it,
# and then calls its AppendToStream(...) function on the (augmented) stream
# (coming from MSMgr.NewPoolRootStream(...)) that the thinning should be set
# up on.
#
class ThinningHelper:

    ## Constructor, creating a default instance of the object
    #
    # @param helperName The instance name of this helper object
    #
    def __init__( self, helperName, edmList, mode ):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        self.helperName = helperName
        self.TriggerChains = ""
        self.worker = None
        self.edmList = edmList
        self.mode = mode
        from AthenaCommon.Logging import logging
        self.log = logging.getLogger( "ThinningHelper" )
        return

    ## Function creating the correct thinning service
    #
    # This function needs to be called on the output stream that thinning
    # is to be applied for. It creates the correct thinning service
    # infrastructure for the stream, and configures trigger navigation
    # thinning if that was requested.
    #
    # @param augmentedStream The augmented stream object returned by
    #                        MultipleStreamManager
    #
    def AppendToStream( self, augmentedStream ):
        # Access the stream object:
        stream = augmentedStream.GetEventStream()
        # Get the name of the "format":
        formatName = stream.name().strip( "StreamDAOD_" )
        # The necessary import(s):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaServices.Configurables import createThinningSvc
        
        # Let's create the navigation thinning service first. Notice that its
        # 'WorkerThinningSvc' property is not getting set at this point yet.
        svcMgr += CfgMgr.TrigNavigationThinningSvc( formatName +
                                                    "ThinningSvc" )
        thinningSvc = getattr( svcMgr, formatName + "ThinningSvc" )

        # Now set up the worker thinning service:
        svcMgr += WorkerThinningSvc( formatName + "NavThinningWorkerSvc",
                                     athStream = stream,
                                     navThinSvc = thinningSvc,
                                     Streams = [ stream.name() ] )
        self.worker = getattr( svcMgr, formatName + "NavThinningWorkerSvc" )

        # And now point the navigation thinning service to the worker:
        thinningSvc.WorkerThinningSvc = self.worker

        from TrigNavTools.TrigNavToolsConfig import slimmingTool
        sTool = slimmingTool( { 'name' : self.helperName,
                                'features' : self.edmList,
                                'chains' : self.TriggerChains,
                                'mode' : self.mode,
                                'ThinningSvc' : thinningSvc } )
        thinningSvc.SlimmingTool = sTool
        return

    ## Convenience function returning the thinning service
    #
    # This is the thinning service that needs to be passed to "normal"
    # thinning tools to operate on.
    #
    # @returns The thinning service that tools should operate on
    #
    def ThinningSvc( self ):
        return self.worker
