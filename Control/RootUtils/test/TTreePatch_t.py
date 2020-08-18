# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: TTreePatch_t.py
# Created: Jul, 2015
# Purpose: Regression tests for PyROOTTTreePatch
#


class Notifier:
    def Notify (self, *args):
        print ('Notify', args)
        return


def make_tree (name, offset):
    import ROOT
    import array
    import struct
    f = ROOT.TFile.Open (name, 'RECREATE')
    t = ROOT.TTree ('tt', 'tt')
    arr = array.array ('b')
    arr.frombytes ((256*' ').encode())
    t.Branch ('b1', arr, 'i1/I:f1/F:d1[3]/D')
    obj = ROOT.RootUtilsTest.TreeTest()
    t.Branch ('b2', 'RootUtilsTest::TreeTest', obj)
    for i in range(10):
        struct.pack_into ('ifddd', arr, 0, offset+i+1, offset+i+1.5,
                          offset+i+11.5,
                          offset+i+21.5,
                          offset+i+31.5)
        obj.i = offset+10+i+1
        obj.f = offset+10+i+1.5
        t.Fill()
    t.Write()
    f.Close()
    return


def dump_tree1 (t):
    for i in range(t.GetEntries()):
        t.GetEntry(i)
        print ('%3d %5.1f %3d %5.1f [%5.1f %5.1f %5.1f]' % (t.i1, t.f1, t.b2.i, t.b2.f,
                                                            t.d1[0], t.d1[1], t.d1[2]))
    return


def dump_tree (name):
    import ROOT
    f = ROOT.TFile.Open (name)
    #f.tt.SetBranchStatus ('*', 0)
    dump_tree1 (f.tt)
    f.Close()
    return


def dump_chain (names):
    import ROOT
    t = ROOT.TChain('tt')
    for n in names:
        t.Add (n)
    #t.SetBranchStatus ('*', 0)
    n = Notifier()
    t.SetNotify(n)
    dump_tree1 (t)
    return


def _test1():
    """test1
    >>> import ROOT
    >>> import cppyy
    >>> make_tree ('tree1.root', 100)
    >>> make_tree ('tree2.root', 200)
    >>> from RootUtils.PyROOTFixes import enable_tree_fixes
    >>> enable_tree_fixes()
    >>> dump_tree ('tree1.root')
    101 101.5 111 111.5 [111.5 121.5 131.5]
    102 102.5 112 112.5 [112.5 122.5 132.5]
    103 103.5 113 113.5 [113.5 123.5 133.5]
    104 104.5 114 114.5 [114.5 124.5 134.5]
    105 105.5 115 115.5 [115.5 125.5 135.5]
    106 106.5 116 116.5 [116.5 126.5 136.5]
    107 107.5 117 117.5 [117.5 127.5 137.5]
    108 108.5 118 118.5 [118.5 128.5 138.5]
    109 109.5 119 119.5 [119.5 129.5 139.5]
    110 110.5 120 120.5 [120.5 130.5 140.5]
    >>> dump_tree ('tree2.root')
    201 201.5 211 211.5 [211.5 221.5 231.5]
    202 202.5 212 212.5 [212.5 222.5 232.5]
    203 203.5 213 213.5 [213.5 223.5 233.5]
    204 204.5 214 214.5 [214.5 224.5 234.5]
    205 205.5 215 215.5 [215.5 225.5 235.5]
    206 206.5 216 216.5 [216.5 226.5 236.5]
    207 207.5 217 217.5 [217.5 227.5 237.5]
    208 208.5 218 218.5 [218.5 228.5 238.5]
    209 209.5 219 219.5 [219.5 229.5 239.5]
    210 210.5 220 220.5 [220.5 230.5 240.5]
    >>> dump_chain (['tree1.root', 'tree2.root'])
    Notify ()
    Notify ()
    101 101.5 111 111.5 [111.5 121.5 131.5]
    102 102.5 112 112.5 [112.5 122.5 132.5]
    103 103.5 113 113.5 [113.5 123.5 133.5]
    104 104.5 114 114.5 [114.5 124.5 134.5]
    105 105.5 115 115.5 [115.5 125.5 135.5]
    106 106.5 116 116.5 [116.5 126.5 136.5]
    107 107.5 117 117.5 [117.5 127.5 137.5]
    108 108.5 118 118.5 [118.5 128.5 138.5]
    109 109.5 119 119.5 [119.5 129.5 139.5]
    110 110.5 120 120.5 [120.5 130.5 140.5]
    Notify ()
    201 201.5 211 211.5 [211.5 221.5 231.5]
    202 202.5 212 212.5 [212.5 222.5 232.5]
    203 203.5 213 213.5 [213.5 223.5 233.5]
    204 204.5 214 214.5 [214.5 224.5 234.5]
    205 205.5 215 215.5 [215.5 225.5 235.5]
    206 206.5 216 216.5 [216.5 226.5 236.5]
    207 207.5 217 217.5 [217.5 227.5 237.5]
    208 208.5 218 218.5 [218.5 228.5 238.5]
    209 209.5 219 219.5 [219.5 229.5 239.5]
    210 210.5 220 220.5 [220.5 230.5 240.5]
"""


import doctest
print ('RootUtils/TTreePatch_t')
doctest.testmod()
