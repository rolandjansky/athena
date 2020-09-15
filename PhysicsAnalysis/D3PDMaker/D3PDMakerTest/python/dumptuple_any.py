# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#
# File: D3PDMakerTest/python/dumptuple_any.py
# Author: snyder@bnl.gov
# Date: Feb, 2010
# Purpose: Test D3PD making.
#

from __future__ import print_function


# Always run in batch mode.
import os
if os.environ.has_key('DISPLAY'):
    del os.environ['DISPLAY']
os.environ['TERM'] = 'dumb'


import ROOT
import cppyy
from PyUtils.Helpers import ROOT6Setup
ROOT6Setup()
# Autoloading doesn't work correctly for vector<unsigned long long>,
# since root will munge the name to vector<ULong64_t> before trying
# to look up the autoload.  (cf  R__FindSTLClass in TROOT.cxx).
# So need to make sure that the reflex dictionary for these classes
# is loaded.
cppyy.load_library('libAtlasSTLAddReflexDict')
ROOT.TClass('vector<float>')
ROOT.TClass('vector<vector<unsigned int> >')
from ordereddict import OrderedDict
import operator


# Work around a library load order issue...
ROOT.D3PDTest.MapDumper

def topy (o):
    if type(o).__name__ in ['map<string,string>',
                            'map<string,float>',
                            'map<string,int>']:
        keys = ROOT.D3PDTest.MapDumper.keys (o)
        values = ROOT.D3PDTest.MapDumper.values (o)
        return OrderedDict (sorted (zip (keys, values), key=lambda t: t[0]))
    if (type(o).__name__.startswith ('vector<char') or
        type(o).__name__.startswith ('vector<unsigned char')):
        ll = list(o)
        lens = [len(x) for x in ll]
        if lens and min(lens) == 1 and max(lens) == 1:
            ll = [ord(x) for x in ll]
        return ll
    if type(o).__name__.startswith ('vector<'):
        return [topy(x) for x in o]
    if type(o).__name__ in ['PyFloatBuffer',
                            'PyIntBuffer']:
        return list(o)
    return o


def alleq (l):
    if len(l) == 0: return True
    for i in range(1, len(l)):
        if l[i] != l[0]: return False
    return True


def isint(o):
    try:
        dum = int(o)
    except ValueError:
        return False
    return True


def form_list(l):
    if type(l) != type([]):
        return l
    return '[' + ','.join(l) + ']'
def squash_list(l):
    if not l: return l
    frags=[]
    nextfrag = []
    i = 0
    while i < len (l):
        if i < len(l)-1 and l[i] == l[i+1]:
            if nextfrag: frags.append(nextfrag)
            nextfrag = []
            j = i+2
            while j < len(l) and l[i] == l[j]:
                j += 1
            frags.append ('%d*[%s]' % (j-i, l[i]))
            i = j
        #elif (i < len(l)-2 and isint(l[i]) and
        #      isint(l[i+1]) and isint(l[i+2]) and
        #      int(l[i+1]) == int(l[i])+1 and int(l[i+2]) == int(l[i])+2):
        #    nextfrag = []
        #    j = i+3
        #    while j < len(l) and isint(l[j]) and int(l[j]) == int(l[i]) + (j-i):
        #        j += 1
        #    frags.append ('range(%s,%d)' % (l[i], int(l[i]) + (j-i)))
        #    i = j
        else:
            nextfrag.append (l[i])
            i += 1
    if nextfrag: frags.append (nextfrag)
    frags = [form_list(s) for s in frags]
    return '+'.join (frags)
                          
            
    
def tostr (o):
    if type(o) == type([]):
        l = [tostr(x) for x in o]
        if len(l) > 1:
            return squash_list(l)
        return '[' + ', '.join (l) + ']'
    if type(o) == type(""):
        s = repr(o)
    else:
        s = str(o)
    if s.endswith ('.0'): s = s[:-2]
    return s


def common_length (s1, s2):
    i = 0
    while i < len(s1) and i < len(s2) and s1[i] == s2[i]:
        i += 1
    return i

def merge_names (blist):
    out = []
    lasttag = ''
    for (i,b) in enumerate(blist):
        if i < len(blist)-1:
            b2 = blist[i+1]
        else:
            b2 = ''
        comm1 = common_length (lasttag, b)
        comm2 = common_length (b, b2)
        if comm1 >= comm2:
            if comm1 != len(lasttag):
                while comm1 > 0 and b[comm1-1] != '_':
                    comm1 -= 1
                if b[:comm1] != lasttag:
                    lasttag = b[:comm1]
                    out.append (':' + lasttag) 
        else:
            while comm2 > 0 and b[comm2-1] != '_':
                comm2 -= 1
            if b[:comm2] != lasttag:
                lasttag = b[:comm2]
                out.append (':' + lasttag)

        nm = b[len(lasttag):]
        if len(nm) == 0: nm = '$'
        out.append (nm)
    return out
                


def dumptree (tt, by_branch):
    n = tt.GetEntries()
    if tt.GetName().startswith ('dum_') and n > 10000000:
        print ('[Dummy tree skipped]')
        return
    bb = [b.GetName() for b in tt.GetListOfBranches()]
    bb.sort()
    if by_branch:
        bb = merge_names (bb)
        print ('\nBranches')
        print ('-----------')
        lasttag = ''
        for b in bb:
            if b[0] == ':':
                print (b)
                lasttag = b[1:]
                continue
            if b == '$':
                bname = lasttag
            else:
                bname = lasttag + b
            br = tt.GetBranch(bname)
            data = []
            for i in range(n):
                br.GetEntry(i)
                data.append (topy (getattr(tt, bname)))
            print (b, tostr (data))
    else:
        for i in range(n):
            tt.GetEntry(i)
            print ('\nEvent', i)
            print ('-----------')
            for b in bb:
                print (b, tostr (topy (getattr (tt, b))))
    return


def dumphist (h):
    print ('bins: ', [h.GetBinContent(i) for i in range(h.GetSize())])
    if h.GetSumw2N():
        print ('errs: ', [h.GetBinError(i) for i in range(h.GetSize())])
    return




def dumpdir (f, by_branch, pref=''):
    kk = [k.GetName() for k in f.GetListOfKeys()]
    kk.sort()
    for k in kk:
        o = f.Get (k)
        if k == 'Schema':
            pass
        elif isinstance (o, ROOT.TTree):
            print ('\n\n\nTree', pref+k)
            dumptree (o, by_branch)
        elif isinstance (o, ROOT.TH1):
            print ('\n\n\nTH1', pref+k)
            dumphist (o)
        elif isinstance (o, ROOT.TDirectory):
            dumpdir (o, by_branch, pref + k + '/')
        elif isinstance (o, ROOT.TObjString):
            print ('\n\n\nString', pref+k)
            if k == '_pickle':
                print ('[pickle data skipped]')
            else:
                print (o, end='')
            print ('__END_OF_STRING__')
        else:
            print ('\n\n\nKey', pref+k)
            print (o)
    return
              


def dumpit (file, by_branch):
    f=ROOT.TFile(file)
    dumpdir (f, by_branch=by_branch)
    return


if __name__ == '__main__':
    import sys
    file = 'test1.root'
    by_branch = False
    if len(sys.argv) > 1 and sys.argv[1] == '--by-branch':
        by_branch = True
        del sys.argv[1]
    if len(sys.argv) > 1:
        file = sys.argv[1]
    dumpit (file, by_branch)
    
