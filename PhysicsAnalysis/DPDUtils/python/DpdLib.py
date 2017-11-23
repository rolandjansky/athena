# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: DPDUtils/python/DpdLib.py
# @purpose: a set of py-components to exercize DPD making with PyAthena
# @author: Sebastien Binet <binet@cern.ch>

__doc__ = """a set of py-components to exercize DPD making with PyAthena"""
__version__ = "$Revision: 1.11 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class SimpleElectronFilter( PyAthena.Alg ):
    """Filtering electrons with PyAthena
    """
    def __init__(self, name = "SimpleElectronFilter", **kw):
        ## init base class
        kw['name'] = name
        super(SimpleElectronFilter,self).__init__(**kw)

        ## Et threshold for the electron filter
        self.minEt = kw.get('MinEt', 20.*Units.GeV)
        ## StoreGate key for electron collection
        self.electronsName = kw.get('Electrons', 'ElectronAODCollection')
        
    def initialize(self):
        self.msg.info("==> initializing [%s]...", self.name())
        self.msg.info( "MinEt: %r GeV/c", self.minEt/Units.GeV)
        self.sg = PyAthena.py_svc("StoreGateSvc")
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc")
            return StatusCode.Failure
        return StatusCode.Success

    def execute(self):
        self.setFilterPassed(False)
        eles = self.sg.retrieve('ElectronContainer',self.electronsName)
        if not eles:
            self.msg.info('Could not retrieve ElectronContainer at %s',
                          self.electronsName)
            return StatusCode.Success

        if len(eles) < 4:
            # not enough electrons
            return StatusCode.Success

        # checking Et of these electrons
        filtered_eles = [e for e in eles if e.et() > self.minEt]
        if len(filtered_eles) >= 2:
            self.setFilterPassed(True)
        
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

    pass # class ElectronFilter

