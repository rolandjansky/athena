# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Electron Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain one
##              electron which fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
electron which fulfills a certain set of criteria.
"""
__version__ = "0.0.3"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class ElectronFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain one 
    electron which fulfills a certain set of criteria. The possible
    criteria are (with default values):
    cutEtMin         = 20.0*Units.GeV)
    cutIsEM          = 'None'    # Possible values: 'None', 'Loose', 'Medium', 'Tight', 'TightNoIsolation'
    cutAuthor        = 'either'  # Possible values: 'softe', 'egamma', 'either'
    cutEtaMax        = 2.5
    removeEtaCrack   = False
    crackEtaMin      = 1.37
    crackEtaMax      = 1.52
    minNumberPassed  = 1
    passAll          = False
    """



    def __init__ ( self, name = "ElectronFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(ElectronFilter, self).__init__(**kw)

        ## Define the cuts
        self.electronCollectionType = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName = kw.get('electronCollectionName', "ElectronAODCollection")
        self.goodElectronCollectionName = kw.get('goodElectronCollectionName', self.name() )
        self.cutEtMin         = kw.get('cutEtMin', 20.0*Units.GeV)
        self.cutIsEM          = kw.get('cutIsEM', "None") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.cutAuthor        = kw.get('cutAuthor', "either") # Possible values: 'softe', 'egamma', 'either'
        self.cutEtaMax        = kw.get('cutEtaMax', 2.5)
        self.removeEtaCrack   = kw.get('removeEtaCrack', False)
        self.crackEtaMin      = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax      = kw.get('crackEtaMax', 1.52)
        self.minNumberPassed  = kw.get('minNumberPassed', 1)
        self.passAll          = kw.get('passAll', False)
        ## Get the storgate handle
        self.storeGateSvc     = None

        return


    

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  EtMinEl          = %r', self.cutEtMin )
        self.msg.info( '  IsEmEl           = %s', self.cutIsEM )
        self.msg.info( '  AuthorEl         = %s', self.cutAuthor )
        self.msg.info( '  EtaMax           = %r', self.cutEtaMax )
        self.msg.info( '  removeEtaCrack   = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin      = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax      = %r', self.crackEtaMax )
        self.msg.info( '  minNumberPassed  = %r', self.minNumberPassed )
        self.msg.info( '  passAll          = %r', self.passAll )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nElectrons         = 0
        self.nEventElPassEta    = 0
        self.nEventElPassEt     = 0
        self.nEventElPassIsEM   = 0
        self.nEventElPassAuthor = 0
        self.nEventMinNumPassed = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()  # noqa: F841

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...', self.name(), self.nProcessed )
        
        nElPassEta    = 0
        nElPassEt     = 0
        nElPassIsEM   = 0
        nElPassAuthor = 0

        # Create a class-member list of all electrons that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodElectrons = PyAthena.ElectronContainer(VIEW_ELEMENTS)
        #goodElectrons = [] 

        
        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true', self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        #Then... here we go!

        # Get the electron collection from StoreGate
        try:
            electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found', self.electronCollectionName )
            self.setFilterPassed(True)
            return StatusCode.Success

        # Loop over all electrons and make the selections cuts
        for electron in electronCollection :
            self.nElectrons += 1
            # Use Et of CaloCluster.
            cluster = None
            cluster = electron.cluster()
            if cluster is None :
                self.msg.warning( 'Could not get the cluster for this electron!' )
                pass
            else :
                if abs(cluster.etaBE(2)) < self.cutEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                    ( abs(cluster.etaBE(2)) < self.crackEtaMin
                                                      or abs(cluster.etaBE(2)) > self.crackEtaMax ) ):
                        nElPassEta += 1
                        # If the Et of this electron is larger than the minimum required Et
                        # of the high-Et cut, then the high-Et electron is found.
                        if cluster.et() > self.cutEtMin :
                            nElPassEt += 1
                            # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                            if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutAuthor ) :
                                nElPassAuthor += 1
                                # Check the quality of the electron using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM ):
                                    nElPassIsEM += 1
                                    # goodElectrons.append( electron )
                                    goodElectrons.push_back( electron )
                                    pass
                                pass
                            pass
                        pass
                    pass
                pass
            pass
        

        ## Translate the electron pass counters into event pass counters
        if nElPassEta > 0 :    self.nEventElPassEta += 1
        if nElPassEt > 0 :     self.nEventElPassEt += 1
        if nElPassIsEM > 0 :   self.nEventElPassIsEM += 1
        if nElPassAuthor > 0 : self.nEventElPassAuthor += 1

        ## Record the good electrons into StoreGate so that they can be retrieved by other algorithms
        #VIEW_ELEMENTS = 1
        #goodElectronColl = PyAthena.ElectronContainer(VIEW_ELEMENTS)
        #goodElectronColl.m_ownPolicy = 1 # This sets it as a VIEW_CONTAINER
        #for el in goodElectrons:
        #    goodElectronColl.push_back(el)
        #    pass
        if self.storeGateSvc.record( goodElectrons, self.goodElectronCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodElectrons into StoreGate with the key = ', self.goodElectronCollectionName )
            pass


        ## Check if the event is accepted
        if goodElectrons.__len__() >= self.minNumberPassed :
            self.nEventMinNumPassed += 1
            self.msg.debug( '%s event passed.', self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed.', self.name() )
            self.setFilterPassed(False)
            pass

        return StatusCode.Success



    def finalize(self):
        ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiElectrons    = 0.0
        effiElPassEta    = 0.0
        effiElPassEt     = 0.0
        effiElPassAuthor = 0.0
        effiElPassIsEM   = 0.0
        effiMinNumPassed = 0.0
        effiErrElectrons    = 0.0
        effiErrElPassEta    = 0.0
        effiErrElPassEt     = 0.0
        effiErrElPassAuthor = 0.0
        effiErrElPassIsEM   = 0.0
        effiErrMinNumPassed = 0.0
        try :
            # Efficiencies
            effiElectrons    = self.nElectrons / (1.0*self.nProcessed)
            effiElPassEta    = self.nEventElPassEta / float(self.nProcessed)
            effiElPassEt     = self.nEventElPassEt / float(self.nProcessed)
            effiElPassAuthor = self.nEventElPassAuthor / float(self.nProcessed)
            effiElPassIsEM   = self.nEventElPassIsEM / float(self.nProcessed)
            effiMinNumPassed = self.nEventMinNumPassed / float(self.nProcessed)

            # Error on efficiencies; use binomial errors
            effiErrElectrons    = math.sqrt(self.nElectrons) / self.nElectrons
            effiErrElPassEta    = 100.0 * math.sqrt( effiElPassEta*(1.0 - effiElPassEta) / float(self.nProcessed) )
            effiErrElPassEt     = 100.0 * math.sqrt( effiElPassEt*(1.0 - effiElPassEt) / float(self.nProcessed) )
            effiErrElPassAuthor = 100.0 * math.sqrt( effiElPassAuthor*(1.0 - effiElPassAuthor) / float(self.nProcessed) )
            effiErrElPassIsEM   = 100.0 * math.sqrt( effiElPassIsEM*(1.0 - effiElPassIsEM) / float(self.nProcessed) )
            effiErrMinNumPassed = 100.0 * math.sqrt( effiMinNumPassed*(1.0 - effiMinNumPassed) / float(self.nProcessed) )

            effiElPassEta    *= 100.0
            effiElPassEt     *= 100.0
            effiElPassAuthor *= 100.0
            effiElPassIsEM   *= 100.0
            effiMinNumPassed *= 100.0

        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...', self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:', self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:  %r', self.nProcessed )
        self.msg.info( ' Number of all electrons:     %r and number of electrons per event: %3.3f +/- %3.3f', self.nElectrons, effiElectrons, effiErrElectrons )
        self.msg.info( ' Events after eta cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventElPassEta,    effiElPassEta,    effiErrElPassEta )
        self.msg.info( ' Events after E_T cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventElPassEt,     effiElPassEt,     effiErrElPassEt )
        self.msg.info( ' Events after author cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventElPassAuthor, effiElPassAuthor, effiErrElPassAuthor )
        self.msg.info( ' Events after IsEM cut:       %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventElPassIsEM,   effiElPassIsEM,   effiErrElPassIsEM )
        self.msg.info( ' Events after minNumberPassed %r and resulting efficiency = (%3.3f +/- %3.3f)%%', self.nEventMinNumPassed, effiMinNumPassed, effiErrMinNumPassed )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

