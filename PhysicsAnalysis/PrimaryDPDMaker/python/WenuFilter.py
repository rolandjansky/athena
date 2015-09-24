# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        W electron nu Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain 
##              W -> e nu.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain W -> e nu.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class WenuFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain W -> e nu.
    The possible criteria are (with default values):
    cutMissingEt   = 20.0*Units.GeV
    cutSumEt       = 999999.0*Units.GeV
    cutEtMin     = 12.0*Units.GeV
    cutIsEM      = 'None'    # Possible values: 'None', 'Loose', 'Medium', 'Tight', 'TightNoIsolation'
    cutAuthor    = 'either'  # Possible values: 'softe', 'egamma', 'either'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52
    passAll        = False
    """



    def __init__ ( self, name = "WenuFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(WenuFilter, self).__init__(**kw)

        ## Define the cuts
        self.electronCollectionType  = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName  = kw.get('electronCollectionName', "ElectronAODCollection")
        self.missingEtCollectionType = kw.get('missingEtCollectionType', "MissingET")
        self.missingEtCollectionName = kw.get('missingEtCollectionName', "MET_RefFinal")
        self.cutMissingEt   = kw.get('cutMisingEt', 20.0*Units.GeV)
        self.cutSumEt       = kw.get('cutSumEt', 999999.0*Units.GeV)
        self.cutEtMin     = kw.get('cutEtMin', 12.0*Units.GeV)
        self.cutIsEM      = kw.get('cutIsEM', "None") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.cutAuthor    = kw.get('cutAuthor', "either") # Possible values: 'softe', 'egamma', 'either'
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
        self.msg.info( '  MissingEt      = %r', self.cutMissingEt )
        self.msg.info( '  SumEt          = %r', self.cutSumEt )
        self.msg.info( '  EtMinEl        = %r', self.cutEtMin )
        self.msg.info( '  IsEmEl         = %s', self.cutIsEM )
        self.msg.info( '  AuthorEl       = %s', self.cutAuthor )
        self.msg.info( '  EtaMax         = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin    = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax    = %r', self.crackEtaMax )
        self.msg.info( '  passAll        = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed          = 0
        self.nEventPassMissingEt = 0
        self.nEventPassSumEt     = 0
        self.nElectrons          = 0
        self.nEventElPassEta     = 0
        self.nEventElPassEt      = 0
        self.nEventElPassIsEM    = 0
        self.nEventElPassAuthor  = 0

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
        
        nElPassEta    = 0
        nElPassEt     = 0
        nElPassIsEM   = 0
        nElPassAuthor = 0

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        hasWenu  = False
        try:
            met = self.storeGateSvc.retrieve( self.missingEtCollectionType, self.missingEtCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.missingEtCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success

        missingEt = met.et()
        sumEt = met.sumet()
        if missingEt > self.cutMissingEt :
            self.nEventPassMissingEt += 1
            if sumEt < self.cutSumEt :
                self.nEventPassSumEt += 1
                try :
                    electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
                except LookupError:
                    self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
                    self.setFilterPassed(False)
                    return StatusCode.Success
                
                for electron in electronCollection :
                    self.nElectrons += 1
                    if not hasWenu :
                        if abs(electron.eta()) < self.cutEtaMax :
                            # Check if the EM calorimeter barrel-encap crack should be removed.
                            if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                            ( abs(electron.eta()) < self.crackEtaMin or abs(electron.eta()) > self.crackEtaMax ) ):
                                nElPassEta += 1
                                # If the Et of this electron is larger than the minimum required Et
                                # of the high-Et cut, then the high-Et electron is found
                                if electron.et() > self.cutEtMin :
                                    nElPassEt += 1
                                    # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutAuthor ) :
                                        nElPassAuthor += 1
                                        # Check the quality of the electron using PrimaryDPDHelpers
                                        if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM ):
                                            nElPassIsEM += 1
                                            hasWenu = True


        ## Translate the electron pass counters into event pass counters
        if nElPassEta > 0 : self.nEventElPassEta += 1
        if nElPassEt > 0 : self.nEventElPassEt += 1
        if nElPassIsEM > 0 : self.nEventElPassIsEM += 1
        if nElPassAuthor > 0 : self.nEventElPassAuthor += 1

        ## Check if the event is accepted
        if hasWenu :
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
        effiElectrons     = 0.0
        effiPassMissingEt = 0.0
        effiPassSumEt     = 0.0
        effiElPassEta     = 0.0
        effiElPassEt      = 0.0
        effiElPassAuthor  = 0.0
        effiElPassIsEM    = 0.0
        effiErrElectrons     = 0.0
        effiErrPassMissingEt = 0.0
        effiErrPassSumEt     = 0.0
        effiErrElPassEta     = 0.0
        effiErrElPassEt      = 0.0
        effiErrElPassAuthor  = 0.0
        effiErrElPassIsEM    = 0.0
        try :
            # Efficiencies
            effiElectrons     = self.nElectrons / (1.0*self.nProcessed)
            effiPassMissingEt = 100.0*self.nEventPassMissingEt / self.nProcessed
            effiPassSumEt     = 100.0*self.nEventPassSumEt / self.nProcessed
            effiElPassEta     = 100.0*self.nEventElPassEta / self.nProcessed
            effiElPassEt      = 100.0*self.nEventElPassEt / self.nProcessed
            effiElPassAuthor  = 100.0*self.nEventElPassAuthor / self.nProcessed
            effiElPassIsEM    = 100.0*self.nEventElPassIsEM / self.nProcessed

            # Error on efficiencies
            effiErrElectrons     = math.sqrt(self.nElectrons) / self.nElectrons
            effiErrPassMissingEt = 100.0 * math.sqrt(self.nEventPassMissingEt) / self.nProcessed
            effiErrPassSumEt     = 100.0 * math.sqrt(self.nEventPassSumEt) / self.nProcessed
            effiErrElPassEta     = 100.0 * math.sqrt(self.nEventElPassEta) / self.nProcessed
            effiErrElPassEt      = 100.0 * math.sqrt(self.nEventElPassEt) / self.nProcessed
            effiErrElPassAuthor  = 100.0 * math.sqrt(self.nEventElPassAuthor) / self.nProcessed
            effiErrElPassIsEM    = 100.0 * math.sqrt(self.nEventElPassIsEM) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all electrons:  %r and number of electrons per event: %3.3f +/- %3.3f' % ( self.nElectrons, effiElectrons, effiErrElectrons ) )
        self.msg.info( ' Events after missing E_T cut: %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassMissingEt, effiPassMissingEt, effiErrPassMissingEt ) )
        self.msg.info( ' Events after sum E_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassSumEt,     effiPassSumEt,     effiErrPassSumEt ) )
        self.msg.info( ' Events after eta cut:         %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassEta,     effiElPassEta,     effiErrElPassEta ) )
        self.msg.info( ' Events after E_T cut:         %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassEt,      effiElPassEt,      effiErrElPassEt ) )
        self.msg.info( ' Events after author cut:      %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassAuthor,  effiElPassAuthor,  effiErrElPassAuthor ) )
        self.msg.info( ' Events after IsEM cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassIsEM,    effiElPassIsEM,    effiErrElPassIsEM ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