## -- Simple semileptonic ttbar filter ----------------------------------------
class ttbarFilter(PyAthena.Alg):
    """
    A simple semileptonic ttbar filter algorithm
    This filter looks for:
      - exactly 1 lepton with E_t > 20 GeV and |eta|<2.5
      - at least 4 jets with |eta| <2.5, E_t>20.GeV not overlapping with
        electrons
      - missing E_t > 20 GeV
    """

    def __init__(self, name = "ttbarFilter", **kw):
        ## init base class
        kw['name'] = name
        super(ttbarFilter,self).__init__(**kw)

        ## lepton's cuts
        self.leptonMinEt  = kw.get('LeptonMinEt',  20.*Units.GeV)
        self.leptonMaxEta = kw.get('LeptonMaxEta', 2.5)

        ## jets' cuts
        self.jetMinEt  = kw.get('JetMinEt',  20.*Units.GeV)
        self.jetMaxEta = kw.get('JetMaxEta', 2.5)

        ## lepton/jet isolation cut
        self.minDeltaRlj = kw.get('MinDeltaRlj', 0.1)
        
        ## missing et cut
        self.minMissingEt = kw.get('MinMissingEt', 20.*Units.GeV)

        ## cluster isolation cut
        self.maxDeltaRcx = kw.get('MaxDeltaRcx', 0.4)
        
        ## track isolation cut
        self.maxDeltaRtx = kw.get('MaxDeltaRtx', 0.4)
        
        ## StoreGate keys
        self.electronsName = kw.get('Electrons',    'ElectronAODCollection')
        self.missingEtName = kw.get('MissingEt',    'MET_RefFinal')
        self.jetsName      = kw.get('Jets',         'Kt4H1TopoJets')
        self.muonsName     = kw.get('Muons',        'StacoMuonCollection')
        self.clustersName  = kw.get('CaloClusters', 'CaloCalTopoCluster')
        self.tracksName    = kw.get('Tracks',       'TrackParticleCandidate')
        
    def initialize(self):
        _info = self.msg.info
        _info("==> initializing [%s]...", self.name())
        _info("Lepton MinEt:   %r GeV/c", self.leptonMinEt/Units.GeV)
        _info("Lepton MaxEta:  %r",       self.leptonMaxEta)
        _info("Jets MinEt:     %r GeV/c", self.jetMinEt/Units.GeV)
        _info("Jets MaxEta:    %r",       self.jetMaxEta)
        _info("Min Missing Et: %r GeV/c", self.minMissingEt/Units.GeV)

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc !")
            return StatusCode.Failure

        self.thSvc = PyAthena.py_svc('ThinningSvc')
        if not self.thSvc:
            self.msg.error("Could not retrieve ThinningSvc !")
            return StatusCode.Failure
        
        ## import some 4-mom utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }
        
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        self.setFilterPassed(False)
        _retrieve = self.sg.retrieve
        met  = _retrieve('MissingET',               self.missingEtName)
        eles = _retrieve('ElectronContainer',       self.electronsName)
        #jets = _retrieve('ParticleJetContainer', self.jetsName)
        jets = _retrieve('JetCollection',           self.jetsName)
        muons= _retrieve('Analysis::MuonContainer', self.muonsName)

        # test that we got all containers
        if not all([met,eles,jets,muons]):
            _info("Failed to retrieve one of the containers")
            return StatusCode.Success

        elejets  = [] # electrons overlapping with jets
        goodeles = [] # good ones
        goodmuons= [] # good muons
        goodjets = [] # ...

        deltaR = self.utils['deltaR']

        def lepton_filter(lepton,
                          maxEta=self.leptonMaxEta,
                          minEt =self.leptonMinEt):
            return abs(lepton.eta()) < maxEta and \
                         lepton.et() > minEt
        
        # count electrons and mark overlaps with jets
        ele_filter = lambda e: e.author(1) and e.isem(0x77FFF)==0
        for ele in eles:
            if not ele_filter(ele):
                continue
            for jet in jets:
                dRej = deltaR(ele, jet)
                if dRej < self.minDeltaRlj and \
                   jet.et() < 2*ele.et():
                    _info('Jet match: %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f',
                          jet.et(), jet.eta(), jet.phi(),
                          ele.et(), ele.eta(), ele.phi())
                    # do not use '+=' as it is less efficient
                    elejets.append (jet)
            if lepton_filter(ele):
                # do not use '+=' as it is less efficient
                goodeles.append (ele)

        # count jets not consisting primarily of an isolated electron
        goodjets = [ jet for jet in jets
                     if not (jet in elejets) and \
                        jet.et() > self.jetMinEt and \
                        abs(jet.eta()) < self.jetMaxEta ]

        # count muons
        goodmuons = [ muon for muon in muons if lepton_filter(muon) ]
            
        # check missing Et
        metCut = met.et() > self.minMissingEt

        # signature for semileptonic ttbar decays:
        #  - at least 4 high Et jets not overlapping with electrons
        #  - 1 good isolated high Et lepton
        #  - high Missing Et
        filterPassed = (len(goodeles)+len(goodmuons)) == 1 and \
                       len(goodjets) >= 4 and \
                       metCut
        self.setFilterPassed(filterPassed)

        if not filterPassed:
            _info('event failed semileptonic ttbar filter')
            return StatusCode.Success

        _info('event passed semileptonic ttbar filter')

        # a collection of all good 4-mom
        good_i4mom = goodeles + goodmuons + goodjets

        # ---------------------------------------------------------------
        # filter clusters
        _info('thinning clusters...')
        clusters = _retrieve('CaloClusterContainer', self.clustersName)
        if not clusters:
            _info('Could not retrieve clusters at [%r]', self.clustersName)
            return StatusCode.Success
        goodclusters = []
        
        # keep those close to good electrons,muons or jets
        for p in good_i4mom:
            goodclusters += [
                cluster for cluster in clusters
                if deltaR(cluster, p) < self.maxDeltaRcx 
                ]

        # keep also those contained in jets 
        for j in jets:
            goodclusters += [
                cluster for cluster in clusters
                if j.contains(cluster)
                ]

