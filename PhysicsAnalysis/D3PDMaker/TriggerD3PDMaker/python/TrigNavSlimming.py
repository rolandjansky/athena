# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: TrigNavSlimming.py 657471 2015-03-27 22:40:36Z ssnyder $
#
# The singleton in this module can be used to slim the trigger navigation
# in jobs running on MC AODs. MC AODs of the future (R18) will have their
# navigation slimmed by the production system, but for MC11 AODs it has
# to be done in the analysis job.

# Gaudi/Athena import(s):
from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
from RecExConfig.Configured import Configured

# D3PDMaker import(s):
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

##
# @short Class decorator for implementing the singleton pattern
#
# I just copy-pasted this code from the TauDiscriGetter code.
# The idea is the same. The navigation slimming should only be
# scheduled once into a job.
#
def singleton( cls ):

    __log = logging.getLogger( "%s::__init__" % cls.__name__ )
    __instances = {}

    def getinstance( *args, **kwargs ):

        # Check if the singleton has already been created:
        if cls in __instances:
            __log.debug( "Navigation slimming already configured" )
            return __instances[ cls ]

        # Create the singleton:
        obj = cls( *args, **kwargs )
        __instances[ cls ] = obj

        return obj

    return getinstance

##
# @short Singleton class setting up the navigation slimming
#
# This class is responsible for setting up the navigation slimming for
# D3PDMaker jobs.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#
# $Revision: 657471 $
# $Date: 2015-03-27 23:40:36 +0100 (Fri, 27 Mar 2015) $
@singleton
class TrigNavSlimming( Configured ):

    ##
    # The default sequence to add this algorithm to is the TopAlg sequence
    # now. As it turns out, some D3PDMaker jobs run algorithms outside of the
    # D3PDMaker pre-sequence, so the slimming should really be done as the
    # very first thing in the event.
    def __init__( self, name = "TrigNavSlimming",
                  sequence = AlgSequence() ):

        # Remember the parameters:
        self.__sequence = sequence
        self.__name = name

        Configured.__init__( self )

    def configure( self ):

        # Don't do anything for data jobs:
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource() == "data":
            return True

        # Also check that this is not a reconstruction job, as the
        # slimming could mess up such a job very badly.
        from RecExConfig.RecFlags import rec
        if rec.doESD() or rec.doAOD():
            return True

        # Let the user know what we're doing:
        __log = logging.getLogger( "TrigNavSlimming" )
        __log.info( "Configuring Trigger Navigation Slimming" )

        # Construct the object/container names for which the navigation
        # should have references:
        from TrigEDMConfig.TriggerEDM import getTriggerObjList, TriggerL2List, TriggerEFList, TriggerHLTList, TriggerResultsList
        tlist = getTriggerObjList('AODFULL', [TriggerL2List, TriggerEFList, TriggerHLTList, TriggerResultsList])
        from RecExConfig.ObjKeyStore import cfgKeyStore
        objlist = []
        for t,kset in tlist.iteritems():
            for k in kset:
                __log.debug( "Checking for object %s#%s" % ( t, k ) )
                # Notice that we can't use the isInInput(...) function.
                # Calling getAODList(...) earlier adds all the trigger
                # objects to be available in the transient store. Even
                # though they won't be. This is why we can't use this
                # singleton in a reconstruction job.
                if cfgKeyStore.isInInputFile( t, k ):
                    objlist += [ "%s#%s" % ( t, k ) ]
                    pass
                pass
            pass
        __log.info( "Will retain references for objects: %s" % objlist )

        actions = ['SyncThinning', 'Reload']
        if objlist:
            actions.append ('DropFeatures')

        # Set up the slimming algorithm:
        from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimming, TrigNavigationThinningTool, HLT__TrigNavigationSlimmingTool

        from AthenaServices.AthenaServicesConf import ThinningSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr
        thinningSvc = ThinningSvc()
        ServiceMgr += thinningSvc
        navSlimmingTool = HLT__TrigNavigationSlimmingTool()
        navSlimmingTool.Actions = actions
        navSlimmingTool.ThinningSvc = thinningSvc
        navSlimmingTool.FeatureInclusionList = objlist

        
        navThinningTool =  TrigNavigationThinningTool()
        navThinningTool.ActInPlace=True
        navThinningTool.SlimmingTool = navSlimmingTool;
        navThinningTool.ThinningSvc = thinningSvc
        ToolSvc += navThinningTool

        slimmer = HLT__TrigNavigationSlimming()                
        slimmer.ThinningTool = navThinningTool        
                                        
        self.__sequence += slimmer

        # Signal that everything went okay:
        return True
