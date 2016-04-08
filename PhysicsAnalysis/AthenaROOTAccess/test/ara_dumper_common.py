# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: ara_dumper_common.py,v 1.15 2009-05-02 05:01:29 ssnyder Exp $
#
# @file AthenaROOTAccess/test/ara_dumper_common.py
# @author sss
# @date Aug 2007
# @brief Test for AthenaROOTAccess.
#
# This is code common to all the ARA dump tests.
#


import ROOT
import cppyy
import AthenaROOTAccess.transientTree
import sys
from AthenaROOTAccess.dumpers import Evdump, try_autokey

if not globals().has_key ('onlykeys'):
    onlykeys = []
if not globals().has_key ('onlytypes'):
    onlytypes = []

class Files:
    def __init__ (self, f, fout_base):
        self.f = f
        self.tt = AthenaROOTAccess.transientTree.makeTree (f)
        if isinstance (fout_base, Files):
            self.fout = fout_base.fout
            self.fout_rand = fout_base.fout_rand
        else:
            import bz2
            #self.fout = open(fout_base + '.out', 'w')
            #self.fout_rand = open(fout_base + '-rand.out', 'w')
            self.fout = bz2.BZ2File(fout_base + '.out.bz2', 'w')
            self.fout_rand = bz2.BZ2File(fout_base + '-rand.out.bz2', 'w')
        return

    def file (self):
        if isinstance (self.f, ROOT.TChain):
            return self.f.GetFile()
        elif isinstance (self.f, ROOT.TTree):
            return self.f.GetDirectory()
        return self.f


def _getTokenField (token, field):
    ipos = token.find ('[' + field + '=')
    if ipos < 0: return None
    beg = ipos + 2 + len(field)
    end = token.find (']', beg)
    if end < 0: return None
    return token[beg:end]


def get_dhes (files, entry):
    dh = AthenaROOTAccess.transientTree.get_dataheader (files.file(), entry)
    # Loop over elements.
    for elem in dh.elements():
        s = elem.token()
        cnt = _getTokenField (s, 'CNT')
        if cnt == "":
            oid = _getTokenField (s, 'OID')
            offset = oid[0:oid.find ('-')]
            cnt = files.tt.getLink( int(offset, 16) )
            tokenStr = s.replace("[CNT=]", "[CNT=" + cnt + "]");
            elem.setToken(tokenStr)
    return [(elem.key(), elem.token()) for elem in dh.elements()]


def run (files, first=0, n=-1):
    evdump = Evdump(files.fout, files.fout_rand,
                    onlykeys = onlykeys, onlytypes = onlytypes)
    last = files.tt.GetEntries()
    if n >= 0:
        last = min (last, first + n)
    for i in range(first, last):
        if isinstance (files.f, ROOT.TChain):
            local_entry = files.f.LoadTree (i)
        else:
            local_entry = i
        def getter (d, dhe, keyprint):
            br = files.tt.GetBranch(keyprint)
            if not br:
                return try_autokey (files.tt, d, dhe, keyprint)
            files.tt.GetBranch(keyprint).GetEntry(i)
            return getattr (files.tt, keyprint)
        files.tt.GetEntry (i)
        evdump.dump_event_tree (get_dhes(files, local_entry), files.tt)
        #files.tt.setEntry(i)
        #evdump.dump_event_common (get_dhes(files, local_entry), getter)
    evdump.print_missed (sys.stdout)
    return


import sys
import os
def make_fin (fin):
    if len(sys.argv) > 1:
        if os.path.isfile (sys.argv[1]):
            return sys.argv[1]
        if fin.startswith ('AOD-'):
            fin = 'AOD-' + sys.argv[1] + fin[3:]
        fin = 'AOD-' + sys.argv[1] + '/' + fin
    return fin