##         # FIXME: use python's builtin set when bug #38763 resolved
##         # https://savannah.cern.ch/bugs/index.php?38763
##         # remove possible duplicates
##         goodclusters = set(goodclusters)


        # apply thinning
        def cluster_filter(cluster):
            return cluster in goodclusters
        cluster_mask = [ cluster_filter(c) for c in clusters ]
        
        if self.thSvc.filter(clusters, cluster_mask) != StatusCode.Success:
            _info('Problem thinning clusters !')

        # ---------------------------------------------------------------
        # filter track particles
        _info('thinning tracks...')
        tracks = _retrieve('Rec::TrackParticleContainer', self.tracksName)
        if not tracks:
            _info('Could not retrieve tracks at [%r]', self.tracksName)
            return StatusCode.Success
        goodtracks = []

        # keep those close to good electrons, muons or jets
        for p in good_i4mom:
            goodtracks += [
                track for track in tracks \
                if deltaR(track, p) < self.maxDeltaRtx
                ]

##         # FIXME: use python's builtin set when bug #38763 resolved
##         # https://savannah.cern.ch/bugs/index.php?38763
##         # remove possible duplicates
##         goodtracks = set(goodtracks)
        
        # thin the tracks according to the goodtracks selection
        def track_filter(track):
            return track in goodtracks
        track_mask = [ track_filter(track) for track in tracks ]
        if self.thSvc.filter(tracks, track_mask) != StatusCode.Success:
            _info('Problem while thinning tracks')

        _info('completed filtering')
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

    pass # class ttbarFilter

## -- Jet slimmer -------------------------------------------------------------
class JetSlimmer(PyAthena.Alg):
    """
    A simple algorithm to slim jets' content (removing parts of the Jet)
    """

    def __init__(self, name = "JetSlimmer", **kw):
        ## init base class
        kw['name'] = name
        super(JetSlimmer,self).__init__(**kw)

        ## jet's constituents to remove
        self.vetoConstituents = kw.get('VetoConstituents', ['Tracks'])

        ## jet's informations to keep
        self.keepInfoKeys = kw.get('KeepInfoKeys', ['IP3D', 'SV1'])
        
        ## StoreGate keys
        self.jetsName = kw.get('JetsInput',
                               ['Cone4H1TowerJets', 'Cone4TruthJets'])
        
    def initialize(self):
        _info = self.msg.info
        _info("==> initializing [%s]...", self.name())
        _info("Vetoing constituents: %r", self.vetoConstituents)
        _info("Keeping InfoKeys:     %r", self.keepInfoKeys)

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc !")
            return StatusCode.Failure

        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _retrieve = self.sg.retrieve
        for jet_key in self.jetsName:
            jets = _retrieve('JetCollection', jet_key)
            if not jets:
                _info('Could not retrieve jets at [%r]', jet_key)
                continue
            for jet in jets:
                map(jet.removeAssociation, self.vetoConstituents)
                map(jet.removeInfo, (k for k in jet.infoKeys()
                                     if k not in self.keepInfoKeys))
                pass
            pass

        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

    pass # class JetSlimmer


## -- Track slimmer -----------------------------------------------------------
class TrackSlimmer(PyAthena.Alg):
    """
    A simple algorithm to slim tracks' content (removing parts of the Track)
    """

    def __init__(self, name = "TrackSlimmer", **kw):
        ## init base class
        kw['name'] = name
        super(TrackSlimmer,self).__init__(**kw)

        ## pt cut for applying the slimming
        self.trackMinPt = kw.get('MinPt', 5.*Units.GeV)

        ## sg keys
        self.tracksName = kw.get('Tracks', 'TrackParticleCandidate')

        return

    def initialize(self):
        _info = self.msg.info
        _info("==> initializing [%s]...", self.name())
        _info("Tracks: %r",       self.tracksName)
        _info("Min Pt: %r GeV/c", self.trackMinPt/Units.GeV)

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc !")
            return StatusCode.Failure

        return StatusCode.Success

    def execute(self):

        tracks = self.sg.retrieve('Rec::TrackParticleContainer',
                                  self.tracksName)
        if not tracks:
            self.msg.info('Could not retrieve tracks at [%r]', self.tracksName)
            return StatusCode.Success

        tracks = (track for track in tracks if track.pt() < self.trackMinPt)
        for track in tracks:
            track.removeSummary()
            track.removeErrorMatrix()

        return StatusCode.Success
        
    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

    pass # class TrackSlimmer

