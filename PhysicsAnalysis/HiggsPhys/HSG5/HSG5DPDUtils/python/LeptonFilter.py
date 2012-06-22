# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        LeptonFilter.py
##
## Author:      Robert Harrington
## Created:     July 2011
##
## Description: This is a short algorithm to select events that contain one
##              electron or muon which fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
electron or muon which fulfills a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "Robert Harrington <roberth@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class LeptonFilter( PyAthena.AthFilterAlgorithm ):

    """
    This class is a short algorithm to select events that contain one 
    electron which fulfills a certain set of criteria.

    The possible criteria for electrons are (with default values):

    cutEtMin         = 20.0*Units.GeV)
    cutIsEM          = 'None'    # Possible values: 'None', 'Loose', 'Medium', 'Tight', 'TightNoIsolation'
    cutElAuthor      = 'either'  # Possible values: 'softe', 'egamma', 'either'
    cutElEtaMax        = 2.5
    removeEtaCrack   = False
    crackEtaMin      = 1.37
    crackEtaMax      = 1.52
    minNumberPassedEl= 1
    passAllEl        = False

    The possible criteria for muons are (with default values):
    
    cutPtMinMu           = 0*Units.GeV
    muonAuthors          = 'all'  # Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
    cutContainerMu       = 'all'      # Possible values: 'staco', 'muid', 'calo','all'
    cutEtaMuMax          = 3.0
    passAllMu            = False
    usingAOD             = True
    doEtconeMu           = False
    etconeMuMax          = 6.0*Units.GeV
    doNoTrackIsolMu      = False
    noTrackIsolMu        = 5
    innerNoTrackRadiusMu = 0.05
    outerNoTrackRadiusMu = 0.5
    doPtTrackIsolMu      = False
    ptTrackIsolMu        = 8.0*Units.GeV
    innerPtTrackRadiusMu = 0.05
    outerPtTrackRadiusMu = 0.5
    doCaloIsolMu         = False
    caloIsolMu           = 6.0*Units.GeV
    innerCaloRadiusMu    = 0.05
    outerCaloRadiusMu    = 0.5
    doJetIsolMu          = False
    jetIsolEtMu          = 15.0*Units.GeV
    jetDistanceMu        = 0.5
    minNumberPassedMu    = 1
    """


    def __init__ ( self, name = "LeptonFilter", **kw ):

        ## initialize base class
        kw['name'] = name
        super(LeptonFilter, self).__init__(**kw)

        ## Define the cuts
        self.electronCollectionType     = kw.get('electronCollectionType', "ElectronContainer")
        self.electronCollectionName     = kw.get('electronCollectionName', "ElectronAODCollection")
        self.goodElectronCollectionName = kw.get('goodElectronCollectionName', self.name() )

        self.cutEtMinEl        = kw.get('cutEtMinEl', 20.0*Units.GeV)
        self.cutIsEM           = kw.get('cutIsEM', "None") # Possible values: "None", "Loose", 'Medium', 'Tight', 'TightNoIsolation'
        self.cutElAuthor       = kw.get('cutElAuthor', "either") # Possible values: 'softe', 'egamma', 'either'
        self.cutElEtaMax       = kw.get('cutElEtaMax', 2.5)
        self.removeEtaCrack    = kw.get('removeEtaCrack', False)
        self.crackEtaMin       = kw.get('crackEtaMin', 1.37)
        self.crackEtaMax       = kw.get('crackEtaMax', 1.52)
        self.minNumberPassedEl = kw.get('minNumberPassedEl', 1)
        self.passAllEl         = kw.get('passAllEl', False)

        ## muon
        self.cutPtMinMu           = kw.get('cutPtMinMu', 0.0*Units.GeV)
        self.muonAuthors          = kw.get('muonAuthors', "all") # Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
        self.muonContainers       = kw.get('muonContainers', "all") # Possible values: 'staco', 'muid', 'calo','all'
        self.goodMuonCollectionName = kw.get('goodMuonCollectionName', self.name() )
        self.cutMuEtaMax          = kw.get('cutMuEtaMax', 3.0)
        self.passAllMu            = kw.get('passAllMu', False)
        self.doEtconeMu           = kw.get('doEtconeMu', False)
        self.etconeMuMax          = kw.get('etconeMuMax', 6.0*Units.GeV)        
        self.doNoTrackIsolMu      = kw.get('doNoTrackIsolMu', False)
        self.noTrackIsolMu        = kw.get('noTrackIsolMu', 5)
        self.innerNoTrackRadiusMu = kw.get('innerNoTrackRadiusMu', 0.05)
        self.outerNoTrackRadiusMu = kw.get('outerNoTrackRadiusMu', 0.5)
        self.doPtTrackIsolMu      = kw.get('doPtTrackIsolMu', False)
        self.ptTrackIsolMu        = kw.get('ptTrackIsolMu', 8.0*Units.GeV)
        self.innerPtTrackRadiusMu = kw.get('innerPtTrackRadiusMu', 0.05)
        self.outerPtTrackRadiusMu = kw.get('outerPtTrackRadiusMu', 0.5)
        self.doCaloIsolMu         = kw.get('doCaloIsolMu', False)
        self.caloIsolMu           = kw.get('caloIsolMu', 6.0*Units.GeV)
        self.innerCaloRadiusMu    = kw.get('innerCaloRadiusMu', 0.05)
        self.outerCaloRadiusMu    = kw.get('outerCaloRadiusMu', 0.5)
        self.doJetIsolMu          = kw.get('doJetIsolMu', False)
        self.jetIsolEtMu          = kw.get('jetIsolEtMu', 15.0*Units.GeV)
        self.jetDistanceMu        = kw.get('jetDistanceMu', 0.5)
        self.jetContainerName     = kw.get('jetContainerName',"Cone4TowerJets")
        self.muonIsolationTool    = kw.get('muonIsolation',None)
        self.minNumberPassedMu    = kw.get('minNumberPassedMu', 1)

        self.usingAOD           = kw.get('usingAOD', True)

        ## Get the storgate handle
        self.storeGateSvc     = None
        self.muContainerNames = []
        
        return
    

    def initialize(self):
        
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following electron cuts:' )
        self.msg.info( '  EtMinEl            = %r', self.cutEtMinEl )
        self.msg.info( '  IsEmEl             = %s', self.cutIsEM )
        self.msg.info( '  AuthorEl           = %s', self.cutElAuthor )
        self.msg.info( '  ElEtaMax           = %r', self.cutElEtaMax )
        self.msg.info( '  removeEtaCrack     = %r', self.removeEtaCrack )
        self.msg.info( '  crackEtaMin        = %r', self.crackEtaMin )
        self.msg.info( '  crackEtaMax        = %r', self.crackEtaMax )
        self.msg.info( '  minNumberPassedEl  = %r', self.minNumberPassedEl )
        self.msg.info( '  passAllEl          = %r', self.passAllEl )

        self.msg.info( 'And the following muon cuts:' )
        self.msg.info( '  PtMinMu            = %r', self.cutPtMinMu )
        self.msg.info( '  AuthorMu           = %s', self.muonAuthors )
        self.msg.info( '  MuEtaMax           = %r', self.cutMuEtaMax )
        if self.doEtconeMu: self.msg.info( '  muon etcone cut    = %r', self.etconeMuMax )
        self.msg.info( '  passAllMu          = %r', self.passAllMu )
        self.msg.info( '  minNumberPassedMu  = %r', self.minNumberPassedMu )
        self.msg.info( '************************************' )

        
        ## Also, declare some counters and initialize them to zero
        self.nProcessed         = 0
        self.nEventMinNumPassed = 0

        self.nElectrons         = 0
        self.nEventElPassEta    = 0
        self.nEventElPassEt     = 0
        self.nEventElPassIsEM   = 0
        self.nEventElPassAuthor = 0

        self.nMuons                = 0
        self.nEventMuPassEta       = 0
        self.nEventMuPassPt        = 0
        self.nEventMuPassAuthor    = 0
        self.nEventMuPassPrescale  = 0
        self.nEventMuPassIsolation = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        if self.doNoTrackIsolMu or self.doPtTrackIsolMu or self.doCaloIsolMu :
            if self.muonIsolationTool is None:
                self.msg.warning("Problem retrieving MuonIsolationTool pointer ")
                self.msg.warning("setting Track and Calo isolation to False ")
                self.doNoTrackIsolMu = False
                self.doPtTrackIsolMu = False
                self.doCaloIsolMu = False

        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }

        _append = self.muContainerNames.append
        if self.usingAOD :
            if self.muonContainers.__contains__('staco') or self.muonContainers is 'all':
                _append("StacoMuonCollection")
            if self.muonContainers.__contains__('muid') or self.muonContainers is 'all':
                _append("MuidMuonCollection")
            if self.muonContainers.__contains__('calo') or self.muonContainers is 'all':
                _append("CaloMuonCollection")
        else :
            if self.muonContainers.__contains__('staco') or self.muonContainers is 'all':
                _append("StacoESDMuonCollection")
                if self.muonAuthors is 'combined' or self.muonAuthors is 'all' :
                    _append("StacoCombinedMuonContainer")
                if self.muonAuthors is 'lowpt' or self.muonAuthors is 'all' :
                    _append("StacoCombinedMuonContainerLowPt")
                if self.muonAuthors is 'standalone' or self.muonAuthors is 'all':
                    _append("MboyESDMuonContainer")
            if self.muonContainers.__contains__('muid') or self.muonContainers is 'all':
                _append("MuidESDMuonCollection")
                if self.muonAuthors is 'combined' or self.muonAuthors is 'all' :
                    _append("MuidCombinedMuonContainer")
                if self.muonAuthors is 'lowpt' or self.muonAuthors is 'all' :
                    _append("MuGirlCombinedMuonContainer")
                if self.muonAuthors is 'standalone' or self.muonAuthors is 'all' :
                    _append("MuidExtrMuonContainer")
            if self.muonContainers.__contains__('calo') or self.muonContainers is 'all':
                if self.muonAuthors is 'calo' or self.muonAuthors is 'all' :
                    _append("CaloMuonIdContainer")
                    _append("CaloESDMuonCollection")
        del _append

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure
        
        return StatusCode.Success




    def execute(self):
        
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )

        passEl = self.passElectronSelection()
        passMu = self.passMuonSelection()
        
        if ( passEl or passMu) :                         
            self.nEventMinNumPassed += 1
            self.msg.debug( '%s event passed.' % self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed.' % self.name() )
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

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:  %r' % self.nProcessed )
        self.msg.info( ' Number of all electrons:     %r and number of electrons per event: %3.3f +/- %3.3f' % ( self.nElectrons, effiElectrons, effiErrElectrons ) )
        self.msg.info( ' Events after eta cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassEta,    effiElPassEta,    effiErrElPassEta ) )
        self.msg.info( ' Events after E_T cut:        %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassEt,     effiElPassEt,     effiErrElPassEt ) )
        self.msg.info( ' Events after author cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassAuthor, effiElPassAuthor, effiErrElPassAuthor ) )
        self.msg.info( ' Events after IsEM cut:       %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventElPassIsEM,   effiElPassIsEM,   effiErrElPassIsEM ) )
        self.msg.info( ' Events after minNumberPassedEl %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMinNumPassed, effiMinNumPassed, effiErrMinNumPassed ) )
        self.msg.info( '***************************************************************' )

       ## Import needed modules
        import math

        ## Calculate the efficiencies and their errors
        effiMuons        = 0.0
        effiMuPassEta    = 0.0
        effiMuPassPt     = 0.0
        effiMuPassAuthor = 0.0
        effiMuPassIsol   = 0.0
        effiMinNumPassed = 0.0
        effiErrMuons        = 0.0
        effiErrMuPassEta    = 0.0
        effiErrMuPassPt     = 0.0
        effiErrMuPassAuthor = 0.0
        effiErrMuPassIsol   = 0.0
        effiErrMinNumPassed = 0.0
        try :
            # Efficiencies
            effiMuons        = self.nMuons / (1.0*self.nProcessed)
            effiMuPassEta    = self.nEventMuPassEta / float(self.nProcessed)
            effiMuPassPt     = self.nEventMuPassPt / float(self.nProcessed)
            effiMuPassAuthor = self.nEventMuPassAuthor / float(self.nProcessed)
            effiMuPassIsol   = self.nEventMuPassIsolation / float(self.nProcessed)
            effiMinNumPassed = self.nEventMinNumPassed / float(self.nProcessed)

            # Error on efficiencies, use binomial errors
            effiErrMuons        = math.sqrt(self.nMuons) / self.nMuons
            effiErrMuPassEta    = 100.0 * math.sqrt( effiMuPassEta*(1.0 - effiMuPassEta) / float(self.nProcessed) )
            effiErrMuPassPt     = 100.0 * math.sqrt( effiMuPassPt*(1.0 - effiMuPassPt) / float(self.nProcessed) )
            effiErrMuPassAuthor = 100.0 * math.sqrt( effiMuPassAuthor*(1.0 - effiMuPassAuthor) / float(self.nProcessed) )
            effiErrMuPassIsol   = 100.0 * math.sqrt( effiMuPassIsol*(1.0 - effiMuPassIsol) / float(self.nProcessed) )
            effiErrMinNumPassed = 100.0 * math.sqrt( effiMinNumPassed*(1.0 - effiMinNumPassed) / float(self.nProcessed) )

            effiMuPassEta    *= 100.0
            effiMuPassPt     *= 100.0
            effiMuPassAuthor *= 100.0
            effiMuPassIsol   *= 100.0
            effiMinNumPassed *= 100.0

        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the uncertainties on the pass efficiencies...' )
            pass

        
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Cut-flow table of %s skimming algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events: %r' % self.nProcessed )
        self.msg.info( ' Number of all muons:  %r and number of muons per event: %3.3f +/- %3.3f' % ( self.nMuons, effiMuons, effiErrMuons ) )
        self.msg.info( ' Events after eta cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMuPassEta,    effiMuPassEta,    effiErrMuPassEta ) )
        self.msg.info( ' Events after p_T cut:     %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMuPassPt,     effiMuPassPt,     effiErrMuPassPt ) )
        self.msg.info( ' Events after author cut:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMuPassAuthor, effiMuPassAuthor, effiErrMuPassAuthor ) )
        self.msg.info( ' Events after isolation cut:  %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMuPassIsolation, effiMuPassIsol, effiErrMuPassIsol ) )
        self.msg.info( ' Events after minNumberPassed %r and resulting efficiency = (%3.3f +/- %3.3f)%%' % ( self.nEventMinNumPassed, effiMinNumPassed, effiErrMinNumPassed ) )
        self.msg.info( '***************************************************************' )


        return StatusCode.Success

    
    def passPresel(self,muon) :
        if not self.doEtconeMu :
            return True
        if (muon.parameter(1)<self.etconeMax) :
            return True
        return False


    def passCaloIsol(self,muon) :

        if not self.doCaloIsolMu :
            return True
        outerConeEt = self.muonIsolationTool.caloIsolationEnergy(muon, dR, False,None)
        innerConeEt = self.muonIsolationTool.caloIsolationEnergy(muon, dR, False,None)
        
        if (outerConeEt-innerConeEt) < self.caloIsolMu :
            return True
        return False

    def passNoTracksIsol(self,muon) :

        if not self.doNoTrackIsolMu :
            return True
        outerConeNoTracks = self.muonIsolationTool.trackIsolationNumberOfTrack(muon,self.outerNoTrackRadiusMu)
        innerConeNoTracks = self.muonIsolationTool.trackIsolationNumberOfTrack(muon,self.innerNoTrackRadiusMu)
        if (outerConeNoTracks-innerConeNoTracks) < self.noTracksIsolMu :
            return True
        return False

    def passPtTracksIsol(self,muon) :

        if not self.doPtTrackIsolMu :
            return True
        outerConePtTracks = self.muonIsolationTool.trackIsolationEnergy(muon,self.outerNoTrackRadiusMu)
        innerConePtTracks = self.muonIsolationTool.trackIsolationEnergy(muon,self.innerNoTrackRadiusMu)
        if (outerConePtTracks-innerConePtTracks) < self.ptTracksIsolMu :
            return True
        return False

    def passJetIsol(self,muon,jetCollection) :

        # Declare the utility to calculate the DeltaR between two objects
        deltaR = self.utils['deltaR']

        if not self.doJetIsolMu :
            return True
        for jet in jetCollection :
            if deltaR(jet,muon) < self.jetDistanceMu and jet.et() > self.jetIsolEtMu :
                return True
        return False

    def passElectronSelection(self) :

        #######################################################
        # electron selection
        #######################################################        

        nElPassEta    = 0
        nElPassEt     = 0
        nElPassIsEM   = 0
        nElPassAuthor = 0

        # Create a class-member list of all electrons that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodElectrons = PyAthena.ElectronContainer(VIEW_ELEMENTS)

        ## If passAll is selected, accept all events
        if self.passAllEl :
            self.msg.debug( '%s event passed electron selection because passAll is true' % self.name() )
            return True

        # Get the electron collection from StoreGate
        try:
            electronCollection = self.storeGateSvc.retrieve( self.electronCollectionType, self.electronCollectionName )
            pass
        except LookupError:
            self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
            return True

        # Loop over all electrons and make the selections cuts
        for electron in electronCollection :
            self.nElectrons += 1
            # Use Et of CaloCluster.
            cluster = None
            cluster = electron.cluster()
            if cluster == None :
                self.msg.warning( 'Could not get the cluster for this electron!' )
                pass
            else :
                if abs(cluster.etaBE(2)) < self.cutElEtaMax :
                    # Check if the EM calorimeter barrel-encap crack should be removed.
                    if not self.removeEtaCrack or ( self.removeEtaCrack and \
                                                    ( abs(cluster.etaBE(2)) < self.crackEtaMin
                                                      or abs(cluster.etaBE(2)) > self.crackEtaMax ) ):
                        nElPassEta += 1
                        # If the Et of this electron is larger than the minimum required Et
                        # of the high-Et cut, then the high-Et electron is found.
                        if cluster.et() > self.cutEtMinEl :
                            nElPassEt += 1
                            # Check that the electron in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                            if PrimaryDPDHelpers.checkEgammaAuthor( electron, self.cutElAuthor ) :
                                nElPassAuthor += 1
                                # Check the quality of the electron using PrimaryDPDHelpers
                                if PrimaryDPDHelpers.checkElectronIsEM( electron, self.cutIsEM ):
                                    nElPassIsEM += 1
                                    # goodElectrons.append( electron )
                                    goodElectrons.push_back( electron) 
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

        if self.storeGateSvc.record( goodElectrons, self.goodElectronCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodElectrons into StoreGate with the key = ' % self.goodElectronCollectionName )
            pass


        ## Check if the event is accepted
        if goodElectrons.__len__() >= self.minNumberPassedEl :
            self.msg.debug( '%s event passed electron selection' % self.name() )
            return True
            
        
        self.msg.debug( '%s event failed electron selection' % self.name() )
        return False


    def passMuonSelection(self) :
        
        #######################################################
        # muon selection
        #######################################################

        ## If passAll is selected, accept all events
        if self.passAllMu :
            self.msg.debug( '%s event passed muon selection because passAllMu is true' % self.name() )
            return True

        jetCollection = None
        _retrieve = self.storeGateSvc.retrieve

        if self.doJetIsolMu :
            try :
                jetCollection = _retrieve ('JetCollection', jetContainerName)
            except LookupError :
                self.msg.warning('%s container not found in StoreGate ' % self.jetContainerName)
                self.msg.warning('Turning jet isolation off ')
                self.doJetIsolMu = False

        #Then... here we go!
        hasMuon     = False
        hasMuonPt   = False
        hasMuonEta  = False

        # Create a class-member list of all muons that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        VIEW_ELEMENTS = 1
        goodMuons = PyAthena.Analysis.MuonContainer(VIEW_ELEMENTS)

        _contains = self.storeGateSvc.contains
        for name in self.muContainerNames :
            if not _contains('Analysis::MuonContainer', name):
                self.msg.debug('%s container not found in StoreGate ' % name)
                continue
            muonCollection = None
            try :
                muonCollection = _retrieve('Analysis::MuonContainer', name)
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % name  )           
                return false
            
            ##############
            nMuPassEta       = 0
            nMuPassPt        = 0
            nMuPassAuthor    = 0
            nMuPassIsolation = 0
            
            for muon in muonCollection :
                self.nMuons += 1

                # eta cut
                if abs(muon.eta()) < self.cutMuEtaMax :
                    nMuPassEta += 1

                    # muon pT cut
                    if muon.pt() > self.cutPtMinMu :
                        nMuPassPt += 1

                        # Check that the muon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                        if PrimaryDPDHelpers.checkMuonAuthor( muon, self.muonAuthors ) :
                            nMuPassAuthor += 1

                            if self.passCaloIsol(muon) and self.passNoTracksIsol(muon) and self.passPtTracksIsol(muon) and self.passJetIsol(muon, jetCollection) and self.passPresel(muon) :
                                nMuPassIsolation += 1

                                goodMuons.push_back(muon)
                                pass
                            pass
                        pass
                    pass
                pass

            
            ## Translate the muon pass counters into event pass counters
            if nMuPassEta    > 0    : self.nEventMuPassEta       += 1
            if nMuPassPt     > 0    : self.nEventMuPassPt        += 1
            if nMuPassAuthor > 0    : self.nEventMuPassAuthor    += 1
            if nMuPassIsolation > 0 : self.nEventMuPassIsolation += 1

            pass

        ## Record the good muons into StoreGate so that they can be retrieved by other algorithms
        if self.storeGateSvc.record( goodMuons, self.goodMuonCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodMuons into StoreGate with the key = ' % self.goodMuonCollectionName )
            pass


        ## Check if the event is accepted
        if goodMuons.__len__() >=  self.minNumberPassedMu :
            self.msg.debug( '%s event passed muon selection' % self.name() )
            return True
        
        self.msg.debug( '%s event failed muon selection' % self.name() )
        return False
