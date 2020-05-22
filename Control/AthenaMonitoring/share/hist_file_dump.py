#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from __future__ import print_function

import ROOT
import sys, os, operator
import argparse
import zlib
import json
from PyUtils.fprint import _formatFloat

def fixprecision(x):
    import math
    if not isinstance(x, float):
        return x
    else:
        mantissa, exponent = math.frexp(x)
        sm = '%.12g' % mantissa
        return _formatFloat (float(sm[:16]) * 2**exponent)

def jsonfixup(instr):
    instr = instr.Data()
    j=json.loads(instr)
    # the following are very subject to floating point numeric effects
    for badkey in ('fTsumw', 'fTsumwx', 'fTsumw2', 'fTsumwx2', 'fTsumwy', 'fTsumwy2', 'fTsumwxy',
                   'fTsumwz', 'fTsumwz2', 'fTsumwxz', 'fTsumwyz' ):
        if badkey in j:
            j[badkey] = fixprecision(j[badkey])
            #print(type(j["fTsumwx"]))
    for badkey in ('fSumw2',):
        if badkey in j:
            j[badkey] = [fixprecision(_) for _ in j[badkey]]
    # the following ignores small layout fluctuations in TTrees
    if 'fBranches' in j:
        for branch in j['fBranches']['arr']:
            branch['fBasketSeek'] = []
    return json.dumps(j, sort_keys=True)

parser=argparse.ArgumentParser()
parser.add_argument('filename',
                    help='Input HIST file name')
parser.add_argument('-r', '--rankorder', default='onfile',
                    choices=['onfile', 'uncompressed', 'name'],
                    help='rankorder is "onfile" (default), "uncompressed" or "name"')
parser.add_argument('-p', '--path',
                    help='Only look under this directory')
parser.add_argument('--hash', action='store_true',
                    help='Print hashes of objects')
parser.add_argument('--metadata', action='store_true',
                    help='Include metadata trees')
parser.add_argument('--no_onfile', action='store_true',
                    help="Don't show on file size")
parser.add_argument('--no_inmem', action='store_true',
                    help="Don't show in memory size")
parser.add_argument('--tree_entries', action='store_true',
                    help="Use more robust hash of TTree branches + entries")
args=parser.parse_args()

ordering = args.rankorder

accounting = {}; hashes = {}; types = {}

ROOT.gInterpreter.LoadText("UInt_t bufferhash(TKey* key) { key->SetBuffer(); key->ReadFile(); UInt_t rv = TString::Hash(key->GetBuffer()+key->GetKeylen(), key->GetNbytes()-key->GetKeylen()); key->DeleteBuffer(); return rv; }")
ROOT.gInterpreter.LoadText("void* getbuffer(TKey* key) { key->SetBuffer(); key->ReadFile(); return (void*) (key->GetBuffer()+key->GetKeylen()); }")
ROOT.gInterpreter.LoadText("UInt_t bufferhash2(TKey* key) { TObject* obj = key->ReadObj(); TMessage msg(kMESS_OBJECT); msg.WriteObject(obj); UInt_t rv = TString::Hash(msg.Buffer(), msg.Length()); delete obj; return rv; }")
ROOT.gInterpreter.LoadText("UInt_t bufferhash3(TKey* key) { TObject* obj = key->ReadObj(); UInt_t rv = obj->Hash(); delete obj; return rv; }")
ROOT.gInterpreter.LoadText("TString getjson(TKey* key) { TObject* obj = key->ReadObj(); auto rv = TBufferJSON::ConvertToJSON(obj); delete obj; return rv; }")

ROOT.gSystem.Load('libDataQualityUtils')

def fuzzytreehash(tkey):
    t = tkey.ReadObj()
    rv = zlib.adler32((' '.join(_.GetName() for _ in t.GetListOfBranches()))
                        + (' '.join(_.GetName() + _.GetTypeName() for _ in t.GetListOfLeaves()))
                        + ' ' + str(t.GetEntries()))
    del t
    return rv

def dumpdir(d):
    thispath = d.GetPath()
    if ':' in thispath:
        thispath = thispath.split(':', 1)[1]
    #print thispath
    subdirs = []
    for k in d.GetListOfKeys():
        if not args.metadata and k.GetName() == 'metadata' and k.GetClassName() == 'TTree':
            continue
        if k.GetClassName().startswith('TDirectory'):
            subdirs.append(k)
        else:
            if args.tree_entries and k.GetClassName() == 'TTree':
                lhash = fuzzytreehash(k)
            elif args.hash:
                lhash = zlib.adler32(jsonfixup(ROOT.getjson(k)).encode())
                if lhash < 0:
                    lhash += 2**32
            else:
                lhash = 0
            idxname = os.path.join(thispath, k.GetName())
            accounting[idxname] = (k.GetObjlen(), k.GetNbytes()-k.GetKeylen())
            hashes[idxname] = lhash
            types[idxname] = k.GetClassName()
    for k in subdirs:
        dumpdir(k.ReadObj())

f = ROOT.TFile.Open(args.filename)
if args.path:
    d = f.Get(args.path.rstrip('/'))
    if not d:
        print("Can't access path", args.path, "- exiting")
        sys.exit(1)
else:
    d = f
dumpdir(d)

if ordering == 'onfile':
    key=lambda x: (x[1][1], x[1][0], x[0])
elif ordering == 'uncompressed':
    key=lambda x: (x[1][0], x[1][1], x[0])
else:
    key=lambda x: (x[0], x[1][1], x[1][0])
sortedl = sorted(accounting.items(), key=key, reverse=True)
if args.hash:
    print('\n'.join(('%s %s: '
                     + ('%d uncompressed' % b if not args.no_inmem else '')
                     + (', %d on file ' % c if not args.no_onfile else ' ')
                     + '(hash %s)')
                    % (types[a], a, hashes[a]) for a, (b, c) in  sortedl)
          )
else:
    print('\n'.join(('%s %s: '
                     + ('%d uncompressed' % b if not args.no_inmem else '')
                     + (', %d on file' % c if not args.no_onfile else ' '))
                    % (types[a], a) for a, (b, c) in  sortedl)
          )

