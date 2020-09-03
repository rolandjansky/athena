# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: D3PDMakerTest/python/dumptuple1.py
# Author: snyder@bnl.gov
# Date: Jan, 2010
# Purpose: Test D3PD making.
#

from __future__ import print_function


# Always run in batch mode.
import os
if 'DISPLAY' in os.environ:
    del os.environ['DISPLAY']


import ROOT
import cppyy
from D3PDMakerTest.Treereader import treereader

# Touch these types early, before dictionaries are loaded,
# to prevent spurious error messages from ROOT.
# See ATLASRECTS-3486.
getattr(cppyy.gbl, 'vector<char>')
getattr(cppyy.gbl, 'vector<unsigned char>')
getattr(cppyy.gbl, 'vector<float>')
getattr(cppyy.gbl, 'vector<short>')
getattr(cppyy.gbl, 'map<string,string>')


from RootUtils import PyROOTFixes
PyROOTFixes.enable_tree_fixes()

from PyUtils.fprint import fprintln

def safeord(x):
    if type(x) == type(''):
        return ord(x)
    if x < 0:
        x = x + 256
    return x

def dump_obj1 (tr, coll = True, dodef = False):
    for i in range (tr.n):
        assert tr.i[i] == tr.mult_i[i]
        print (' %1d %3d %3d %3d %3d %3d %3d %5.1f %5.1f %1d %3d %5.1f %3d %5.1f %3d %5.1f %3d %5.1f' %
               (i,
                safeord(tr.c[i]),
                safeord(tr.uc[i]),
                tr.s[i],
                tr.us[i],
                tr.i[i],
                tr.ui[i],
                tr.f[i],
                tr.d[i],
                tr.b[i],
                tr.o2a_i[i],
                tr.o2a_f[i],
                tr.o2c_i[i],
                tr.o2c_f[i],
                tr.o2c_o2d_i[i],
                tr.o2c_o2d_f[i],
                tr.o2c_o2e_i[i],
                tr.o2c_o2e_i[i],
                ))
        if dodef:
            print (' %3d %3d' % (ord(tr.c1[i]),  ord(tr.c2[i])), end='')
            print (' %3d %3d' % (ord(tr.uc1[i]), ord(tr.uc2[i])), end='')
            print (' %3d %3d' % (tr.s1[i],  tr.s2[i]), end='')
            print (' %3d %3d' % (tr.us1[i], tr.us2[i]), end='')
            print (' %3d %3d' % (tr.i1[i],  tr.i2[i]), end='')
            print (' %3d %3d' % (tr.ui1[i], tr.ui2[i]), end='')
            print (' %5.1f %5.1f' % (tr.f1[i], tr.f2[i]), end='')
            print (' %5.1f %5.1f' % (tr.d1[i], tr.d2[i]), end='')
            print (' %3d %3d' % (tr.b1[i],  tr.b2[i]))
        if coll:
            for j in range (tr.o2b_n[i]):
                assert tr.o2b_mult_i[i][j] == tr.o2b_i[i][j]
                print ('   o2b%1d %3d %5.1f' % (j,
                                                tr.o2b_i[i][j],
                                                tr.o2b_f[i][j]))
    return


def dump_obj1_2 (tr):
    for i in range (tr.n):
        print (' %1d %3d %5.1f %3d %5.1f %3d %5.1f' %
               (i,
                tr.o2a_i[i],
                tr.o2a_f[i],
                tr.o2b_i[i],
                tr.o2b_f[i],
                tr.o2c_i[i],
                tr.o2c_f[i],
                ))
    return


def dump_obj3 (tr):
    for i in range (tr.n):
        fprintln (sys.stdout, ' ', i, tr.pt[i], tr.eta[i], tr.phi[i], tr.m[i], tuple(tr.index[i]), tr.a1_index[i])
        fprintln (sys.stdout, '   ', tr.hlv_pt[i], tr.hlv_eta[i], tr.hlv_phi[i], tr.hlv_m[i])
    return


def dump_obj4 (tr):
    for i in range (tr.n):
        print (' ', i, tr.obj4[i])


def dump_obj5 (tr):
    for i in range (tr.n):
        print (' ', i, tr.obj5[i], tr.anInt[i], tr.aFloat[i], tr.s[i],
               '%7.1f %6.3f %6.3f' % (tr.aFourvec[i].Pt(),
                                      tr.aFourvec[i].Eta(),
                                      tr.aFourvec[i].Phi()),
               tr.dummy[i])


def dump_def (tr):
    print (' %3d %3d' % (ord(tr.c1),  ord(tr.c2)))
    print (' %3d %3d' % (ord(tr.uc1), ord(tr.uc2)))
    print (' %3d %3d' % (tr.s1,  tr.s2))
    print (' %3d %3d' % (tr.us1, tr.us2))
    print (' %3d %3d' % (tr.i1,  tr.i2))
    print (' %3d %3d' % (tr.ui1, tr.ui2))
    print (' %5.1f %5.1f' % (tr.f1, tr.f2))
    print (' %5.1f %5.1f' % (tr.d1, tr.d2))
    print (' %3d %3d' % (tr.b1,  tr.b2))
    return


def dump_mdt (tr):
    for i in range (tr.n):
        print (' ', i, tr.globalTime[i], tr.driftRadius[i],
               tr.stepLength[i], tr.energyDeposit[i],
               tr.particleEncoding[i], tr.kineticEnergy[i],
               tr.MDTid[i], tr.trackNumber[i])
    return


def dump_trackrecord (tr):
    for i in range (tr.n):
        fprintln (sys.stdout, ' ',
                  i, tr.barcode[i], tr.pdgId[i], tr.E[i], tr.pt[i],
                  tr.eta[i], tr.phi[i], tr.pos_x[i], tr.pos_y[i], tr.pos_z[i],
                  tr.time[i])
    return


def dumpit (file):
    f=ROOT.TFile(file)
    tt=f.Get('test1')
    tr = treereader(tt)
    tr_o1 = tr.subreader ('o1_%s')
    tr_o1filt = tr.subreader ('o1filt_%s')
    tr_o12 = tr.subreader ('o12_%s')
    tr_o3 = tr.subreader ('o3_%s')
    tr_o4 = tr.subreader ('o4_%s')
    tr_o5 = tr.subreader ('o5_%s')
    tr_def = tr.subreader ('def_%s')
    tr_mdt = tr.subreader ('mdt_hit_%s')
    tr_trackrecord = tr.subreader ('ms_entry_truth_%s')
    n = tt.GetEntries()
    for i in range(n):
        tt.GetEntry(i)
        print ('\nEvent', i)
        print ('-----------')
        print ('** Obj1')
        dump_obj1 (tr_o1, dodef = True)
        print ('** Obj1_2')
        dump_obj1_2 (tr_o12)
        print ('** Obj1filt')
        dump_obj1 (tr_o1filt, coll = False)
        print ('** Obj3')
        dump_obj3 (tr_o3)
        print ('** Obj4')
        dump_obj4 (tr_o4)
        print ('** Obj5')
        dump_obj5 (tr_o5)
        print ('** Def')
        dump_def (tr_def)
        print ('** MDT')
        dump_mdt (tr_mdt)
        print ('** TrackRecord')
        dump_trackrecord (tr_trackrecord)
    return


if __name__ == '__main__':
    import sys
    file = 'test1.root'
    if len(sys.argv) > 1:
        file = sys.argv[1]
    dumpit (file)
    
