# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Di-Photon Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain two
##              electrons which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain two 
photons which fulfill a certain set of criteria.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class DiPhotonFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain two 
    photons which fulfill a certain set of criteria. The possible
    criteria are (with default values):
    cutEtMin1    = 10.0*Units.GeV)
    cutEtMin2    = 10.0*Units.GeV)
    cutIsEM1     = 'None'    # Possible values: 'None', 'Photon'
    cutIsEM2     = 'None'    # Possible values: 'None', 'Photon'
    cutAuthor1   = 'Photon'  # Possible values: 'Photon'
    cutAuthor2   = 'Photon'  # Possible values: 'Photon'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52
    passAll        = False
    """



    def __init__ ( self, name = "DiPhotonFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(DiPhotonFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.photonCollectionType = kw.get('photonCollectionType', "PhotonContainer")
        self.photonCollectionName = kw.get('photonCollectionName', "PhotonAODCollection")
        self.cutEtMin1    = kw.get('cutEtMin1', 10.0*Units.GeV)
        self.cutEtMin2    = kw.get('cutEtMin2', 10.0*Units.GeV)
        self.cutIsEM1     = kw.get('cutIsEM1', "None") # Possible values: "None", "Photon"
        self.cutIsEM2     = kw.get('cutIsEM2', "None") # Possible values: "None", "Photon"
        self.cutAuthor1   = kw.get('cutAuthor1', "Photon") # Possible values: 'Photon'
        self.cutAuthor2   = kw.get('cutAuthor2', "Photon") # Possible values: 'Photon'
        self.cutEtaMax      = kw.get('cutEtaMax', 2.5)
        self.removeEtaCrack = kw.get('removeEtaCrack', False)
        self.crackEtaMin    = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax    = kw.get('crackEtaMax', 1.52)
        self.passAll        = kw.get('passAll', False)
        ## Get the storgate handle
        self.storeGateSvc   = None



    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  EtMinPh1       = %r', self.cutEtMin1 )
        self.msg.info( '  EtMinPh2       = %r', self.cutEtMin2 )
        self.msg.info( '  IsEmPh1        = %s', self.cutIsEM1 )
        self.msg.info( '  IsEmPh2        = %s', self.cutIsEM2 )
        self.msg.info( '  AuthorPh1      = %s', self.cutAuthor1 )
        self.msg.info( '  AuthorPh2      = %s', self.cutAuthor2 )
        self.msg.info( '  EtaMax         = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin    = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax    = %r', self.crackEtaMax )
        self.msg.info( '  passAll        = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed          = 0
        self.nPhotons            = 0
        self.nEventPh1PassEta    = 0
        self.nEventPh2PassEta    = 0
        self.nEventPh1PassEt     = 0
        self.nEventPh2PassEt     = 0
        self.nEventPh1PassIsEM   = 0
        self.nEventPh2PassIsEM   = 0
        self.nEventPh1PassAuthor = 0
        self.nEventPh2PassAuthor = 0

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
        
        nPh1PassEta    = 0
        nPh2PassEta    = 0
        nPh1PassEt     = 0
        nPh2PassEt     = 0
        nPh1PassIsEM   = 0
        nPh2PassIsEM   = 0
        nPh1PassAuthor = 0
        nPh2PassAuthor = 0

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        hasFirstPhoton  = False
        hasSecondPhoton = False
        photonCollection = None
        try:
            photonCollection = self.storeGateSvc.retrieve( self.photonCollectionType, self.photonCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.photonCollectionName )
            self.setFilterPassed(False)
            return StatusCode.Success


        for photon in photonCollection :
            self.nPhotons += 1
            if not hasFirstPhoton or not hasSecondPhoton:
                if abs(photon.eta()) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                    ( abs(photon.eta()) < self.crackEtaMin or abs(photon.eta()) > self.crackEtaMax ) ):
                        # Check if the higher Et photon is already found.
                        if not hasFirstPhoton :
                            nPh1PassEta += 1
                            # If the Et of this photon is larger than the minimum required Et
                            # of the high-Et cut, then the high-Et photon is found
                            if photon.et() > self.cutEtMin1 :
                                nPh1PassEt += 1
                                # Check that the photon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( photon, self.cutAuthor1 ) :
                                    nPh1PassAuthor += 1
                                    # Check the quality of the photon using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkPhotonIsEM( photon, self.cutIsEM1 ):
                                        nPh1PassIsEM += 1
                                        hasFirstPhoton = True
                            # If this photon doesn't pass the higher-Et cut, maybe it passes the
                            # lower-Et cut; but only if the lower Et photon is not found yet.
                            elif not hasSecondPhoton and photon.et() > self.cutEtMin2 :
                                nPh2PassEt += 1
                                nPh1PassEta -= 1
                                nPh2PassEta += 1
                                # Check that the photon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( photon, self.cutAuthor2 ) :
                                    self.nPh2PassAuthor += 1
                                    # Check the quality of the photon using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkPhotonIsEM( photon, self.cutIsEM2 ):
                                        nPh2PassIsEM += 1
                                        hasSecondPhoton = True
                        else :
                            nPh2PassEta += 1
                            if photon.et() > self.cutEtMin2 :
                                nPh2PassEt += 1
                                # Check that the photon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( photon, self.cutAuthor2 ) :
                                    nPh2PassAuthor += 1
                                    # Check the quality of the photon using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkPhotonIsEM( photon, self.cutIsEM2 ):
                                        nPh2PassIsEM += 1
                                        hasSecondPhoton = True

        ## Translate the photon pass counters into event pass counters
        if nPh1PassEta > 0 : self.nEventPh1PassEta += 1
        if nPh2PassEta > 0 : self.nEventPh2PassEta += 1
        if nPh1PassEt > 0 : self.nEventPh1PassEt += 1
        if nPh2PassEt > 0 : self.nEventPh2PassEt += 1
        if nPh1PassIsEM > 0 : self.nEventPh1PassIsEM += 1
        if nPh2PassIsEM > 0 : self.nEventPh2PassIsEM += 1
        if nPh1PassAuthor > 0 : self.nEventPh1PassAuthor += 1
        if nPh2PassAuthor > 0 : self.nEventPh2PassAuthor += 1

        ## Check if the event is accepted
        if hasFirstPhoton and hasSecondPhoton :
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
        else:
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiPhotons     = 0.0
        effiPh1PassEta    = 0.0
        effiPh1PassEt     = 0.0
        effiPh1PassAuthor = 0.0
        effiPh1PassIsEM   = 0.0
        effiPh2PassEta    = 0.0
        effiPh2PassEt     = 0.0
        effiPh2PassAuthor = 0.0
        effiPh2PassIsEM   = 0.0
        effiErrPhotons     = 0.0
        effiErrPh1PassEta    = 0.0
        effiErrPh1PassEt     = 0.0
        effiErrPh1PassAuthor = 0.0
        effiErrPh1PassIsEM   = 0.0
        effiErrPh2PassEta    = 0.0
        effiErrPh2PassEt     = 0.0
        effiErrPh2PassAuthor = 0.0
        effiErrPh2PassIsEM   = 0.0
        try :
            # Efficiencies
            effiPhotons       = self.nPhotons / (1.0*self.nProcessed)
            effiPh1PassEta    = 100.0*self.nEventPh1PassEta / self.nProcessed
            effiPh1PassEt     = 100.0*self.nEventPh1PassEt / self.nProcessed
            effiPh1PassAuthor = 100.0*self.nEventPh1PassAuthor / self.nProcessed
            effiPh1PassIsEM   = 100.0*self.nEventPh1PassIsEM / self.nProcessed
            effiPh2PassEta    = 100.0*self.nEventPh2PassEta / self.nProcessed
            effiPh2PassEt     = 100.0*self.nEventPh2PassEt / self.nProcessed
            effiPh2PassAuthor = 100.0*self.nEventPh2PassAuthor / self.nProcessed
            effiPh2PassIsEM   = 100.0*self.nEventPh2PassIsEM / self.nProcessed

            # Error on efficiencies
            effiErrPhotons       = math.sqrt(self.nPhotons) / self.nPhotons
            effiErrPh1PassEta    = 100 * math.sqrt(self.nEventPh1PassEta) / self.nProcessed
            effiErrPh1PassEt     = 100 * math.sqrt(self.nEventPh1PassEt) / self.nProcessed
            effiErrPh1PassAuthor = 100 * math.sqrt(self.nEventPh1PassAuthor) / self.nProcessed
            effiErrPh1PassIsEM   = 100 * math.sqrt(self.nEventPh1PassIsEM) / self.nProcessed
            effiErrPh2PassEta    = 100 * math.sqrt(self.nEventPh2PassEta) / self.nProcessed
            effiErrPh2PassEt     = 100 * math.sqrt(self.nEventPh2PassEt) / self.nProcessed
            effiErrPh2PassAuthor = 100 * math.sqrt(self.nEventPh2PassAuthor) / self.nProcessed
            effiErrPh2PassIsEM   = 100 * math.sqrt(self.nEventPh2PassIsEM) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all photons:    %r and number of photons per event: %3.3f +/- %3.3f' % ( self.nPhotons, effiPhotons, effiErrPhotons ) )
        self.msg.info( ' Events after first eta cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh1PassEta,    effiPh1PassEta,    effiErrPh1PassEta ) )
        self.msg.info( ' Events after first E_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh1PassEt,     effiPh1PassEt,     effiErrPh1PassEt ) )
        self.msg.info( ' Events after first author cut:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh1PassAuthor, effiPh1PassAuthor, effiErrPh1PassAuthor ) )
        self.msg.info( ' Events after first IsEM cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh1PassIsEM,   effiPh1PassIsEM,   effiErrPh1PassIsEM ) )
        self.msg.info( ' Events after second eta cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh2PassEta,    effiPh2PassEta,    effiErrPh2PassEta ) )
        self.msg.info( ' Events after second E_T cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh2PassEt,     effiPh2PassEt,     effiErrPh2PassEt ) )
        self.msg.info( ' Events after second author cut: %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh2PassAuthor, effiPh2PassAuthor, effiErrPh2PassAuthor ) )
        self.msg.info( ' Events after second IsEM cut:   %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPh2PassIsEM,   effiPh2PassIsEM,   effiErrPh2PassIsEM ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

