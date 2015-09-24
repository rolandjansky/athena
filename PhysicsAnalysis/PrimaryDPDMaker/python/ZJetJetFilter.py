# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Z -> Jet Jet Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2008
##
## Description: This is a short algorithm to select events that contain Z -> 
##              Jet Jet which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain Z ->
Jet Jet which fulfill a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Karsten Koeneke <karsten.koeneke@desy.de>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

import math


class ZJetJetFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain Z -> 
    Jet Jet which fulfill a certain set of criteria. The possible
    criteria are (with default values):

    the type of jets by its jetCollectionName:
    jetCollectionName       = 'Cone4TowerParticleJets'
    
    minimum E_T of both jets:
    cutEtMin                = 25.0*Units.GeV 

    maximum |eta| of both jets:
    cutEtaMax               = 2.5 

    di-jet mass lower bound:
    cutDiJetMassLower       = 70.0*Units.GeV

    di-jet mass upper bound:
    cutDiJetMassUpper       = 110.0*Units.GeV

    Maximum SumEt cut:
    cutSumEt       = 400.0*Units.GeV

    pass all events if true:
    passAll                 = False
    """



    def __init__ ( self, name = "ZJetJetFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(ZJetJetFilter, self).__init__(**kw)
        
        # Define the cuts
        self.jetCollectionType = kw.get('jetCollectionType', "JetCollection")
        self.jetCollectionName = kw.get('jetCollectionName', "Cone4TowerJets")
        self.cutEtMin          = kw.get('cutEtMin', 25.0*Units.GeV)
        self.cutEtaMax         = kw.get('cutEtaMax', 2.5)
        self.cutDiJetMassLower = kw.get('cutDiJetMassLower', 70.0*Units.GeV)
        self.cutDiJetMassUpper = kw.get('cutDiJetMassUpper', 110.0*Units.GeV)
        self.cutSumEt          = kw.get('cutSumEt', 400.0*Units.GeV)
        self.passAll           = kw.get('passAll', False)
        self.storeGateSvc      = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using jetCollection  = %r', self.jetCollectionName )
        self.msg.info( '  EtMin                = %r', self.cutEtMin )
        self.msg.info( '  EtaMax               = %r', self.cutEtaMax )
        self.msg.info( '  DiJetMassLower       = %r', self.cutDiJetMassLower )
        self.msg.info( '  DiJetMassUpper       = %r', self.cutDiJetMassUpper )
        self.msg.info( '  SumEt                = %r', self.cutSumEt )
        self.msg.info( '  passAll              = %r', self.passAll )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed              = 0
        self.nEventPassSumEt         = 0
        self.nJets                   = 0
        self.nGoodJets               = 0
        self.nEventJetPassEta        = 0
        self.nEventJetPassEt         = 0
        self.nEventJetPassDiJetMass  = 0

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
        nJetPassEta        = 0
        nJetPassEt         = 0
        nJetPassDiJetMass  = 0

        # If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Then... here we go!
        acceptEvent  = False
        # Get the missing et from StoreGate
        try:
            met = self.storeGateSvc.retrieve('MissingET', 'MET_RefFinal')
        except LookupError:
            self.msg.warning( 'Collection MET_RefFinal not found' )           
            self.setFilterPassed(False)
            return StatusCode.Success

        missingEt = met.et()
        sumEt = met.sumet()
        if sumEt < self.cutSumEt :
            self.nEventPassSumEt += 1
            
            # Get the jet collections from StoreGate
            try :
                # try to get the jet collection the user wants
                jetCollection = self.storeGateSvc.retrieve( self.jetCollectionType, self.jetCollectionName )
            except LookupError :
                try :
                    # If that didn't work, get the jet collection for release 14.1.0 (onwards)
                    jetCollection = self.storeGateSvc.retrieve( 'JetCollection', 'Cone4TowerJets' )
                except LookupError :
                    # If even that didn't work, try to get the release 13 jet collections
                    jetCollection = self.storeGateSvc.retrieve( 'ParticleJetContainer', 'Cone4TowerParticleJets' )
                    pass
                pass

            if not jetCollection:
                self.msg.warning( 'Collection %s not found' % self.jetCollectionName )           
                self.setFilterPassed(False)
                return StatusCode.Success

            #look for back-to-back jets with pT > 15 GeV
            for i in range( jetCollection.size() ) :
                self.nJets += 1
                for j in range( jetCollection.size() ) :
                    if j>i and not acceptEvent :
                        jet1 = jetCollection[i]
                        jet2 = jetCollection[j]
                        if abs(jet1.eta()) < self.cutEtaMax and abs(jet2.eta()) < self.cutEtaMax :
                            #Update event counter...
                            nJetPassEta += 1
                            if jet1.et() > self.cutEtMin and jet2.et() > self.cutEtMin :
                                #Update event counter...
                                nJetPassEt += 1
                                self.nGoodJets += 1

                                # Calculate the di-jet invariant mass using TLorentzVectors from ROOT
                                jetTLVec1 = TLorentzVector( jet1.px(), jet1.py(), jet1.pz(), jet1.e() )
                                jetTLVec2 = TLorentzVector( jet2.px(), jet2.py(), jet2.pz(), jet2.e() )
                                jetjetTLVec = jetTLVec1 + jetTLVec2
                                diJetMass = jetjetTLVec.M()

                                # Select events inside the invariant di-jet mass window
                                if diJetMass > self.cutDiJetMassLower and diJetMass < self.cutDiJetMassUpper :
                                    nJetPassDiJetMass += 1
                                    acceptEvent = True


        ## Translate the electron pass counters into event pass counters
        if nJetPassEta > 0 :        self.nEventJetPassEta += 1
        if nJetPassEt > 0 :         self.nEventJetPassEt += 1
        if nJetPassDiJetMass > 0 :  self.nEventJetPassDiJetMass += 1

        ## Check if the event is accepted
        if acceptEvent :
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
        effiJets                 = 0.0
        effiGoodJets             = 0.0
        effiPassSumEt            = 0.0
        effiJetPassEta           = 0.0
        effiJetPassEt            = 0.0
        effiJetPassDiJetMass     = 0.0
        effiErrJets              = 0.0
        effiErrGoodJets          = 0.0
        effiErrPassSumEt         = 0.0
        effiErrJetPassEta        = 0.0
        effiErrJetPassEt         = 0.0
        effiErrJetPassDiJetMass  = 0.0
        try :
            # Efficiencies
            effiJets              = self.nJets / (1.0*self.nProcessed)
            effiGoodJets          = self.nGoodJets / (1.0*self.nProcessed)
            effiPassSumEt         = 100.0*self.nEventPassSumEt / self.nProcessed
            effiJetPassEta        = 100.0*self.nEventJetPassEta / self.nProcessed
            effiJetPassEt         = 100.0*self.nEventJetPassEt / self.nProcessed
            effiJetPassDiJetMass  = 100.0*self.nEventJetPassDiJetMass / self.nProcessed

            # Error on efficiencies
            effiErrJets              = math.sqrt(self.nJets) / self.nJets
            effiErrGoodJets          = math.sqrt(self.nGoodJets) / self.nGoodJets
            effiErrPassSumEt         = 100.0 * math.sqrt(self.nEventPassSumEt) / self.nProcessed
            effiErrJetPassEta        = 100.0 * math.sqrt(self.nEventJetPassEta) / self.nProcessed
            effiErrJetPassEt         = 100.0 * math.sqrt(self.nEventJetPassEt) / self.nProcessed
            effiErrJetPassDiJetMass  = 100.0 * math.sqrt(self.nEventJetPassDiJetMass) / self.nProcessed
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )


        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all jets:                      %r and number of jets per event: %3.3f +/- %3.3f' % ( self.nJets, effiJets, effiErrJets ) )
        self.msg.info( ' Number of jets after E_T and eta cuts:   %r and number of jets per event: %3.3f +/- %3.3f' % ( self.nGoodJets, effiGoodJets, effiErrGoodJets ) )
        self.msg.info( ' Events after sum E_T cut:                %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassSumEt,         effiPassSumEt,         effiErrPassSumEt ) )
        self.msg.info( ' Events after eta cut:                    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventJetPassEta,        effiJetPassEta,        effiErrJetPassEta ) )
        self.msg.info( ' Events after E_T cut:                    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventJetPassEt,         effiJetPassEt,         effiErrJetPassEt ) )
        self.msg.info( ' Events after di-jet invariant mass cut:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventJetPassDiJetMass,  effiJetPassDiJetMass,  effiErrJetPassDiJetMass ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

