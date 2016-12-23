from __future__ import print_function
import ROOT
import cppyy
import AthenaROOTAccess.transientTree

cppyy.loadDictionary("libDataModelTestDataCommonDict")
cppyy.loadDictionary("libDataModelTestDataReadDict")
ROOT.DMTest.B
ROOT.DMTest.setPluginLibrary ('_PERS_DMTest::HAuxContainer_v1',
                              'libDataModelTestDataReadCnvPoolCnv.so')
ROOT.DMTest.setPluginLibrary ('_PERS_DMTest::HVec_v1',
                              'libDataModelTestDataReadCnvPoolCnv.so')
ROOT.DMTest.setPluginLibrary ('_PERS_DMTest__HAuxContainer_v1',
                              'libDataModelTestDataReadCnvPoolCnv.so')
ROOT.DMTest.setPluginLibrary ('_PERS_DataVector<DMTest::H_v1>',
                              'libDataModelTestDataReadCnvPoolCnv.so')

f = ROOT.TFile.Open ('xaoddata.root')
assert f.IsOpen()


from AthenaCommon.Include import Include
include = Include(show = False)
include('DataModelRunTests/xAODRootTest.py')


tt = AthenaROOTAccess.transientTree.makeTree(f)
reader = xAODTestRead()
for i in range(tt.GetEntries()):
    tt.GetEntry(i)
    print ('\nEntry', i)
    reader.execute(tt)
    print ('gvec')
    vec = getattr (tt, 'gvec')
    for g in vec:
        dump_auxdata (g)
        print('')
    print()
    
    
