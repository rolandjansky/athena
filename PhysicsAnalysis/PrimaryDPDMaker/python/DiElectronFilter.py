# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Di-Electron Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain two
##              electrons which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain two 
electrons which fulfill a certain set of criteria.
"""
__version__ = "0.0.2"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units


class DiElectronFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain two 
    electrons which fulfill a certain set of criteria. The possible
    criteria are (with default values):
    cutEtMin1    = 8.0*Units.GeV)
    cutEtMin2    = 8.0*Units.GeV)
    cutIsEM1     = 'None'    # Possible values: 'None', 'Loose',
    cutIsEM2     = 'None'    # 'Medium', 'Tight', 'TightNoIsolation'
    cutAuthor1   = 'either'  # Possible values: 'softe', 'egamma', 'either'
    cutAuthor2   = 'either'  # Possible values: 'softe', 'egamma', 'either'
    cutEtaMax      = 2.5
    removeEtaCrack = False
    crackEtaMin    = 1.37
    crackEtaMax    = 1.52
    passAll        = False
    """



    def __init__ ( self, name = "DiElectronFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(DiElectronFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.electronCollectionType = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName = kw.get('electronCollectionName', "ElectronAODCollection")
        self.cutEtMin1    = kw.get('cutEtMin1', 8.0*Units.GeV)
        self.cutEtMin2    = kw.get('cutEtMin2', 8.0*Units.GeV)
        self.cutIsEM1     = kw.get('cutIsEM1', "None") # Possible values: "None", "Loose",
        self.cutIsEM2     = kw.get('cutIsEM2', "None") # "Medium", "Tight", "TightNoIsolation"
        self.cutAuthor1   = kw.get('cutAuthor1', "either") # Possible values: 'softe', 'egamma', 'either'
        self.cutAuthor2   = kw.get('cutAuthor2', "either") # Possible values: 'softe', 'egamma', 'either'
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
        self.msg.info( '  EtMinEl1       = %r', self.cutEtMin1 )
        self.msg.info( '  EtMinEl2       = %r', self.cutEtMin2 )
        self.msg.info( '  IsEmEl1        = %s', self.cutIsEM1 )
        self.msg.info( '  IsEmEl2        = %s', self.cutIsEM2 )
        self.msg.info( '  AuthorEl1      = %s', self.cutAuthor1 )
        self.msg.info( '  AuthorEl2      = %s', self.cutAuthor2 )
        self.msg.info( '  EtaMax         = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin    = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax    = %r', self.crackEtaMax )
        self.msg.info( '  passAll        = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed          = 0
        self.nElectrons          = 0
        self.nEventEl1PassEta    = 0
        self.nEventEl2PassEta    = 0
        self.nEventEl1PassEt     = 0
        self.nEventEl2PassEt     = 0
        self.nEventEl1PassIsEM   = 0
        self.nEventEl2PassIsEM   = 0
        self.nEventEl1PassAuthor = 0
        self.nEventEl2PassAuthor = 0

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
        
        nEl1PassEta    = 0
        nEl2PassEta    = 0
        nEl1PassEt     = 0
        nEl2PassEt     = 0
        nEl1PassIsEM   = 0
        nEl2PassIsEM   = 0
        nEl1PassAuthor = 0
        nEl2PassAuthor = 0

        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!
        hasFirstElectron  = False
        hasSecondElectron = False
        electronCollection = None
        try:
            electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success


        for electron in electronCollection :
            self.nElectrons += 1
            if not hasFirstElectron or not hasSecondElectron:
                if abs(electron.eta()) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                    ( abs(electron.eta()) < self.crackEtaMin or abs(electron.eta()) > self.crackEtaMax ) ):
                        # Check if the higher Et electron is already found.
                        if not hasFirstElectron :
                            nEl1PassEta += 1
                            # If the Et of this electron is larger than the minimum required Et
                            # of the high-Et cut, then the high-Et electron is found
                            if electron.et() > self.cutEtMin1 :
                                nEl1PassEt += 1
                                # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutAuthor1 ) :
                                    nEl1PassAuthor += 1
                                    # Check the quality of the electron using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM1 ):
                                        nEl1PassIsEM += 1
                                        hasFirstElectron = True
                            # If this electron doesn't pass the higher-Et cut, maybe it passes the
                            # lower-Et cut; but only if the lower Et electron is not found yet.
                            elif not hasSecondElectron and electron.et() > self.cutEtMin2 :
                                nEl2PassEt += 1
                                nEl1PassEta -= 1
                                nEl2PassEta += 1
                                # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutAuthor2 ) :
                                    self.nEl2PassAuthor += 1
                                    # Check the quality of the electron using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM2 ):
                                        nEl2PassIsEM += 1
                                        hasSecondElectron = True
                        else :
                            nEl2PassEta += 1
                            if electron.et() > self.cutEtMin2 :
                                nEl2PassEt += 1
                                # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutAuthor2 ) :
                                    nEl2PassAuthor += 1
                                    # Check the quality of the electron using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM2 ):
                                        nEl2PassIsEM += 1
                                        hasSecondElectron = True

        ## Translate the electron pass counters into event pass counters
        if nEl1PassEta > 0 : self.nEventEl1PassEta += 1
        if nEl2PassEta > 0 : self.nEventEl2PassEta += 1
        if nEl1PassEt > 0 : self.nEventEl1PassEt += 1
        if nEl2PassEt > 0 : self.nEventEl2PassEt += 1
        if nEl1PassIsEM > 0 : self.nEventEl1PassIsEM += 1
        if nEl2PassIsEM > 0 : self.nEventEl2PassIsEM += 1
        if nEl1PassAuthor > 0 : self.nEventEl1PassAuthor += 1
        if nEl2PassAuthor > 0 : self.nEventEl2PassAuthor += 1

        ## Check if the event is accepted
        if hasFirstElectron and hasSecondElectron :
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
        effiEl1PassEta    = 0.0
        effiEl1PassEt     = 0.0
        effiEl1PassAuthor = 0.0
        effiEl1PassIsEM   = 0.0
        effiEl2PassEta    = 0.0
        effiEl2PassEt     = 0.0
        effiEl2PassAuthor = 0.0
        effiEl2PassIsEM   = 0.0
        effiErrElectrons     = 0.0
        effiErrEl1PassEta    = 0.0
        effiErrEl1PassEt     = 0.0
        effiErrEl1PassAuthor = 0.0
        effiErrEl1PassIsEM   = 0.0
        effiErrEl2PassEta    = 0.0
        effiErrEl2PassEt     = 0.0
        effiErrEl2PassAuthor = 0.0
        effiErrEl2PassIsEM   = 0.0
        try :
            # Efficiencies
            effiElectrons     = self.nElectrons / (1.0*self.nProcessed)
            effiEl1PassEta    = 100.0*self.nEventEl1PassEta / self.nProcessed
            effiEl1PassEt     = 100.0*self.nEventEl1PassEt / self.nProcessed
            effiEl1PassAuthor = 100.0*self.nEventEl1PassAuthor / self.nProcessed
            effiEl1PassIsEM   = 100.0*self.nEventEl1PassIsEM / self.nProcessed
            effiEl2PassEta    = 100.0*self.nEventEl2PassEta / self.nProcessed
            effiEl2PassEt     = 100.0*self.nEventEl2PassEt / self.nProcessed
            effiEl2PassAuthor = 100.0*self.nEventEl2PassAuthor / self.nProcessed
            effiEl2PassIsEM   = 100.0*self.nEventEl2PassIsEM / self.nProcessed

            # Error on efficiencies
            effiErrElectrons     = math.sqrt(self.nElectrons) / self.nElectrons
            effiErrEl1PassEta    = 100 * math.sqrt(self.nEventEl1PassEta) / self.nProcessed
            effiErrEl1PassEt     = 100 * math.sqrt(self.nEventEl1PassEt) / self.nProcessed
            effiErrEl1PassAuthor = 100 * math.sqrt(self.nEventEl1PassAuthor) / self.nProcessed
            effiErrEl1PassIsEM   = 100 * math.sqrt(self.nEventEl1PassIsEM) / self.nProcessed
            effiErrEl2PassEta    = 100 * math.sqrt(self.nEventEl2PassEta) / self.nProcessed
            effiErrEl2PassEt     = 100 * math.sqrt(self.nEventEl2PassEt) / self.nProcessed
            effiErrEl2PassAuthor = 100 * math.sqrt(self.nEventEl2PassAuthor) / self.nProcessed
            effiErrEl2PassIsEM   = 100 * math.sqrt(self.nEventEl2PassIsEM) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all electrons:    %r and number of electrons per event: %3.3f +/- %3.3f' % ( self.nElectrons, effiElectrons, effiErrElectrons ) )
        self.msg.info( ' Events after first eta cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl1PassEta,    effiEl1PassEta,    effiErrEl1PassEta ) )
        self.msg.info( ' Events after first E_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl1PassEt,     effiEl1PassEt,     effiErrEl1PassEt ) )
        self.msg.info( ' Events after first author cut:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl1PassAuthor, effiEl1PassAuthor, effiErrEl1PassAuthor ) )
        self.msg.info( ' Events after first IsEM cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl1PassIsEM,   effiEl1PassIsEM,   effiErrEl1PassIsEM ) )
        self.msg.info( ' Events after second eta cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl2PassEta,    effiEl2PassEta,    effiErrEl2PassEta ) )
        self.msg.info( ' Events after second E_T cut:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl2PassEt,     effiEl2PassEt,     effiErrEl2PassEt ) )
        self.msg.info( ' Events after second author cut: %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl2PassAuthor, effiEl2PassAuthor, effiErrEl2PassAuthor ) )
        self.msg.info( ' Events after second IsEM cut:   %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventEl2PassIsEM,   effiEl2PassIsEM,   effiErrEl2PassIsEM ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

