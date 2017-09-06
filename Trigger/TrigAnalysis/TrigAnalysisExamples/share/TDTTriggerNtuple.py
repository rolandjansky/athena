#!/usr/bin/env python

class EmTau_RoI:
    thresholds = ['']

class InitialRoI:
    roiword = 0
    eta = 0.0
    phi = 0.0
    
class Cluster:
    triggers = ['']
    e237 = 0.0
    
class Tracks:
    triggers = ['']
    count = 0
    label = ''

class Electron:
    triggers   = ['']
    e = 0.
    charge = 0

class EgammaEntry:
    triggers = ['']
    l1roi = None
    initialroi = None
    cluster = None
    tracks = None
    electron = None
    def __str__(self):
        s = ''
        s += 'triggers: ' + str(self.triggers) + '\n'
        s += 'l1 thresholds: ' + str(self.l1roi.thresholds) + '\n'
        s += 'intialRoI eta: ' + str(self.initialroi.eta) + ' phi: ' + str(self.initialroi.phi) + '\n'
        if self.cluster == None:
            s += 'cluster: None\n'
        else:
            s += 'cluster e237: ' + str(self.cluster.e237) + ' triggers: ' + str(self.cluster.triggers)+  '\n'

        if self.tracks == None:
            s += 'tracks: None\n'
        else:
            s += 'tracks count: ' + str(self.tracks.count) + ' triggers: ' + str(self.tracks.triggers)+  '\n'


        if self.electron == None:
            s += 'electron: None\n'
        else:
            s += 'electron e: ' + str(self.electron.e) + ' charge: ' + str(self.electron.charge) + ' triggers: ' + str(self.tracks.triggers)+  '\n'

        s += 'triggers passed due to this RoI: ' + str(self.triggers) + '\n'
        s += '..................................'
        return s
    



    
### import the modules 
print '... importing ROOT'
import ROOT

### define the list of input files
print '... define list of files'
PoolAODInput=['/afs/cern.ch/user/r/redlinge/scratch0/testfiles/AOD.039563._00346.pool.root.1']
#PoolAODInput=['/afs/cern.ch/atlas/offline/external/FullChainTest/devval/last_good_root/AllBasicSamples.AOD.pool.root']
for f in PoolAODInput:
    print '    + %s' % f


### build the transient event and metadata trees
print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput)


### instantiate the TrigDecisionToolARA
print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)
### changing the output level of the TrigDecisionTool (default is ERROR)
import AthenaCommon.Constants as Constants
#tdt.setOutputLevel(Constants.WARNING)
#tdt.setOutputLevel(Constants.INFO)


electrons = tdt.getChainGroup('L2_e[0-9]+_(medium|loose|passHLT|loose1|medium1)')



### loop over the events
nevt = transientTree.GetEntries()
print '... loop over %i events' % nevt


for evt in xrange(8,9):
#for evt in xrange(nevt):

    print "... reading event",evt
    transientTree.GetEntry(evt)

    print '    ... getting features from cnt: ', evt, ' event: ', transientTree.McEventInfo.event_ID().event_number()


    #    alll2features = tdt.features('L2_e[0-9]+', ROOT.TrigDefs.alsoDeactivateTEs)
    #    print '    ... got electron chains features' 

    entries=[]
    for chain in list(electrons.getListOfTriggers()):
        # print ' .... start scanning objects contributed to chain: ', chain
        fc = tdt.features(chain, ROOT.TrigDefs.alsoDeactivateTEs)
        combinations = fc.getCombinations()
        # print '.... combinations: ', combinations.size()
        for comb in combinations:
            irois = comb.get('TrigRoiDescriptor')('initialRoI')
            if irois.size() != 1:
                print ' .... WARNING no dealing with chain of multiplicity !=1 ', iroi.size()
                break
            iroi = irois[0]
            # print ' ...... got initial RoI'
            
            # find in entries one corresponding to this RoI and was inserted by another chain
            qentry = [ e for e in entries if e.initialroi.roiword == iroi.cptr().roiWord()]
            if qentry == []:
                #  print '......... adding new entry to ntuple',iroi.cptr().roiWord()
                newentry = EgammaEntry()
                #  print '.. filling Initial RoI info',  iroi.cptr().roiWord()
                newentry.initialroi = InitialRoI()
                newentry.initialroi.roiword = iroi.cptr().roiWord()
                newentry.initialroi.eta = iroi.cptr().eta0()
                newentry.initialroi.phi = iroi.cptr().phi0()
                
                l1roi = tdt.ancestor('EmTau_ROI')(iroi.te(), '')
                newentry.l1roi = EmTau_RoI()
                newentry.l1roi.thresholds = list(l1roi.cptr().getThresholdNames())
                
                entries += [newentry]
                # print '..', entries
                qentry = [entries[-1]]
            
            entry = qentry[0]
            if comb.active():
                entry.triggers += [chain]
            clusters = comb.get('TrigEMCluster')('') 
            if not clusters.empty():
                cluster = clusters[0]
                if entry.cluster == None:                
                    entry.cluster = Cluster()
                    if cluster.te().getActiveState():
                        entry.cluster.triggers += [chain]
                    entry.cluster.e237 = cluster.cptr().e237()
                else:
                    if cluster.te().getActiveState():
                        entry.cluster.triggers += [chain]
            tracks = comb.get('TrigInDetTrackCollection')('')
            if not tracks.empty():
                trk = tracks[0]  ## taht means one track collection in RoI
                if entry.tracks == None:
                    entry.tracks = Tracks()                    
                    entry.tracks.triggers = [chain]
                    entry.tracks.count = trk.cptr().size()
                    entry.tracks.label = trk.label()
                else:
                    entry.tracks.triggers += [chain]
            # print '..got tracks'
            els = comb.get('TrigElectronContainer')()
            if not els.empty():
                electroncont = els[0]
                electron = electroncont.cptr()[0]
                if entry.electron == None:
                    entry.electron = Electron()
                    if electroncont.te().getActiveState():
                        entry.electron.triggers += [chain]
                    entry.electron.e = electron.e()
                    entry.electron.charge = electron.charge()
                else:
                    if electroncont.te().getActiveState():
                        entry.electron.triggers += [chain]
                    
    for e in entries:
        print e