## -- Electron filter --------------------------------------------------------
class ElectronFilter(PyAthena.Alg):
    """A py-algorithm to filter/thin electrons and slim+thin tracks
    """
    def __init__(self, name='ElectronFilter', **kw):
        ## init base class
        kw['name'] = name
        super(ElectronFilter,self).__init__(**kw)

        ## cuts
        self.eleMinPt   = kw.get('EleMinPt',   10.*Units.GeV)
        self.eleMaxEta  = kw.get('EleMaxEta',   2.5)
        self.elePidCut  = kw.get('ElePidCut',  'ElectronLoose')
        self.trackMinPt = kw.get('TrackMinPt',  3.*Units.GeV)
        self.maxDeltaRte= kw.get('MaxDeltaRte', 0.4)
        
        ## sg keys
        self.electronsName = kw.get('Electrons', 'ElectronAODCollection')
        self.tracksName    = kw.get('Tracks',    'TrackParticleCandidate')

        return

    def initialize(self):
        _info = self.msg.info
        _info("==> initializing [%s]...", self.name())
        _info("Electrons: %r", self.electronsName)
        _info("Min Pt:    %r GeV/c", self.eleMinPt/Units.GeV)
        _info("Max |eta|: %r",       self.eleMaxEta)
        _info("PID cut:   %r",       self.elePidCut)
        _info("-"*40)
        _info("Tracks: %r",       self.tracksName)
        _info("Min Pt: %r GeV/c", self.trackMinPt/Units.GeV)
        _info("Max deltaR(track,ele): %r", self.maxDeltaRte)

        self.sg = PyAthena.py_svc('StoreGateSvc')
        if not self.sg:
            self.msg.error("Could not retrieve StoreGateSvc")
            return StatusCode.Failure

        self.thSvc = PyAthena.py_svc("ThinningSvc")
        if not self.thSvc:
            self.msg.error("Could not retrieve ThinningSvc")
            return StatusCode.Failure

        # get the C++ enum...
        # we have to explicitly load the dictionary holding the enum b/c
        # of a gccxml/reflex bug
        import cppyy
        egdict = cppyy.loadDict("egammaEventDict")
        self.elePidCut = getattr(PyAthena.egammaPID, self.elePidCut)

        # get some C++ utils
        import FourMomUtils.Bindings
        self.utils = { 'deltaR' : PyAthena.P4Helpers.deltaR }

        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _retrieve = self.sg.retrieve
        self.setFilterPassed(False)
        
        ## -- thin electrons --------------------------------------------------
        eles = _retrieve('ElectronContainer', self.electronsName)
        if not eles:
            _info("Could not retrieve electrons at [%r]", self.electronsName)
            return StatusCode.Success

        def ele_cut(e):
            return e.author(1) and (e.isem(self.elePidCut)==0) and \
                   e.pt() > self.eleMinPt and abs(e.eta()) < self.eleMaxEta
        goodeles = [ e for e in eles if ele_cut(e) ]

        filterPassed = len(goodeles)>1
        self.setFilterPassed(filterPassed)
        if filterPassed:
            _info('event passed filtering cuts')
        else:
            _info('event failed filtering cuts')
            pass
        
        ele_mask = [ ele_cut(e) for e in eles ]
        if self.thSvc.filter(eles, ele_mask) != StatusCode.Success:
            _info("Problem while thinning electrons")
        
        ## -- thin tracks -----------------------------------------------------
        tracks = self.sg.retrieve('Rec::TrackParticleContainer',
                                  self.tracksName)
        if not tracks:
            self.msg.info('Could not retrieve tracks at [%r]', self.tracksName)
            return StatusCode.Success

        deltaR = self.utils['deltaR']
        goodtracks = []
        for ele in goodeles:
            goodtracks += [ track for track in tracks
                            if deltaR(track,ele) < self.maxDeltaRte ]
##         # FIXME: use python's builtin set when bug #38763 resolved
##         # https://savannah.cern.ch/bugs/index.php?38763
##         # remove duplicates
##         goodtracks = set(goodtracks)

        track_mask = [ True if track in goodtracks else False \
                       for track in tracks ]
        if self.thSvc.filter(tracks, track_mask) != StatusCode.Success:
            _info("Problem while thinning tracks")

        ## -- slim tracks ---
        for track in goodtracks:
            if track.pt() < self.trackMinPt:
                track.removeSummary()
                track.removeErrorMatrix()

        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

    pass # class ElectronFilter
    
