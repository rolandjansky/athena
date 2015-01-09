from __future__ import print_function
import ROOT
import cppyy
import AthenaROOTAccess.transientTree

cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")

f = ROOT.TFile.Open ('xaoddata.root')
assert f.IsOpen()


from AthenaCommon.Include import Include
include = Include(show = False)
include('DataModelRunTests/xAODRootTest.py')


tt = AthenaROOTAccess.transientTree.makeTree(f)
reader = xAODTestRead()
for i in range(tt.GetEntries()):
    tt.GetEntry(i)
    print ('Entry', i)
    reader.execute(tt)
    print ('gvec')
    vec = getattr (tt, 'gvec')
    for g in vec:
        dump_auxdata (g)
        print('')
    print()
    
    
