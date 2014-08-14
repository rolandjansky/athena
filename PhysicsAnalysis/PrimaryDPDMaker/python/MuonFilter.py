# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        Single Muon Skimmer
##
## Author:      David Lopez Mateos (Columbia University/Caltech)
## Created:     July 2008
##
## Description: This is a short algorithm to select events that contain one
##              muon which fulfills a certain set of criteria.
##=============================================================================

__doc__ = """This is a short algorithm to select events that contain one 
muon which fulfills a certain set of criteria.
"""
__version__ = "0.0.1"
__author__  = "David Lopez Mateos <David.Lopez@cern.ch>"


import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode
from PrimaryDPDMaker import PrimaryDPDHelpers

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class MuonFilter( PyAthena.AthFilterAlgorithm ):
    """
    This class is a short algorithm to select events that contain one 
    muon which fulfills a certain set of criteria. The possible
    criteria are (with default values):
    cutPtMinMu     = 0*Units.GeV)
    muonAuthors    = 'all'  # Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
    cutContainerMu = 'all'      # Possible values: 'staco', 'muid', 'calo','all'
    cutEtaMax      = 3.0
    passAll        = False
    usingAOD       = True
    doEtcone         = False
    etconeMax        = 6.0*Units.GeV
    doNoTrackIsol      = False
    noTrackIsol        = 5
    innerNoTrackRadius = 0.05
    outerNoTrackRadius = 0.5
    doPtTrackIsol      = False
    ptTrackIsol        = 8.0*Units.GeV
    innerPtTrackRadius = 0.05
    outerPtTrackRadius = 0.5
    doCaloIsol         = False
    caloIsol           = 6.0*Units.GeV
    innerCaloRadius    = 0.05
    outerCaloRadius    = 0.5
    doJetIsol          = False
    jetIsolEt          = 15.0*Units.GeV
    jetDistance        = 0.5
    minNumberPassed    = 1
    
    """



    def __init__ ( self, name = "MuonFilter", **kw ):
        ## initialize base class
        kw['name'] = name
        super(MuonFilter, self).__init__(**kw)
        
        ## Define the cuts
        self.cutPtMinMu     = kw.get('cutPtMinMu', 0.0*Units.GeV)
        self.muonAuthors    = kw.get('muonAuthors', "all") # Possible values: 'lowpt', 'combined', 'calo', 'standalone', 'all'
        self.muonContainers = kw.get('muonContainers', "all") # Possible values: 'staco', 'muid', 'calo','all'
        self.goodMuonCollectionName = kw.get('goodMuonCollectionName', self.name() )
        self.cutEtaMax          = kw.get('cutEtaMax', 3.0)
        self.passAll            = kw.get('passAll', False)
        self.usingAOD           = kw.get('usingAOD', True)
        self.doEtcone           = kw.get('doEtcone', False)
        self.etconeMax          = kw.get('etconeMax', 6.0*Units.GeV)        
        self.doNoTrackIsol      = kw.get('doNoTrackIsol', False)
        self.noTrackIsol        = kw.get('noTrackIsol', 5)
        self.innerNoTrackRadius = kw.get('innerNoTrackRadius', 0.05)
        self.outerNoTrackRadius = kw.get('outerNoTrackRadius', 0.5)
        self.doPtTrackIsol      = kw.get('doPtTrackIsol', False)
        self.ptTrackIsol        = kw.get('ptTrackIsol', 8.0*Units.GeV)
        self.innerPtTrackRadius = kw.get('innerPtTrackRadius', 0.05)
        self.outerPtTrackRadius = kw.get('outerPtTrackRadius', 0.5)
        self.doCaloIsol         = kw.get('doCaloIsol', False)
        self.caloIsol           = kw.get('caloIsol', 6.0*Units.GeV)
        self.innerCaloRadius    = kw.get('innerCaloRadius', 0.05)
        self.outerCaloRadius    = kw.get('outerCaloRadius', 0.5)
        self.doJetIsol          = kw.get('doJetIsol', False)
        self.jetIsolEt          = kw.get('jetIsolEt', 15.0*Units.GeV)
        self.jetDistance        = kw.get('jetDistance', 0.5)
        self.jetContainerName   = kw.get('jetContainerName',"Cone4TowerJets")
        self.muonIsolationTool  = kw.get('muonIsolation',None)
        self.minNumberPassed    = kw.get('minNumberPassed', 1)

        #handle to storegate
        self.sgSvc = None
        self.containerNames = []

        return


    

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...', self.name() )
        self.msg.info( 'Will apply the following cuts:' )
        self.msg.info( '  PtMinMu          = %r', self.cutPtMinMu )
        self.msg.info( '  AuthorMu         = %s', self.muonAuthors )
        self.msg.info( '  EtaMax           = %r', self.cutEtaMax )
        if self.doEtcone: self.msg.info( '  etcone cut       = %r', self.etconeMax )
        self.msg.info( '  passAll          = %r', self.passAll )
        self.msg.info( '  minNumberPassed  = %r', self.minNumberPassed )
        self.msg.info( '************************************' )

        ## Also, declare some counters and initialize them to zero
        self.nProcessed            = 0
        self.nMuons                = 0
        self.nEventMuPassEta       = 0
        self.nEventMuPassPt        = 0
        self.nEventMuPassAuthor    = 0
        self.nEventMuPassPrescale  = 0
        self.nEventMuPassIsolation = 0
        self.nEventMinNumPassed    = 0

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()

        if self.doNoTrackIsol or self.doPtTrackIsol or self.doCaloIsol :
            if self.muonIsolationTool is None:
                self.msg.warning("Problem retrieving MuonIsolationTool pointer ")
                self.msg.warning("setting Track and Calo isolation to False ")
                self.doNoTrackIsol = False
                self.doPtTrackIsol = False
                self.doCaloIsol = False

        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }

        _append = self.containerNames.append
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
        ## retrieve a handle to the event store
        self.sgSvc = PyAthena.py_svc('StoreGateSvc')
        if not self.sgSvc :
            self.msg.error("could not retrieve a handle to the event store !")
            return StatusCode.Failure
        return StatusCode.Success



    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
        
        ## If passAll is selected, accept all events
        if self.passAll :
            self.msg.debug( '%s event passed because passAll is true' % self.name() )
            self.setFilterPassed(True)
            return StatusCode.Success

        jetCollection = None
        _retrieve = self.sgSvc.retrieve

        if self.doJetIsol :
            try :
                jetCollection = _retrieve ('JetCollection', jetContainerName)
            except LookupError :
                self.msg.warning('%s container not found in StoreGate ' % self.jetContainerName)
                self.msg.warning('Turning jet isolation off ')
                self.doJetIsol = False

        #Then... here we go!
        hasMuon     = False
        hasMuonPt   = False
        hasMuonEta  = False

        # Create a class-member list of all muons that passed all cuts.
        # This can then be used by other filters to do overlap removal, etc.
        goodMuons = []


        _contains = self.sgSvc.contains
        for name in self.containerNames :
            if not _contains('Analysis::MuonContainer', name):
                self.msg.debug('%s container not found in StoreGate ' % name)
                continue
            muonCollection = None
            try :
                muonCollection = _retrieve('Analysis::MuonContainer', name)
                pass
            except LookupError:
                self.msg.warning( 'Collection %s not found' % name  )           
                self.setFilterPassed(False)
                return StatusCode.Success
            
            hasMuonTemp, hasMuonTempEta, hasMuonTempPt, goodMuons = self.hasAnyMuon(muonCollection, hasMuon, hasMuonEta, hasMuonPt, jetCollection, goodMuons)
            hasMuon    = hasMuon    or hasMuonTemp
            hasMuonEta = hasMuonEta or hasMuonTempEta
            hasMuonPt  = hasMuonPt  or hasMuonTempPt
            pass

        ## Record the good muons into StoreGate so that they can be retrieved by other algorithms
        VIEW_ELEMENTS = 1
        goodMuonCont = PyAthena.Analysis.MuonContainer(VIEW_ELEMENTS)
        for muon in goodMuons :
            goodMuonCont.push_back( muon )
            pass
        if self.sgSvc.record( goodMuonCont, self.goodMuonCollectionName ) != StatusCode.Success :
            self.msg.error( 'Could not record the goodMuons into StoreGate with the key = ' % self.goodMuonCollectionName )
            pass


        ## Check if the event is accepted
        if goodMuons.__len__() >=  self.minNumberPassed :
            self.nEventMinNumPassed += 1
            self.msg.debug( '%s event passed' % self.name() )
            self.setFilterPassed(True)
            pass
        else :
            self.msg.debug( '%s event failed' % self.name() )
            self.setFilterPassed(False)
            pass

        return StatusCode.Success



    def finalize(self):
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


    def hasAnyMuon(self, muonCollection, hasMuon, hasMuonEta, hasMuonPt, jetCollection, goodMuons):

        nMuPassEta       = 0
        nMuPassPt        = 0
        nMuPassAuthor    = 0
        nMuPassIsolation = 0
        
        hasMuonInThisCollection = False
        for muon in muonCollection :
            self.nMuons += 1
            if abs(muon.eta()) < self.cutEtaMax :
                nMuPassEta += 1
                # If the Pt of this electron is larger than the minimum required Pt
                # of the high-Pt cut, then the high-Pt electron is found
                if muon.pt() > self.cutPtMinMu :
                    nMuPassPt += 1
                    # Check that the muon in question comes from the right reconstruction algorithm using PrimaryDPDHelpers
                    if PrimaryDPDHelpers.checkMuonAuthor( muon, self.muonAuthors ) :
                        nMuPassAuthor += 1
                        if self.passCaloIsol(muon) and self.passNoTracksIsol(muon) and self.passPtTracksIsol(muon) and self.passJetIsol(muon, jetCollection) and self.passPresel(muon) :
                            nMuPassIsolation += 1
                            hasMuonInThisCollection = True
                            goodMuons.append(muon)

        ## Translate the muon pass counters into event pass counters
        if not hasMuonEta and nMuPassEta > 0 :
            self.nEventMuPassEta += 1
            hasMuonEta = True
        if not hasMuonPt   and nMuPassPt > 0 :
            self.nEventMuPassPt += 1
            hasMuonPt = True
        if not hasMuon and nMuPassAuthor > 0 : self.nEventMuPassAuthor += 1
        if not hasMuon and nMuPassIsolation > 0 : self.nEventMuPassIsolation += 1
        return hasMuonInThisCollection, hasMuonEta, hasMuonPt, goodMuons
    
    def passPresel(self,muon) :
        if not self.doEtcone :
            return True
        if (muon.parameter(1)<self.etconeMax) :
            return True
        return False


    def passCaloIsol(self,muon) :

        if not self.doCaloIsol :
            return True
        outerConeEt = self.muonIsolationTool.caloIsolationEnergy(muon, dR, False,None)
        innerConeEt = self.muonIsolationTool.caloIsolationEnergy(muon, dR, False,None)
        
        if (outerConeEt-innerConeEt) < self.caloIsol :
            return True
        return False

    def passNoTracksIsol(self,muon) :

        if not self.doNoTrackIsol :
            return True
        outerConeNoTracks = self.muonIsolationTool.trackIsolationNumberOfTrack(muon,self.outerNoTrackRadius)
        innerConeNoTracks = self.muonIsolationTool.trackIsolationNumberOfTrack(muon,self.innerNoTrackRadius)
        if (outerConeNoTracks-innerConeNoTracks) < self.noTracksIsol :
            return True
        return False

    def passPtTracksIsol(self,muon) :

        if not self.doPtTrackIsol :
            return True
        outerConePtTracks = self.muonIsolationTool.trackIsolationEnergy(muon,self.outerNoTrackRadius)
        innerConePtTracks = self.muonIsolationTool.trackIsolationEnergy(muon,self.innerNoTrackRadius)
        if (outerConePtTracks-innerConePtTracks) < self.ptTracksIsol :
            return True
        return False

    def passJetIsol(self,muon,jetCollection) :

        # Declare the utility to calculate the DeltaR between two objects
        deltaR = self.utils['deltaR']

        if not self.doJetIsol :
            return True
        for jet in jetCollection :
            if deltaR(jet,muon) < self.jetDistance and jet.et() > self.jetIsolEt :
                return True
        return False
