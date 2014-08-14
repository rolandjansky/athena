# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Z -> mu mu Skimmer
##
## Author:      Karsten Koeneke (DESY)
## Created:     August 2008
##
## Description: This is a short algorithm to select events that contain Z -> mumu
##              which fulfill a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain Z -> mumu
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


class ZmumuFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain Z -> ee
    which fulfill a certain set of criteria. The possible
    criteria are (with default values):

    the type of muons by its jetCollectionName:
    muonCollectionType  = 'MuonContainer'
    muonCollectionName  = 'MuonAODCollection'
    
    minimum E_T of both muons:
    cutEtMin                = 15.0*Units.GeV 

    maximum |eta| of both muons:
    cutEtaMax               = 2.5 

    di-muon mass lower bound:
    cutDiMuMassLower        = 60.0*Units.GeV

    di-muon mass upper bound:
    cutDiMuMassUpper        = 120.0*Units.GeV

    pass all events if true:
    passAll                 = False
    """



    def __init__ ( self, name = "ZmumuFilter", **kw ):
        # initialize base class
        kw['name'] = name
        super(ZmumuFilter, self).__init__(**kw)

        # Define the cuts
        self.muonCollectionType     = kw.get('muonCollectionType', "Analysis::MuonContainer")
        self.muonCollectionName     = kw.get('muonCollectionName', "StacoMuonCollection")
        self.cutEtMin               = kw.get('cutEtMin', 15.0*Units.GeV)
        self.cutLeadingEtMin        = kw.get('cutLeadingEtMin', 15.0*Units.GeV)
        self.cutEtaMax              = kw.get('cutEtaMax', 2.5)             # THIS IS NOT RIGHT FOR MUONS!!!
        self.removeEtaCrack         = kw.get('removeEtaCrack', False)
        self.crackEtaMin            = kw.get('crackEtaMin', 1.37)     # THIS IS NOT RIGHT FOR MUONS!!!
        self.crackEtaMax            = kw.get('crackEtaMax', 1.52)     # THIS IS NOT RIGHT FOR MUONS!!!
        self.requireLooseMuon       = kw.get('requireLooseMuon', False )
        self.minNumberCombinedMuons = kw.get('minNumberCombinedMuons', 0 )
        self.cutDiMuMassLower       = kw.get('cutDiMuMassLower', 60.0*Units.GeV)
        self.cutDiMuMassUpper       = kw.get('cutDiMuMassUpper', 120.0*Units.GeV)
        self.passAll                = kw.get('passAll', False)
        self.storeGateSvc           = None


    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  using muon collection  = %r %r' % (self.muonCollectionType, self.muonCollectionName) )
        self.msg.info( '  EtMin                      = %r', self.cutEtMin )
        self.msg.info( '  EtaMax                     = %r', self.cutEtaMax )
        self.msg.info( '  requireLooseMuon           = %r', self.requireLooseMuon )
        self.msg.info( '  minNumberCombinedMuons     = %r', self.minNumberCombinedMuons )
        self.msg.info( '  DiMuMassLower              = %r', self.cutDiMuMassLower )
        self.msg.info( '  DiMuMassUpper              = %r', self.cutDiMuMassUpper )
        self.msg.info( '  removeEtaCrack             = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin                = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax                = %r', self.crackEtaMax )
        self.msg.info( '  passAll                    = %r', self.passAll )
        self.msg.info( '************************************' )

        # Also, declare some counters and initialize them to zero
        self.nProcessed             = 0
        self.nMuons                 = 0
        self.nGoodMuons             = 0
        self.nEventPassEta          = 0
        self.nEventPassEt           = 0
        self.nEventPassLeadingEt    = 0
        self.nEventPassLooseMuon    = 0
        self.nEventPassCombinedMuon = 0
        self.nEventPassDiMuonMass   = 0

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
        nPassEta          = 0
        nPassEt           = 0
        nPassLeadingEt    = 0
        nPassLooseMuon    = 0
        nPassCombinedMuon = 0
        nPassDiMuonMass   = 0

        # If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success


        # Then... here we go!
        acceptEvent  = False
        # Get the muon collections from StoreGate
        try:
            muonCollection = self.storeGateSvc.retrieve( self.muonCollectionType, self.muonCollectionName )
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.muonCollectionName )           
            self.setFilterPassed(False)
            return StatusCode.Success

        #look for two muons 
        for i in range( muonCollection.size() ) :
            self.nMuons += 1
            for j in range( muonCollection.size() ) :
                if j>i and not acceptEvent :
                    muon1 = muonCollection[i]
                    muon2 = muonCollection[j]

                    # Determine some muon properties
                    muon1_isCombined = 0
                    muon2_isCombined = 0
                    if muon1.isCombinedMuon() :
                        muon1_isCombined = 1
                        pass
                    if muon2.isCombinedMuon() :
                        muon2_isCombined = 1
                        pass

                    muon1_isLoose = muon1.isLoose()
                    muon2_isLoose = muon2.isLoose()                        
                    
                    # Check if the eta requirements are fullfilled
                    if abs(muon1.eta()) < self.cutEtaMax and abs(muon2.eta()) < self.cutEtaMax :
                        # Check if the EM calorimeter barrel-encap crack should be removed.
                        if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                        ( abs(muon1.eta()) < self.crackEtaMin or abs(muon1.eta()) > self.crackEtaMax ) and \
                                                        ( abs(muon2.eta()) < self.crackEtaMin or abs(muon2.eta()) > self.crackEtaMax )  ) :
                            # Update event counter...
                            nPassEta += 1
                            if muon1.et() > self.cutEtMin and muon2.et() > self.cutEtMin :
                                # Update event counter...
                                nPassEt += 1
                                
                                if muon1.et() > self.cutLeadingEtMin or muon2.et() > self.cutLeadingEtMin :
                                    # Update event counter...
                                    nPassLeadingEt += 1

                                    # Check if both muons are loose muons
                                    if not self.requireLooseMuon or ( self.requireLooseMuon and muon1_isLoose and muon1_isLoose ) :
                                        # update counter
                                        nPassLooseMuon += 1

                                        # Check for isCombinedMuon
                                        if ( muon1_isCombined + muon2_isCombined ) >= self.minNumberCombinedMuons :
                                            # Update counter
                                            nPassCombinedMuon += 1
                                        

                                            self.nGoodMuons += 1

                                            # Calculate the di-jet invariant mass using TLorentzVectors from ROOT
                                            muonTLVec1 = TLorentzVector( muon1.px(), muon1.py(), muon1.pz(), muon1.e() )
                                            muonTLVec2 = TLorentzVector( muon2.px(), muon2.py(), muon2.pz(), muon2.e() )
                                            muonmuonTLVec = muonTLVec1 + muonTLVec2
                                            diMuonMass = muonmuonTLVec.M()

                                            # Select events inside the invariant di-muon mass window
                                            if diMuonMass > self.cutDiMuMassLower and diMuonMass < self.cutDiMuMassUpper :
                                                nPassDiMuonMass += 1
                                                acceptEvent = True
                                                pass  # End: Check DiMuonMass 
                                            pass  # End: Check isCombinedMuon
                                        pass  # End: Check LooseMuons
                                    pass  # End: Check leading muon Et
                                pass  # End: Check Et
                            pass  # End: Check eta crack removal
                        pass  # End: Check eta acceptance
                    pass  # End: if j>i and not acceptEvent
                pass  # End: for j in range( muonCollection.size() )
            pass  # End: for i in range( muonCollection.size() )


        ## Translate the muon pass counters into event pass counters
        if nPassEta > 0 :        self.nEventPassEta        += 1
        if nPassEt > 0 :         self.nEventPassEt         += 1
        if nPassDiMuonMass > 0 : self.nEventPassDiMuonMass += 1

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
        effiMuons             = 0.0
        effiGoodMuons         = 0.0
        effiPassEta           = 0.0
        effiPassEt            = 0.0
        effiPassDiMuonMass    = 0.0
        effiErrMuons          = 0.0
        effiErrGoodMuons      = 0.0
        effiErrPassEta        = 0.0
        effiErrPassEt         = 0.0
        effiErrPassDiMuonMass = 0.0
        try :
            # Efficiencies
            effiMuons          = self.nMuons / (1.0*self.nProcessed)
            effiGoodMuons      = self.nGoodMuons / (1.0*self.nProcessed)
            effiPassEta        = 100.0*self.nEventPassEta / self.nProcessed
            effiPassEt         = 100.0*self.nEventPassEt / self.nProcessed
            effiPassDiMuonMass = 100.0*self.nEventPassDiMuonMass / self.nProcessed

            # Error on efficiencies
            effiErrMuons          = math.sqrt(self.nMuons) / self.nMuons
            effiErrGoodMuons      = math.sqrt(self.nGoodMuons) / self.nGoodMuons
            effiErrPassEta        = 100.0 * math.sqrt(self.nEventPassEta) / self.nProcessed
            effiErrPassEt         = 100.0 * math.sqrt(self.nEventPassEt) / self.nProcessed
            effiErrPassDiMuonMass = 100.0 * math.sqrt(self.nEventPassDiMuonMass) / self.nProcessed
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all muons:                     %r and number of muons per event: %3.3f +/- %3.3f' % ( self.nMuons,               effiMuons,          effiErrMuons ) )
        self.msg.info( ' Number of muons after E_T and eta cuts:  %r and number of muons per event: %3.3f +/- %3.3f' % ( self.nGoodMuons,           effiGoodMuons,      effiErrGoodMuons ) )
        self.msg.info( ' Events after eta cut:                    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEta,        effiPassEta,        effiErrPassEta ) )
        self.msg.info( ' Events after E_T cut:                    %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassEt,         effiPassEt,         effiErrPassEt ) )
        self.msg.info( ' Events after di-muon invariant mass cut: %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventPassDiMuonMass, effiPassDiMuonMass, effiErrPassDiMuonMass ) )
        self.msg.info( '***************************************************************' )

        return StatusCode.Success

