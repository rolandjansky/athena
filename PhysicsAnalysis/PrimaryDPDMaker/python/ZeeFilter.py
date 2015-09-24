# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Z -> ee Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain Z -> ee
##              which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain Z -> ee
which fulfill a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

import math


class ZeeFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain Z -> ee
    which fulfill a certain set of criteria. The possible
    criteria are (with default values):

    the type of electrons by its jetCollectionName:
    electronCollectionType  = 'ElectronContainer'
    electronCollectionName  = 'ElectronAODCollection'
    
    minimum E_T of both electrons:
    cutEtMin                = 15.0*Units.GeV 

    maximum |eta| of both electrons:
    cutEtaMax               = 2.5 

    author selection of both electrons:
    cutAuthor             = 'egamma'

    IsEM selectrion of both electrons:
    cutIsEMEl               = 'Medium'
    
    di-electron mass lower bound:
    cutDiElMassLower        = 60.0*Units.GeV

    di-electron mass upper bound:
    cutDiElMassUpper        = 120.0*Units.GeV

    pass all events if true:
    passAll                 = False
    """



    def __init__ ( self, name = "ZeeFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(ZeeFilter, self).__init__(**kw)

        # Define the cuts
        self.electronCollectionType = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName = kw.get('electronCollectionName', "ElectronAODCollection")
        self.cutEtMin         = kw.get('cutEtMin', 15.0*Units.GeV)
        self.cutEtaMax        = kw.get('cutEtaMax', 2.5)
        self.cutAuthor        = kw.get('cutAuthor', "egamma") # Possible values: 'softe', 'egamma', 'either'
        self.cutIsEM          = kw.get('cutIsEM', "Medium") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.useEl1Cuts       = kw.get('useEl1Cuts', False)
        self.cutEl1EtMin      = kw.get('cutEl1EtMin', 0.0*Units.GeV)
        self.cutEl1EtaMax     = kw.get('cutEl1EtaMax', 1000.0)
        self.cutEl1Author     = kw.get('cutEl1Author', "either" )
        self.cutEl1IsEM       = kw.get('cutEl1IsEM', "None") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.removeEtaCrack   = kw.get('removeEtaCrack', False)
        self.crackEtaMin      = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax      = kw.get('crackEtaMax', 1.52)
        self.cutDiElMassLower = kw.get('cutDiElMassLower', 60.0*Units.GeV)
        self.cutDiElMassUpper = kw.get('cutDiElMassUpper', 120.0*Units.GeV)
        self.passAll          = kw.get('passAll', False)
        self.storeGateSvc     = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using electron collection  = %r %r' % (self.electronCollectionType, self.electronCollectionName) )
        self.msg.info( '  cutEtMin                   = %r', self.cutEtMin )
        self.msg.info( '  cutEtaMax                  = %r', self.cutEtaMax )
        self.msg.info( '  cutIsEmEl                  = %s', self.cutIsEM )
        self.msg.info( '  cutAuthorEl                = %s', self.cutAuthor )
        self.msg.info( '  useEl1Cuts                 = %r', self.useEl1Cuts )
        self.msg.info( '  cutEl1EtMin                = %r', self.cutEl1EtMin )
        self.msg.info( '  cutEl1EtaMax               = %r', self.cutEl1EtaMax )
        self.msg.info( '  cutEl1IsEmEl               = %s', self.cutEl1IsEM )
        self.msg.info( '  cutEl1AuthorEl             = %s', self.cutEl1Author )
        self.msg.info( '  cutDiElMassLower           = %r', self.cutDiElMassLower )
        self.msg.info( '  cutDiElMassUpper           = %r', self.cutDiElMassUpper )
        self.msg.info( '  removeEtaCrack             = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin                = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax                = %r', self.crackEtaMax )
        self.msg.info( '  passAll                    = %r', self.passAll )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed                = 0
        self.nElectrons                = 0
        self.nGoodElectrons            = 0
        self.nEventPassEta             = 0
        self.nEventPassEt              = 0
        self.nEventPassAuthor          = 0
        self.nEventPassIsEM            = 0
        self.nEventPassDiElectronMass  = 0

        # Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        # Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        from ROOT import TLorentzVector
        nPassEta            = 0
        nPassEt             = 0
        nPassAuthor         = 0
        nPassIsEM           = 0
        nPassDiElectronMass = 0

        # If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Then... here we go!
        acceptEvent  = False
        # Get the electron collections from StoreGate
        try:
            electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success



        #Look for the El1 electron that has another selection
        electron1List = []
        if self.useEl1Cuts :
            for i in range( electronCollection.size() ) :
                electron1 = electronCollection[i]
                # Check if the eta requirements are fullfilled
                if abs(electron1.eta()) < self.cutEl1EtaMax :
                    if electron1.et() > self.cutEl1EtMin  :
                        # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                        if PrimaryDPDHelpers.checkEgammaAuthor( electron1, self.cutEl1Author ):
                            # Check the quality of the electron using PrimaryDPDHelpers
                            if PrimaryDPDHelpers.checkElectronIsEM( electron1, self.cutEl1IsEM ) :
                                electron1List.append( electron1 )
                            pass
                        pass
                    pass
                pass # End: loop over electrons
            pass
        else :
            electron1List = electronCollection
            pass
        



##        #look for two electrons 
##        for i in range( electronCollection.size() ) :
##            self.nElectrons += 1
##            for j in range( electronCollection.size() ) :
##                if j>i and not acceptEvent :
##                    electron1 = electronCollection[i]
##                    electron2 = electronCollection[j]

        #look for two electrons 
        for electron2 in electronCollection :
            self.nElectrons += 1
            for electron1 in electron1List :
                if not acceptEvent and ( electron1.getAthenaBarCode() != electron2.getAthenaBarCode() )  :
                    # Check if the eta requirements are fullfilled
                    if abs(electron1.eta()) < self.cutEtaMax and abs(electron2.eta()) < self.cutEtaMax :
                        # Check if the EM calorimeter barrel-encap crack should be removed.
                        if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                        ( abs(electron1.eta()) < self.crackEtaMin or abs(electron1.eta()) > self.crackEtaMax ) and \
                                                        ( abs(electron2.eta()) < self.crackEtaMin or abs(electron2.eta()) > self.crackEtaMax )  ) :
                            #Update event counter...
                            nPassEta += 1
                            if electron1.et() > self.cutEtMin and electron2.et() > self.cutEtMin :
                                #Update event counter...
                                nPassEt += 1

                                # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkEgammaAuthor( electron1, self.cutAuthor ) and \
                                       PrimaryDPDHelpers.checkEgammaAuthor( electron2, self.cutAuthor ):
                                    nPassAuthor += 1
                                    # Check the quality of the electron using PrimaryDPDHelpers
                                    if PrimaryDPDHelpers.checkElectronIsEM( electron1, self.cutIsEM ) and \
                                           PrimaryDPDHelpers.checkElectronIsEM( electron2, self.cutIsEM ) :
                                        nPassIsEM += 1                                    
                                        self.nGoodElectrons += 1

                                        # Calculate the di-jet invariant mass using TLorentzVectors from ROOT
                                        electronTLVec1 = TLorentzVector( electron1.px(), electron1.py(), electron1.pz(), electron1.e() )
                                        electronTLVec2 = TLorentzVector( electron2.px(), electron2.py(), electron2.pz(), electron2.e() )
                                        electronelectronTLVec = electronTLVec1 + electronTLVec2
                                        diElectronMass = electronelectronTLVec.M()

                                        # Select events inside the invariant di-electron mass window
                                        if diElectronMass > self.cutDiElMassLower and diElectronMass < self.cutDiElMassUpper :
                                            nPassDiElectronMass += 1
                                            acceptEvent = True
                                            pass  # End: Check DiElectronMass 
                                        pass  # End: Check IsEM
                                    pass  # End: Check Author
                                pass  # End: Check Et
                            pass  # End: Check eta crack removal
                        pass  # End: Check eta acceptance
                    pass  # End: if j>i and not acceptEvent
                pass  # End: for j in range( electronCollection.size() )
            pass  # End: for i in range( electronCollection.size() )


        ## Translate the electron pass counters into event pass counters
        if nPassEta > 0 :            self.nEventPassEta            += 1
        if nPassEt > 0 :             self.nEventPassEt             += 1
        if nPassAuthor > 0 :         self.nEventPassAuthor         += 1
        if nPassIsEM > 0 :           self.nEventPassIsEM           += 1
        if nPassDiElectronMass > 0 : self.nEventPassDiElectronMass += 1

        ## Check if the event is accepted
        if acceptEvent :
            self.msg.debug( '%s event passed' % self.name() )
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
        effiElectrons             = 0.0
        effiGoodElectrons         = 0.0
        effiPassEta               = 0.0
        effiPassEt                = 0.0
        effiPassAuthor            = 0.0
        effiPassIsEM              = 0.0
        effiPassDiElectronMass    = 0.0
        effiErrElectrons          = 0.0
        effiErrGoodElectrons      = 0.0
        effiErrPassEta            = 0.0
        effiErrPassEt             = 0.0
        effiErrPassAuthor         = 0.0
        effiErrPassIsEM           = 0.0
        effiErrPassDiElectronMass = 0.0
        try :
            # Efficiencies
            effiElectrons          = self.nElectrons / (1.0*self.nProcessed)
            effiGoodElectrons      = self.nGoodElectrons / (1.0*self.nProcessed)
            effiPassEta            = 100.0*self.nEventPassEta / self.nProcessed
            effiPassEt             = 100.0*self.nEventPassEt / self.nProcessed
            effiPassAuthor         = 100.0*self.nEventPassAuthor / self.nProcessed
            effiPassIsEM           = 100.0*self.nEventPassIsEM / self.nProcessed
            effiPassDiElectronMass = 100.0*self.nEventPassDiElectronMass / self.nProcessed

            # Error on efficiencies
            effiErrElectrons          = math.sqrt(self.nElectrons) / self.nElectrons
            effiErrGoodElectrons      = math.sqrt(self.nGoodElectrons) / self.nGoodElectrons
            effiErrPassEta            = 100.0 * math.sqrt(self.nEventPassEta) / self.nProcessed
            effiErrPassEt             = 100.0 * math.sqrt(self.nEventPassEt) / self.nProcessed
            effiErrPassAuthor         = 100.0 * math.sqrt(self.nEventPassAuthor) / self.nProcessed
            effiErrPassIsEM           = 100.0 * math.sqrt(self.nEventPassIsEM) / self.nProcessed
            effiErrPassDiElectronMass = 100.0 * math.sqrt(self.nEventPassDiElectronMass) / self.nProcessed
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all electrons:                     %r and number of electrons per event: %3.3f +/- %3.3f'  % ( self.nElectrons,               effiElectrons,          effiErrElectrons ) )
        self.msg.info( ' Number of electrons after E_T and eta cuts:  %r and number of electrons per event: %3.3f +/- %3.3f'  % ( self.nGoodElectrons,           effiGoodElectrons,      effiErrGoodElectrons ) )
        self.msg.info( ' Events after eta cut:                        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEta,            effiPassEta,            effiErrPassEta ) )
        self.msg.info( ' Events after E_T cut:                        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEt,             effiPassEt,             effiErrPassEt ) )
        self.msg.info( ' Events after electron author requirement:    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassAuthor,         effiPassAuthor,         effiErrPassAuthor ) )
        self.msg.info( ' Events after electron IsEM requirement:      %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassIsEM,           effiPassIsEM,           effiErrPassIsEM ) )
        self.msg.info( ' Events after di-electron invariant mass cut: %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassDiElectronMass, effiPassDiElectronMass, effiErrPassDiElectronMass ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

