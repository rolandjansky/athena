# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Photon Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain one
##              photon which fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
photon which fulfills a certain set of criteria.
"""
__version__ = "0.0.3"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class PhotonFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain one 
    photon which fulfills a certain set of criteria. The possible
    criteria are (with default values):
    goodPhotonCollectionName
    cutEtMin         = 20.0*Units.GeV
    cutIsEM          = 'None'    # Possible values: 'None', 'Photon'
    cutAuthor        = 'Photon'  # Possible values: 'Photon'
    cutEtaMax        = 2.5
    removeEtaCrack   = False
    crackEtaMin      = 1.37
    crackEtaMax      = 1.52
    passAll          = False
    """



    def __init__ ( self, name = "PhotonFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(PhotonFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.photonCollectionType     = kw.get('photonCollectionType', "PhotonContainer")
        self.photonCollectionName     = kw.get('photonCollectionName', "PhotonAODCollection")
        self.goodPhotonCollectionName = kw.get('goodPhotonCollectionName', self.name() )
        self.cutEtMin                 = kw.get('cutEtMin', 20.0*Units.GeV)
        self.cutIsEM                  = kw.get('cutIsEM', "None") # Possible values: "None", "Photon"
        self.cutAuthor                = kw.get('cutAuthor', "either") # Possible values: 'Photon'
        self.cutEtaMax                = kw.get('cutEtaMax', 2.5)
        self.removeEtaCrack           = kw.get('removeEtaCrack', False)
        self.crackEtaMin              = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax              = kw.get('crackEtaMax', 1.52)
        self.minNumberPassed          = kw.get('minNumberPassed', 1)
        self.passAll                  = kw.get('passAll', False)
        ## Get the storgate handle
        self.storeGateSvc = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  goodPhotonCollectionName = %r', self.goodPhotonCollectionName )
        self.msg.info( '  EtMinPh                  = %r', self.cutEtMin )
        self.msg.info( '  IsEmPh                   = %s', self.cutIsEM )
        self.msg.info( '  AuthorPh                 = %s', self.cutAuthor )
        self.msg.info( '  EtaMax                   = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack           = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin              = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax              = %r', self.crackEtaMax )
        self.msg.info( '  minNumberPassed          = %r', self.minNumberPassed )
        self.msg.info( '  passAll                  = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nPhotons           = 0
        self.nEventPhPassEta    = 0
        self.nEventPhPassEt     = 0
        self.nEventPhPassIsEM   = 0
        self.nEventPhPassAuthor = 0
        self.nEventMinNumPassed = 0
        
        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        nPhPassEta    = 0
        nPhPassEt     = 0
        nPhPassIsEM   = 0
        nPhPassAuthor = 0

        # Create a class-member list of all photons that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodPhotons = PyAthena.PhotonContainer(VIEW_ELEMENTS)
        #goodPhotons = []

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        photonCollection = None
        try:
            photonCollection = self.storeGateSvc.retrieve( self.photonCollectionType, self.photonCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.photonCollectionName )           
            self.setFilterPassed(True)
            return StatusCode.Success

        for photon in photonCollection :
            self.nPhotons += 1
            # Use Et of cluster.
            cluster = None
            cluster = photon.cluster()
            if cluster == None :
                self.msg.warning( 'Could not get the cluster for this photon!' )
                pass
            else :
                if abs(cluster.etaBE(2)) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                    ( abs(cluster.etaBE(2)) < self.crackEtaMin
                                                      or abs(cluster.etaBE(2)) > self.crackEtaMax ) ):
                        nPhPassEta += 1
                        # If the Et of this photon is larger than the minimum required Et
                        # of the high-Et cut, then the high-Et photon is found.
                        if photon.cluster().et() > self.cutEtMin :
                            nPhPassEt += 1
                            # Check that the photon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                            if PrimaryDPDHelpers.checkEgammaAuthor( photon, self.cutAuthor ) :
                                nPhPassAuthor += 1
                                # Check the quality of the photon using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkPhotonIsEM( photon, self.cutIsEM ):
                                    nPhPassIsEM += 1
                                    hasPhoton = True
                                    goodPhotons.push_back( photon )
                                    pass
                                pass
                            pass
                        pass
                    pass
                pass
            pass



        ## Translate the photon pass counters into event pass counters
        if nPhPassEta > 0 : self.nEventPhPassEta += 1
        if nPhPassEt > 0 : self.nEventPhPassEt += 1
        if nPhPassIsEM > 0 : self.nEventPhPassIsEM += 1
        if nPhPassAuthor > 0 : self.nEventPhPassAuthor += 1


        ## Record the good photons into StoreGate so that they can be retrieved by other algorithms
        if self.storeGateSvc.record( goodPhotons, self.goodPhotonCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodPhotons into StoreGate with the key = ' % self.goodPhotonCollectionName )
            pass


        ## Check if the event is accepted
        if goodPhotons.__len__() >= self.minNumberPassed :
            self.nEventMinNumPassed += 1
            self.msg.debug( '%s event passed.' % self.name() )
            self.setFilterPassed(True)
            pass
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass
        
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiPhotons     = 0.0
        effiPhPassEta    = 0.0
        effiPhPassEt     = 0.0
        effiPhPassAuthor = 0.0
        effiPhPassIsEM   = 0.0
        effiMinNumPassed = 0.0
        effiErrPhotons      = 0.0
        effiErrPhPassEta    = 0.0
        effiErrPhPassEt     = 0.0
        effiErrPhPassAuthor = 0.0
        effiErrPhPassIsEM   = 0.0
        effiErrMinNumPassed = 0.0

        try :
            # Efficiencies
            effiPhotons      = self.nPhotons / (1.0*self.nProcessed)
            effiPhPassEta    = self.nEventPhPassEta / float(self.nProcessed)
            effiPhPassEt     = self.nEventPhPassEt / float(self.nProcessed)
            effiPhPassAuthor = self.nEventPhPassAuthor / float(self.nProcessed)
            effiPhPassIsEM   = self.nEventPhPassIsEM / float(self.nProcessed)
            effiMinNumPassed = self.nEventMinNumPassed / float(self.nProcessed)

            # Error on efficiencies
            effiErrPhotons      = math.sqrt(self.nPhotons) / self.nPhotons
            effiErrPhPassEta    = 100.0 * math.sqrt( effiPhPassEta*(1.0 - effiPhPassEta) / float(self.nProcessed) )
            effiErrPhPassEt     = 100.0 * math.sqrt( effiPhPassEt*(1.0 - effiPhPassEt) / float(self.nProcessed) )
            effiErrPhPassAuthor = 100.0 * math.sqrt( effiPhPassAuthor*(1.0 - effiPhPassAuthor) / float(self.nProcessed) )
            effiErrPhPassIsEM   = 100.0 * math.sqrt( effiPhPassIsEM*(1.0 - effiPhPassIsEM) / float(self.nProcessed) )
            effiErrMinNumPassed = 100.0 * math.sqrt( effiMinNumPassed*(1.0 - effiMinNumPassed) / float(self.nProcessed) )
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass
        
        effiPhPassEta    *= 100.0
        effiPhPassEt     *= 100.0
        effiPhPassAuthor *= 100.0
        effiPhPassIsEM   *= 100.0
        effiMinNumPassed *= 100.0

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:  %r' % self.nProcessed )
        self.msg.info( ' Number of all photons:       %r and number of photons per event: %3.3f +/- %3.3f' % ( self.nPhotons, effiPhotons, effiErrPhotons ) )
        self.msg.info( ' Events after eta cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPhPassEta,    effiPhPassEta,    effiErrPhPassEta ) )
        self.msg.info( ' Events after E_T cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPhPassEt,     effiPhPassEt,     effiErrPhPassEt ) )
        self.msg.info( ' Events after author cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPhPassAuthor, effiPhPassAuthor, effiErrPhPassAuthor ) )
        self.msg.info( ' Events after IsEM cut:       %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPhPassIsEM,   effiPhPassIsEM,   effiErrPhPassIsEM ) )
        self.msg.info( ' Events after minNumberPassed %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMinNumPassed, effiMinNumPassed, effiErrMinNumPassed ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

