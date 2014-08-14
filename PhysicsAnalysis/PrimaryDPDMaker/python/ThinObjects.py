# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name: ThinObjects.py   
##
## Author: Karsten Koeneke (DESY)
## Email:  karsten.koeneke@desy.de
## 
## Description: Only save the Objects in the vecinity of a "good"
##              electron or photon.
##                      
##=============================================================================


import AthenaCommon.SystemOfUnits as Units
from AthenaPython.PyAthena import StatusCode
import AthenaPython.PyAthena as PyAthena
from PyTriggerTools import PyTriggerTools



class ThinObjects( PyAthena.Alg ):
    """
    This class is a short algorithm inheriting from PyAthena.Alg that
    is meant to remove uninteresting objects from a container (thinning).
    The possible criteria are (with default values):

    thinCollectionType       = 'Rec::TrackParticleContainer'
    thinCollectionName       = 'TrackParticleCandidate'
    jetCollectionType        = 'JetCollection'
    jetCollectionNames       = ['Cone4TowerJets', 'Cone4TopoJets']
    electronCollectionType   = 'ElectronContainer'
    electronCollectionName   = 'ElectronAODCollection'
    photonCollectionType     = 'PhotonContainer'
    photonCollectionName     = 'PhotonAODCollection'
    muonCollectionType       = 'Analysis::MuonContainer'
    muonCollectionName       = 'StacoMuonCollection'
    tauCollectionType        = 'TauJetContainer'
    tauCollectionName        = 'TauRecContainer'
    minEtElectron            = 3.0*Units.GeV
    minEtPhoton              = 3.0*Units.GeV
    minEtMuon                = 5.0*Units.GeV
    minEtTau                 = 5.0*Units.GeV
    minEtJet                 = 15.0*Units.GeV
    keepObjectsNearElectrons = True
    maxDeltaRElectronObject  = 0.5
    keepObjectsNearPhotons   = True
    maxDeltaRPhotonObject    = 0.5
    keepObjectsNearMuons     = True
    maxDeltaRMuonObject      = 0.5
    keepObjectsNearTaus      = True
    maxDeltaRTauObject       = 0.5
    keepObjectsNearJets      = True
    maxDeltaRJetObject       = 0.5
    keepIsolatedObjects      = True
    minPtIsolatedObjects     = 5.0*Units.GeV
    keepObjectsFromEGamma    = False
   
    """
    
    def __init__ ( self, name = "ThinObjects", **kw ) :
        # initialize base class
        kw['name'] = name
        super(ThinObjects,self).__init__(**kw)

        # Set default values for properties
        self.thinCollectionType       = kw.get('thinCollectionType', 'Rec::TrackParticleContainer')
        self.thinCollectionName       = kw.get('thinCollectionName', 'TrackParticleCandidate')
        self.jetCollectionType        = kw.get('jetCollectionType', 'JetCollection')
        self.jetCollectionNames       = kw.get('jetCollectionNames', ['Cone4TowerJets', 'Cone4TopoJets'])
        self.electronCollectionType   = kw.get('electronCollectionType', 'ElectronContainer')
        self.electronCollectionName   = kw.get('electronCollectionName', 'ElectronAODCollection')
        self.photonCollectionType     = kw.get('photonCollectionType', 'PhotonContainer')
        self.photonCollectionName     = kw.get('photonCollectionName', 'PhotonAODCollection')
        self.muonCollectionType       = kw.get('muonCollectionType', 'Analysis::MuonContainer')
        self.muonCollectionName       = kw.get('muonCollectionName', 'StacoMuonCollection')
        self.tauCollectionType        = kw.get('tauCollectionType', 'TauJetContainer')
        self.tauCollectionName        = kw.get('tauCollectionName', 'TauRecContainer')
        self.minEtElectron            = kw.get('minEtElectron', 3.0*Units.GeV)
        self.minEtPhoton              = kw.get('minEtPhoton', 3.0*Units.GeV)
        self.minEtMuon                = kw.get('minEtMuon', 5.0*Units.GeV)
        self.minEtTau                 = kw.get('minEtTau', 5.0*Units.GeV)
        self.minEtJet                 = kw.get('minEtJet', 15.0*Units.GeV)
        self.keepObjectsNearLvl1Rois  = kw.get('keepObjectsNearLvl1Rois', False)
        self.Lvl1RoiNames             = kw.get('Lvl1RoiNames', [ 'EM7' ] )
        self.maxDeltaRLvl1RoiObject   = kw.get('maxDeltaRLvl1RoiObject', 0.5)
        self.keepObjectsNearElectrons = kw.get('keepObjectsNearElectrons', False)
        self.maxDeltaRElectronObject  = kw.get('maxDeltaRElectronObject', 0.5)
        self.keepObjectsNearPhotons   = kw.get('keepObjectsNearPhotons', False)
        self.maxDeltaRPhotonObject    = kw.get('maxDeltaRPhotonObject', 0.5)
        self.keepObjectsNearMuons     = kw.get('keepObjectsNearMuons', False)
        self.maxDeltaRMuonObject      = kw.get('maxDeltaRMuonObject', 0.5)
        self.keepObjectsNearTaus      = kw.get('keepObjectsNearTaus', False)
        self.maxDeltaRTauObject       = kw.get('maxDeltaRTauObject', 0.5)
        self.keepObjectsNearJets      = kw.get('keepObjectsNearJets', False)
        self.maxDeltaRJetObject       = kw.get('maxDeltaRJetObject', 0.5)
        self.keepIsolatedObjects      = kw.get('keepIsolatedObjects', False)
        self.minPtIsolatedObjects     = kw.get('minPtIsolatedObjects', 5.0*Units.GeV)
        self.keepObjectsFromEGamma    = kw.get('keepObjectsFromEGamma', False)
        self.useDeltaEtaPhi           = kw.get('useDeltaEtaPhi', False)
        self.maxDeltaEtaEGammaObject  = kw.get('maxDeltaEtaEGammaObject', 1.5)
        self.maxDeltaPhiEGammaObject  = kw.get('maxDeltaPhiEGammaObject', 0.3)
        ## thinning-svc name
        self.thinSvc                  = kw.get('thinSvc', 'ThinningSvc/EGammaThinningSvc')


        # Initialize some services to None
        self.storeGateSvc = None
        
        return

    

    def initialize(self):
        self.msg.info( '************************************' )
        self.msg.info( '==> initialize %s...' % self.name() )
        self.msg.info( 'Will apply the following selections:' )
        self.msg.info( '  using ThinCollectionType       = %r', self.thinCollectionType )
        self.msg.info( '  using ThinCollectionName       = %r', self.thinCollectionName )
        self.msg.info( '  using JetCollectionType        = %r', self.jetCollectionType )
        self.msg.info( '  using JetCollectionNames       = %r', self.jetCollectionNames )
        self.msg.info( '  using ElectronCollectionType   = %r', self.electronCollectionType )
        self.msg.info( '  using ElectronCollectionName   = %r', self.electronCollectionName )
        self.msg.info( '  using PhotonCollectionType     = %r', self.photonCollectionType )
        self.msg.info( '  using PhotonCollectionName     = %r', self.photonCollectionName )
        self.msg.info( '  using MuonCollectionType       = %r', self.muonCollectionType )
        self.msg.info( '  using MuonCollectionName       = %r', self.muonCollectionName )
        self.msg.info( '  using TauCollectionType        = %r', self.tauCollectionType )
        self.msg.info( '  using TauCollectionName        = %r', self.tauCollectionName )
        self.msg.info( '  minimum Electron E_T           = %r', self.minEtElectron)
        self.msg.info( '  minimum Photon E_T             = %r', self.minEtPhoton)
        self.msg.info( '  minimum Muon E_T               = %r', self.minEtMuon)
        self.msg.info( '  minimum Tau E_T                = %r', self.minEtTau)
        self.msg.info( '  minimum Jet E_T                = %r', self.minEtJet)
        self.msg.info( '  keep objects near Lvl1 RoIs    = %r', self.keepObjectsNearLvl1Rois)
        self.msg.info( '  Lvl1 RoI names to use          = %r', self.Lvl1RoiNames)
        self.msg.info( '  maximum DeltaR Lvl1 RoI-Object = %r', self.maxDeltaRLvl1RoiObject)
        self.msg.info( '  keep objects near electrons    = %r', self.keepObjectsNearElectrons)
        self.msg.info( '  maximum DeltaR Electron-Object = %r', self.maxDeltaRElectronObject)
        self.msg.info( '  keep objects near photons      = %r', self.keepObjectsNearPhotons)
        self.msg.info( '  maximum DeltaR Photon-Object   = %r', self.maxDeltaRPhotonObject)
        self.msg.info( '  keep objects near muons        = %r', self.keepObjectsNearMuons)
        self.msg.info( '  maximum DeltaR Muon-Object     = %r', self.maxDeltaRMuonObject)
        self.msg.info( '  keep objects near taus         = %r', self.keepObjectsNearTaus)
        self.msg.info( '  maximum DeltaR Tau-Object      = %r', self.maxDeltaRTauObject)
        self.msg.info( '  keep objects near jets         = %r', self.keepObjectsNearJets)
        self.msg.info( '  maximum DeltaR Jet-Object      = %r', self.maxDeltaRJetObject)
        self.msg.info( '  keep isolated Objects          = %r', self.keepIsolatedObjects)
        self.msg.info( '  minimum pt of isolated Objects = %r', self.minPtIsolatedObjects)
        self.msg.info( '  keep Objects from EGamma       = %r', self.keepObjectsFromEGamma)
        self.msg.info( '  use also deltaEta and deltaPhi = %r', self.useDeltaEtaPhi)
        self.msg.info( '  maximum DeltaEta EGamma-Object = %r', self.maxDeltaEtaEGammaObject)
        self.msg.info( '  maximum DeltaPhi EGamma-Object = %r', self.maxDeltaPhiEGammaObject)
        self.msg.info( '  thinning service name          = %r', self.thinSvc)

        ## Declare some counters and initialize them to zero
        self.nProcessed        = 0
        self.nObjectsProcessed = 0
        self.nObjectsKept      = 0
        self.nObjectsRemoved   = 0

        # Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        # Import the Thinning Service
        self.thinningSvc = PyAthena.py_svc( self.thinSvc )
        if not self.thinningSvc:
            self.msg.error("Could not retrieve ThinningSvc with name %s !" % self.thinSvc)
            return StatusCode.Failure

        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR , 'deltaEta' : PyAthena.P4Helpers.deltaEta , 'deltaPhi' : PyAthena.P4Helpers.deltaPhi }

        ## Import needed modules
        import PyUtils.RootUtils as ru
        ROOT = ru.import_root()
        self.std = ROOT.std
        # Get enum for tracking enum
        PyCintex.loadDict("libTrkTrackSummaryDict")
        self.trk = ROOT.Trk

        return StatusCode.Success




    def execute(self):
        self.nProcessed+=1
        self.msg.debug( '==> execute %s on %r. event...' % (self.name(), self.nProcessed) )
                
        # Retrieve containers
        thinColl    = self.storeGateSvc.retrieve(self.thinCollectionType, self.thinCollectionName)
        if self.keepObjectsNearElectrons :
            try:
                electronColl = self.storeGateSvc.retrieve(self.electronCollectionType, self.electronCollectionName)
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.electronCollectionName )           
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        if self.keepObjectsNearPhotons :
            try:
                photonColl   = self.storeGateSvc.retrieve(self.photonCollectionType, self.photonCollectionName)
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.photonCollectionName )           
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        if self.keepObjectsNearMuons :
            try:
                muonColl     = self.storeGateSvc.retrieve(self.muonCollectionType, self.muonCollectionName)
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.muonCollectionName )           
                self.setFilterPassed(False)
                return StatusCode.Success
            pass
        if self.keepObjectsNearTaus :
            try:
                tauColl      = self.storeGateSvc.retrieve(self.tauCollectionType, self.tauCollectionName)
            except LookupError:
                self.msg.warning( 'Collection %s not found' % self.tauCollectionName )           
                self.setFilterPassed(False)
                return StatusCode.Success
            pass

        # test that we got all containers
        if not thinColl :
            self.msg.warning("Failed to retrieve ThinContainer at %s" % self.thinCollectionName )
            return StatusCode.Success
        if self.keepObjectsNearElectrons and not electronColl :
            self.msg.warning("Failed to retrieve ElectronContainer at %s" % self.electronCollectionName )
            return StatusCode.Success
        if self.keepObjectsNearPhotons and not photonColl :
            self.msg.warning("Failed to retrieve PhotonContainer at %s" % self.photonCollectionName )
            return StatusCode.Success
        if self.keepObjectsNearMuons and not muonColl :
            self.msg.warning("Failed to retrieve MuonContainer at %s" % self.muonCollectionName )
            return StatusCode.Success
        if self.keepObjectsNearTaus and not tauColl :
            self.msg.warning("Failed to retrieve TauContainer at %s" % self.tauCollectionName )
            return StatusCode.Success

        # Declare the utility to calculate the DeltaR between two objects
        deltaR   = self.utils['deltaR']
        deltaEta = self.utils['deltaEta']
        deltaPhi = self.utils['deltaPhi']

        # Initialize a list (dictionary actually, for speed reasons) of good objects (e.g. tracks, cells, clusters...)
        goodObjectsDict = {}

        # Search for all objects near passed Level 1 RoIs and store them in a list
        if self.keepObjectsNearLvl1Rois :
            goodEmTauRois = []
            # Get the Lvl1RoIs
            Lvl1Rois = PyTriggerTools.getLVL1_ROI("LVL1_ROI")
            if not Lvl1Rois :
                self.msg.info("Failed to retrieve Level 1 RoIs")
                pass
            else :
                emTauRois = Lvl1Rois.getEmTauROIs()
                for roi in emTauRois:
                    for roiIndex in xrange( roi.getThresholdNames().size() ) :
                        for i in xrange( len( self.Lvl1RoiNames ) ) :
                            if roi.getThresholdNames()[roiIndex] == self.Lvl1RoiNames[i] :
                                goodEmTauRois += [ roi ]
                                for j in xrange( thinColl.size() ) :
                                    deltaRLvl1RoiObject = deltaR( roi, thinColl.at(j) )
                                    if deltaRLvl1RoiObject < self.maxDeltaRLvl1RoiObject :
                                        goodObjectsDict[j] = 1
                                        pass
                                    pass
                                pass
                            pass
                        pass
                    pass
                pass
            pass
        
        # Search for all objects near good electrons and store them in a list
        if self.keepObjectsNearElectrons :
            for i in xrange(electronColl.size() ) :
                trackParticleFromElectron = 0
                if electronColl.at(i).trackParticleElementLink().isValid() :
                    trackParticleFromElectron = electronColl.at(i).trackParticle()
                    pass
                for j in xrange( thinColl.size() ) :
                    # Keep TrackParticles used by the electron
                    if self.keepObjectsFromEGamma and electronColl.at(i).trackParticleElementLink().isValid() :
                        if trackParticleFromElectron.__hash__() == thinColl.at(j).__hash__() :
                            goodObjectsDict[j] = 1
                            pass
                        pass
                    # Keep objects near electrons
                    if electronColl.at(i).et() > self.minEtElectron :
                        deltaRElectronObject = deltaR( electronColl.at(i), thinColl.at(j) )
                        deltaEtaElectronObject = deltaEta( electronColl.at(i), thinColl.at(j) )
                        deltaPhiElectronObject = deltaPhi( electronColl.at(i), thinColl.at(j) )
                        if deltaRElectronObject < self.maxDeltaRElectronObject :
                            goodObjectsDict[j] = 1
                            pass
                        # Keep Track Particles in a broader window if they have less than 5 hits in the pixel+SCT
                        if self.useDeltaEtaPhi :
                            trackSummary = thinColl.at(j).trackSummary()
                            # The trackSummary enumerates are defined here:
                            # http://atdaq-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Tracking/TrkEvent/TrkTrackSummary/TrkTrackSummary/TrackSummary.h?view=log
                            nPixelHits = trackSummary.get(self.trk.numberOfPixelHits)
                            nSCTHits   = trackSummary.get(self.trk.numberOfSCTHits)
                            if (nPixelHits + nSCTHits) < 5 :
                                if deltaEtaElectronObject < self.maxDeltaEtaEGammaObject \
                                       and deltaPhiElectronObject < self.maxDeltaPhiEGammaObject :
                                    goodObjectsDict[j] = 1
                                    pass
                                pass
                            pass
                        pass
                    pass
                # Get enum for tracking enum
                #PyCintex.loadDict("libTrkTrackSummaryDict")
                #trk = ROOT.Trk
                #trackSummary  = trackParticle.trackSummary()
                # The trackSummary enumerates are defined here:
                # http://atdaq-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Tracking/TrkEvent/TrkTrackSummary/TrkTrackSummary/TrackSummary.h?view=log
                #nPixelHits = trackSummary.get(trk.numberOfPixelHits)
                #nSCTHits   = trackSummary.get(trk.numberOfSCTHits)
                ##nPixelHits = trackSummary.get(2)
                ##nSCTHits   = trackSummary.get(3)
                pass
            pass
        
        # Search for all objects near good photons and store them in a list
        if self.keepObjectsNearPhotons :
            for i in xrange(photonColl.size() ) :
                trackParticleFromPhoton = 0
                if photonColl.at(i).trackParticleElementLink().isValid() :
                    trackParticleFromPhoton = photonColl.at(i).trackParticle()
                    pass
                for j in xrange( thinColl.size() ) :
                    # Keep TrackParticles used by the photon
                    if self.keepObjectsFromEGamma and photonColl.at(i).trackParticleElementLink().isValid() :
                        if trackParticleFromPhoton.__hash__() == thinColl.at(j).__hash__() :
                            goodObjectsDict[j] = 1
                            pass
                        pass
                    # Keep objects near photons
                    if photonColl.at(i).et() > self.minEtPhoton :
                        deltaRPhotonObject = deltaR( photonColl.at(i), thinColl.at(j) )
                        deltaEtaPhotonObject = deltaEta( photonColl.at(i), thinColl.at(j) )
                        deltaPhiPhotonObject = deltaPhi( photonColl.at(i), thinColl.at(j) )
                        if deltaRPhotonObject < self.maxDeltaRPhotonObject :
                            goodObjectsDict[j] = 1
                            pass
                        # Keep Track Particles in a broader window if they have less than 5 hits in the pixel+SCT
                        if self.useDeltaEtaPhi :
                            trackSummary = thinColl.at(j).trackSummary()
                            # The trackSummary enumerates are defined here:
                            # http://atdaq-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/Tracking/TrkEvent/TrkTrackSummary/TrkTrackSummary/TrackSummary.h?view=log
                            nPixelHits = trackSummary.get(self.trk.numberOfPixelHits)
                            nSCTHits   = trackSummary.get(self.trk.numberOfSCTHits)
                            if (nPixelHits + nSCTHits) < 5 :
                                if deltaEtaPhotonObject < self.maxDeltaEtaEGammaObject \
                                       and deltaPhiPhotonObject < self.maxDeltaPhiEGammaObject :
                                    goodObjectsDict[j] = 1
                                    pass
                                pass
                            pass
                        pass
                    pass
                pass
            pass

        # Search for all objects near good muons and store them in a list
        if self.keepObjectsNearMuons :
            for i in xrange(muonColl.size() ) :
                if muonColl.at(i).et() > self.minEtMuon :
                    for j in xrange( thinColl.size() ) :
                        deltaRMuonObject = deltaR( muonColl.at(i), thinColl.at(j) )
                        if deltaRMuonObject < self.maxDeltaRMuonObject :
                            goodObjectsDict[j] = 1
                            pass
                        pass
                    pass
                pass
            pass

        # Search for all objects near good taus and store them in a list
        if self.keepObjectsNearTaus :
            for i in xrange(tauColl.size() ) :
                if tauColl.at(i).et() > self.minEtTau :
                    for j in xrange( thinColl.size() ) :
                        deltaRTauObject = deltaR( tauColl.at(i), thinColl.at(j) )
                        if deltaRTauObject < self.maxDeltaRTauObject :
                            goodObjectsDict[j] = 1
                            pass
                        pass
                    pass
                pass
            pass

        # Search for all objects near good jets and store them in a list
        if self.keepObjectsNearJets :
            for jetCollName in self.jetCollectionNames :
                # Get the jet collections from StoreGate
                try :
                    # try to get the jet collection the user wants
                    jetColl = self.storeGateSvc.retrieve( self.jetCollectionType, jetCollName )
                    pass
                except LookupError :
                    # If that didn't work, try to analyze if the user wanted the old ParticleJets
                    # or the new (starting from release 14.1.0) Jets. Also cover the case that the
                    # user gave the wrong type for the given input file.
                    if jetCollName.__contains__('Particle') : # user gave old ParticleJets
                        try :
                            jetColl = self.storeGateSvc.retrieve( 'ParticleJetContainer', jetCollName )
                            pass
                        except LookupError :
                            jetColl = self.storeGateSvc.retrieve( 'JetCollection', jetCollName.replace('Particle','') )
                            pass
                        pass
                    else : # User did NOT give old ParticleJets, assume new Jets are given.
                        try :
                            jetColl = self.storeGateSvc.retrieve( 'JetCollection', jetCollName )
                            pass
                        except LookupError :
                            jetColl = self.storeGateSvc.retrieve( 'ParticleJetContainer', jetCollName.replace('Jets','ParticleJets') )
                            pass
                        pass
                    pass

                # Check that we have a valid jet collection
                if not jetColl :
                    self.msg.info("Failed to retrieve JetContainer at %s" % jetCollName )
                    return StatusCode.Success

                # Now that we have the right jet collections, find objects near these jets
                for i in xrange(jetColl.size() ) :
                    if jetColl.at(i).et() > self.minEtJet :
                        for j in xrange( thinColl.size() ) :
                            deltaRJetObject = deltaR( jetColl.at(i), thinColl.at(j) )
                            if deltaRJetObject < self.maxDeltaRJetObject :
                                goodObjectsDict[j] = 1
                                pass
                            pass
                        pass
                    pass
                pass
            pass
        
        # Also, keep isolated objects above a certain pt:
        if self.keepIsolatedObjects :
            for i in xrange( thinColl.size() ) :
                if thinColl.at(i).pt() > self.minPtIsolatedObjects :
                    goodObjectsDict[j] = 1
                    pass
                pass
            pass


        # create the vector that will tell the ThinningService which objects to keep
        thin_mask = []
        nBeforeThin = thinColl.size()
        nGood = 0
        nBad = 0
        for j in range(thinColl.size()) :
            if  goodObjectsDict.has_key(j):
                thin_mask += [True]
                nGood += 1
                pass
            else:
                thin_mask += [False]
                nBad += 1
                pass
            pass
        
        # Update the counters
        self.nObjectsProcessed += nBeforeThin
        self.nObjectsKept      += nGood
        self.nObjectsRemoved   += nBad

        # Do the actual thinning of the objects
        if self.thinningSvc.filter(thinColl,thin_mask) != StatusCode.Success :
            self.msg.info("Problem while thinning the objects with name %s" % self.thinCollectionName )
            pass
        nAfterThin = len([m for m in thin_mask if m])

        # Some debug messages
        self.msg.debug( 'Thinning statistics for %s: keeping %s objects and rejecting %s objects' % ( self.name(), nGood, nBad ) )
        self.msg.debug( 'Size of thinning mask: %s' % len(thin_mask) )
        self.msg.debug( 'Thinning mask elements: %s' % (thin_mask) )
        self.msg.debug( 'Number of objects before thinning: %s, and after thinning: %s' % (nBeforeThin, nAfterThin) )

        return StatusCode.Success



    def finalize(self):
        self.msg.info( '==> finalize %s...' % self.name() )
        self.msg.info( '***************************************************************' )
        self.msg.info( 'Results of %s thinning algorithm:' % self.name() )
        self.msg.info( '-------------' )
        self.msg.info( ' Number of processed events:  %r' % self.nProcessed )
        self.msg.info( ' Number of %s objects processed:                   %s' % (self.thinCollectionName, self.nObjectsProcessed) )
        self.msg.info( ' Number of %s objects kept:                        %s' % (self.thinCollectionName, self.nObjectsKept) )
        self.msg.info( ' Number of %s objects removed:                     %s' % (self.thinCollectionName, self.nObjectsRemoved) )
        try:
            self.msg.info( ' Average per event number of %s objects processed: %6.3f' % (self.thinCollectionName, 1.0*self.nObjectsProcessed/self.nProcessed) )
            self.msg.info( ' Average per event number of %s objects kept:      %6.3f' % (self.thinCollectionName, 1.0*self.nObjectsKept/self.nProcessed) )
            self.msg.info( ' Average per event number of %s objects removed:   %6.3f' % (self.thinCollectionName, 1.0*self.nObjectsRemoved/self.nProcessed) )
            self.msg.info( ' Average percent of %s objects kept:               %6.3f%%' % (self.thinCollectionName, 100.0*self.nObjectsKept/self.nObjectsProcessed) )
            pass
        except ZeroDivisionError :
            self.msg.warning( 'Division by zero error when calculating the average kept rates...' )
            pass
        
        return StatusCode.Success
