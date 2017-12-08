from glob import glob
cbntfiles = glob('/tmp/binet/data/cbnt_aod.aan.*.root')
cbntfiles = [
    'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.0.root',
    'root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd/ntuple.1.root',
    ]

import ROOT
ROOT.gROOT.SetBatch(True)

#tuple = ROOT.TChain('CollectionTree')
tuple = ROOT.TChain('egamma')
for cbnt in cbntfiles:
    tuple.Add(cbnt)

print ">>> nevents:",tuple.GetEntries()

tuple.SetBranchStatus('*', 0)

branches = [ '*' ]
for br  in branches:
    tuple.SetBranchStatus(br, 1)
    
for ievt in xrange(tuple.GetEntriesFast()):
    tuple.GetEntry(ievt)

    print '[%10i] branch [%s]: %r' % ( ievt, 'nMC', getattr(tuple, 'nMC') )
    

    
