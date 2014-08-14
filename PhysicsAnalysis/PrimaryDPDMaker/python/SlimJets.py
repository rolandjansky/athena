# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name: SlimJets.py   
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## 
## Based on: DPDUtils
##
## Description: Remove information from the used jets.
##              Remove: Links to Tracks
##                      All b-tagging keys but for the three standard ones.
##=============================================================================


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class SlimJets( PyAthena.Alg ):
    """
    This class is able to remove inforamtion from jets (slimming).
    The JetCollection type should be given and also a list of
    JetCollectionNames:

    """


    
    def __init__ ( self, name = "SlimJets", **kw ) :
        ## initialize base class
        kw['name'] = name
        super(SlimJets, self).__init__(**kw)

        # Get the configuration for this class
        self.jetCollectionType  = kw.get('jetCollectionType', "JetCollection")
        self.jetCollectionNames = kw.get('jetCollectionNames', [])
        self.removeTracks       = kw.get('removeTracks', True )
        self.removeInfoKeys     = kw.get('removeInfoKeys', False )
        self.keepInfoKeys       = kw.get('keepInfoKeys', ["IP2D", "IP3D", "SV1"] )
        self.storeGateSvc       = None

        return

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...' % self.name() )
        self.msg.info( 'Will apply the following selections:' )
        self.msg.info( '  using jetCollectionType       = %r', self.jetCollectionType )
        self.msg.info( '  using jetCollectionNames      = %r', self.jetCollectionNames )
        self.msg.info( '  going to remove tracks        = %r', self.removeTracks )
        self.msg.info( '  going to remove InfoKeys      = %r', self.removeInfoKeys )
        self.msg.info( '  ...but keeping these InfoKeys = %r', self.keepInfoKeys )

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        return StatusCode.Success


    def slim(self, jets):
        self.msg.debug("now in slim")
        for jet in jets:
            ## Remove links to tracks
            if self.removeTracks :
                try :
                    jet.constituent("Tracks").removeAll() # this works with release 13 ParticleJets
                except AttributeError :
                    jet.removeAssociation("Tracks") # this works with new jets, starting with release 14.1.0

            # Remove the jet tagging keys that are not needed
            if self.removeInfoKeys :
                map(jet.removeInfo, (k for k in jet.infoKeys()
                                     if k not in self.keepInfoKeys))
##                for key in jet.infoKeys():
##                    # Remove all keys but for the three standard b-tagging ones
##                    if not key in self.keepInfoKeys :
##                        jet.removeInfo(key)

        return


    def execute(self):
        self.msg.debug("now in execute %s" % self.jetCollectionNames )
        ## Depending on whether the input has truth or not:
        for jetCollName in self.jetCollectionNames :

            # Get the jet collections from StoreGate
            try :
                # try to get the jet collection the user wants
                jetCollection = self.storeGateSvc.retrieve( self.jetCollectionType, jetCollName )
            except LookupError :
                # If that didn't work, try to analyze if the user wanted the old ParticleJets
                # or the new (starting from release 14.1.0) Jets. Also cover the case that the
                # user gave the wrong type for the given input file.
                if jetCollName.__contains__('Particle') : # user gave old ParticleJets
                    try :
                        jetCollection = self.storeGateSvc.retrieve( 'ParticleJetContainer', jetCollName )
                        pass
                    except LookupError :
                        jetCollection = self.storeGateSvc.retrieve( 'JetCollection', jetCollName.replace('Particle','') )
                        self.msg.warning( 'Collection %s not found' % jetCollName )           
                        return StatusCode.Success
                else : # User did NOT give old ParticleJets, assume new Jets are given.
                    try :
                        jetCollection = self.storeGateSvc.retrieve( 'JetCollection', jetCollName )
                    except LookupError :
                        jetCollection = self.storeGateSvc.retrieve( 'ParticleJetContainer', jetCollName.replace('Jets','ParticleJets') )
                        self.msg.warning( 'Collection %s not found' % jetCollName )           
                        return StatusCode.Success
                    pass
                pass
 
            self.slim( jetCollection )

        return StatusCode.Success


    def finalize(self):
        return StatusCode.Success

