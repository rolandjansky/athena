# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

####################################################################
# ThinningHelper.py
# James.Catmore@cern.ch
# This is a helper class to hide the details of the thinning service
# configuration from the derivation job options. 
# The code does three things:
# (1) establishes a thinning service for a given stream provided by
# the user
# (2) sets up trigger navigation thinning if requested 
# (3) passes the thinning service back to the job options on request 
####################################################################

# Needed import(s):
import AthenaCommon.CfgMgr as CfgMgr

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
    def __init__( self, helperName ):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        self.helperName = helperName
        self.TriggerChains = ""
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
    def AppendToStream( self, augmentedStream, extraTriggerContent = [] ):
        # Access the stream object:
        stream = augmentedStream.GetEventStream()
        # Get the name of the "format":
        formatName = stream.name().strip( "StreamDAOD_" )
        # The necessary import(s):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        
        if self.TriggerChains == "":
            # No trigger selection required.
            return
        
        # And finally, configure what the navigation thinning is supposed
        # to do:
        from DerivationFrameworkMuons.MuonTriggerContent import \
            MuonTriggerContent
        from DerivationFrameworkCore.EGammaTriggerContent import \
            EGammaTriggerContent
        from DerivationFrameworkCore.JetTriggerContent import \
            JetTriggerContent 
        from DerivationFrameworkCore.EtMissTriggerContent import \
            EtMissTriggerContent 
        from DerivationFrameworkCore.TauTriggerContent import \
            TauTriggerContent 
        from DerivationFrameworkFlavourTag.BJetTriggerContent import \
            BJetTriggerContent
        from DerivationFrameworkCore.BPhysTriggerContent import \
            BPhysTriggerContent 
        from DerivationFrameworkCore.MinBiasTriggerContent import \
            MinBiasTriggerContent 
        allFeatures = MuonTriggerContent + EGammaTriggerContent + \
            JetTriggerContent +EtMissTriggerContent + \
            TauTriggerContent + BJetTriggerContent + \
            BPhysTriggerContent + MinBiasTriggerContent + extraTriggerContent
        from TrigNavTools.TrigNavToolsConfig import navigationThinningSvc
        tSvc = navigationThinningSvc( { 'name' : self.helperName,
                                        'features' : allFeatures,
                                        'chains' : self.TriggerChains,
                                        'mode' : 'slimming' } )

        for t in stream.HelperTools:
            if t.getType() == 'Athena::ThinningCacheTool':
                t.TrigNavigationThinningSvc = tSvc
                break
        else:
            log.error ("Can't find ThinningCacheTool for stream %s", stream)

        return

