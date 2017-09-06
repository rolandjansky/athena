import ROOT
if not ('PoolAODInput' in dir()):
    PoolAODInput=['/afs/cern.ch/user/t/tbold/public/TDTtest/menu1/AOD.pool.root']
    PoolAODInput=['AOD.pool.root']

print 'input file = ', PoolAODInput

print '... build collection and metadata trees'
from TrigDecisionTool.BuildTransientTrees import BuildTransientTrees
(transientTree, transientMetaDataTree) = BuildTransientTrees(PoolAODInput)

### instantiate the TrigDecisionToolARA
print '... instantiate TrigDecisionToolARA'
tdt = ROOT.Trig.TrigDecisionToolARA(transientTree, transientMetaDataTree)

cg = tdt.getChainGroup('EF_mu10')

nevt = transientTree.GetEntries()

print '... loop over %i events' % nevt

for evt in xrange(nevt):
    transientTree.GetEntry(evt)

    featcont = cg.features()

    combs = featcont.getCombinations()

    for comb in combs:
        

        muefinfo = comb.get('TrigMuonEFInfoContainer')()

        for muefinfofeat in muefinfo:
            thecont = muefinfofeat.cptr()
            for efinfo in thecont:
                print 'EF roi, eta = ', efinfo.EtaPreviousLevel()
                
                trkcont = efinfo.TrackContainer()
                for infotrk in trkcont:
                    print 'Working on next TrigMuonEF track'

                    if infotrk.hasCombinedTrack():
                        print 'found combined track'
                        combtrk = infotrk.CombinedTrack()
                        print 'comb pt = ', combtrk.pt()
                        print 'extrap pt = ', infotrk.ExtrapolatedTrack().pt()
                        if combtrk.getIDTrackParticle()!=0:
                            print "attached id trk pt = ", combtrk.getIDTrackParticle().pt()
