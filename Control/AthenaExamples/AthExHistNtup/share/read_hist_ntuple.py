import ROOT
print ":" * 80
print "::: ntuple..."
f1 = ROOT.TFile.Open("ntuple.root","READ")
t = f1.Get("trees/tree1")
print ":: entries:",t.GetEntries()
print ":: branches:",[b.GetName() for b in t.GetListOfBranches()]
for i in xrange(t.GetEntries()):
    if t.GetEntry(i) <= 0:
        print "** error reading entry [%i]" % (i,)
        break
    print ":: entry [%i]..." % (i,)
    print "::  run:",t.run
    print "::  size:",t.size
    print "::  rundata:",list(t.rundata)
print ":: done."

print ":" * 80
print "::: histogram..."
f2 = ROOT.TFile.Open("hist.root", "READ")
h1 = f2.Get("simple1D/h1")
print ":: entries:",h1.GetEntries()
h1.Draw()
print ":: done."

print "::: bye."

