#
# simple semileptonic tt-bar filter
#
# Author: Sven Menke
#
# the filter looks for
# exactly 1 lepton with E_T > 20 GeV, |eta| < 2.5;
# at least 4 jets with |eta| < 2.5, E_T > 20 GeV, not overlapping with electrons
# missing E_T > 20 GeV
# 
from GaudiPython import PyAlgorithm

class ttbarFilterAlgorithm( PyAlgorithm ):
    def __init__ ( self, name, **kw ) :
        PyAlgorithm.__init__(self,name)
        self.name = name

        ## lepton's cuts
        self.leptonMinEt  = kw.get('LeptonMinEt',  20.*GeV)
        self.leptonMaxEta = kw.get('LeptonMaxEta', 2.5)

        ## jets' cuts
        self.jetMinEt  = kw.get('JetMinEt',  20.*GeV)
        self.jetMaxEta = kw.get('JetMaxEta', 2.5)

        ## lepton/jet isolation cut
        self.minDeltaRlj = kw.get('MinDeltaRlj', 0.1)
        
        ## missing et cut
        self.minMissingEt = kw.get('MinMissingEt', 20.*GeV)

        ## cluster isolation cut
        self.maxDeltaRcx = kw.get('MaxDeltaRcx', 0.4)
        
        ## track isolation cut
        self.maxDeltaRtx = kw.get('MaxDeltaRtx', 0.4)
        
        ## StoreGate keys
        self.electronsName = kw.get('Electrons',    'ElectronAODCollection')
        self.missingEtName = kw.get('MissingEt',    'MET_RefFinal')
        self.jetsName      = kw.get('Jets',         'Kt4LCTopoJets')
        self.muonsName     = kw.get('Muons',        'StacoMuonCollection')
        self.clustersName  = kw.get('CaloClusters', 'CaloCalTopoCluster')
        self.tracksName    = kw.get('Tracks',       'TrackParticleCandidate')

    def initialize(self):
        print "==> initializing ", self.name
        print "Electrons:      ",       self.electronsName
        print "Muons:          ",       self.muonsName
        print "Jets:           ",       self.jetsName
        print "Clusters:       ",       self.clustersName
        print "Tracks:         ",       self.tracksName
        print "MissingET:      ",       self.missingEtName
        print "Lepton MinEt:   ",       self.leptonMinEt/GeV,  " GeV"
        print "Lepton MaxEta:  ",       self.leptonMaxEta
        print "Jets MinEt:     ",       self.jetMinEt/GeV, " GeV"
        print "Jets MaxEta:    ",       self.jetMaxEta
        print "Min Missing Et: ",       self.minMissingEt/GeV, " GeV"
        print "Lep/Jet DeltaR: ",       self.minDeltaRlj
        print "Cluster DeltaR: ",       self.maxDeltaRcx
        print "Track   DeltaR: ",       self.maxDeltaRtx
        return True

    def execute(self):
        met = PyParticleTools.getMissingET(self.missingEtName)
        ec  = PyParticleTools.getElectrons(self.electronsName)
        jets= PyParticleTools.getJets(self.jetsName)
        muons=PyParticleTools.getMuons(self.muonsName)
        nmet = 0
        nele = 0
        njets = 0
        nmuons = 0
        elejets = []
        goodele = []
        goodmu = []
        goodjet = []
        # count electrons and mark overlap with jets
        for i in range(ec.size()) :
            if ec[i].author(1) and ec[i].isem(ROOT.egammaPID.ElectronMedium) == 0 :
                eh4=ec[i].hlv()
                for j in range(jets.size()) :
                    if jets[j].hlv().deltaR(eh4) < self.minDeltaRlj \
                           and jets[j].et() < 2*ec[i].et():
                        print 'Jet Match', jets[j].et(), jets[j].eta(), \
                              jets[j].phi(), ec[i].et(),ec[i].eta(),ec[i].phi()
                        elejets += [j]
                if abs(ec[i].eta()) < self.leptonMaxEta and ec[i].et() > self.leptonMinEt:
                    nele += 1
                    goodele += [i]
        # count jets not consisting primarily of
        # an isolated electron
        for j in range(jets.size()) :
            if not elejets.__contains__(j) \
                   and jets[j].et() > self.jetMinEt      \
                   and abs(jets[j].eta()) < self.jetMaxEta:
                njets += 1
                goodjet += [j]
        # count muons
        for i in range(muons.size()) :
            if abs(muons[i].eta()) < self.leptonMaxEta \
                   and muons[i].et() > self.leptonMinEt:
                nmuons += 1
                goodmu += [i]
        # check MET
        if met.et() > self.minMissingEt:
            nmet += 1
        # signature for semileptonic tt-bar deacys:
        #  - at least 4 high et() jets not overlapping with electrons
        #  - 1 good (isolated) high et() lepton
        #  - high MET
        if nele+nmuons == 1 and njets > 3 and nmet > 0:
            print 'Event passed semileptonic tt-bar Filter'
            self.setFilterPassed(True)
            # filter clusters
            clus = PyParticleTools.getCaloClusters(self.clustersName)
            goodclus = []
            # keep those close to good electrons
            for i in range(len(goodele)) :
                for j in range(clus.size()) :
                    if clus[j].hlv().deltaR(ec[goodele[i]].hlv()) < self.maxDeltaRcx:
                        goodclus += [j] 
            # keep those close to good muons
            for i in range(len(goodmu)) :
                for j in range(clus.size()) :
                    if clus[j].hlv().deltaR(muons[goodmu[i]].hlv()) < self.maxDeltaRcx:
                        goodclus += [j] 
            # keep those inside jets
            for i in range(len(jets)) :
                for j in range(clus.size()) :
                    if jets[i].contains(clus[j]) :
                        goodclus += [j] 
            vbool = ROOT.std.vector(bool)()    
            vbool.resize(0)
            for j in range(clus.size()) :
                if  goodclus.__contains__(j):
                    vbool.push_back(True)
                else:
                    vbool.push_back(False)
            # do the actual filtering
            ThinningSvcWrapper.filter(clus,vbool)        
            # filter track particles
            track = PyParticleTools.getTrackParticles(self.tracksName)
            goodtrack = []
            # keep those close to good electrons
            for i in range(len(goodele)) :
                for j in range(track.size()) :
                    if not track[j].hlv().deltaR(ec[goodele[i]].hlv()) < self.maxDeltaRtx:
                        goodtrack += [j] 
            # keep those close to good muons
            for i in range(len(goodmu)) :
                for j in range(track.size()) :
                    if track[j].hlv().deltaR(muons[goodmu[i]].hlv()) < self.maxDeltaRtx:
                        goodtrack += [j] 
            # keep those close to good jets
            for i in range(len(goodjet)) :
                for j in range(track.size()) :
                    if track[j].hlv().deltaR(jets[goodjet[i]].hlv()) < self.maxDeltaRtx:
                        goodtrack += [j] 
            vbool = ROOT.std.vector(bool)()    
            vbool.resize(0)
            for j in range(track.size()) :
                if  goodtrack.__contains__(j):
                    vbool.push_back(True)
                else:
                    vbool.push_back(False)
            # do the actual filtering
            ThinningSvcWrapper.filter(track,vbool)        
        else:
            print 'Event failed semileptonic tt-bar Filter'
            self.setFilterPassed(False)
        return True
            
    def finalize(self):
        return True
