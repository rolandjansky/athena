# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

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
        self.helperName = helperName
        self.TriggerChains = ""
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
        
        from TrigNavTools.TrigNavToolsConfig import navigationThinningSvc
        tSvc = navigationThinningSvc( { 'name' : self.helperName,
                                        'features' : self.edmList,
                                        'chains' : self.TriggerChains,
                                        'mode' : self.mode } )

        for t in stream.HelperTools:
            if t.getType() == 'Athena::ThinningCacheTool':
                t.TrigNavigationThinningSvc = tSvc
                break
        else:
            log.error ("Can't find ThinningCacheTool for stream %s", stream)

        return

