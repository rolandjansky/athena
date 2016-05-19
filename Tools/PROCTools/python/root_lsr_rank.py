#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## root_lsr_rank.py - Brief description of the purpose of this script (Has to be in PROC tools)
# When run as
# root_lsr_rank.py <HIST file> --hash
#
# it produces a listing of all objects in the HIST file and a hash value for each of them.  This output can then be compared between the clean and patched runs to look 
# for any changes in HIST output.  Since it's entirely possible to change the HIST output unintentionally without changing the AOD/ESD etc., this catches a new class of potential errors.
# $Id$

import ROOT
import sys, os, operator
import argparse
import zlib

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
args=parser.parse_args()

ordering = args.rankorder

accounting = {}; hashes = {}; types = {}

ROOT.gInterpreter.LoadText("UInt_t bufferhash(TKey* key) { key->SetBuffer(); key->ReadFile(); UInt_t rv = TString::Hash(key->GetBuffer()+key->GetKeylen(), key->GetNbytes()-key->GetKeylen()); key->DeleteBuffer(); return rv; }")
ROOT.gInterpreter.LoadText("void* getbuffer(TKey* key) { key->SetBuffer(); key->ReadFile(); return (void*) (key->GetBuffer()+key->GetKeylen()); }")
ROOT.gInterpreter.LoadText("UInt_t bufferhash2(TKey* key) { TObject* obj = key->ReadObj(); TMessage msg(kMESS_OBJECT); msg.WriteObject(obj); UInt_t rv = TString::Hash(msg.Buffer(), msg.Length()); delete obj; return rv; }")


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
            if args.hash:
                #lhash = ROOT.bufferhash(k)
                #objsize = (k.GetNbytes()-k.GetKeylen())/8
                #print (k.GetNbytes()-k.GetKeylen())/8.
                #buf = ROOT.getbuffer(k); buf.SetSize(objsize)
                #print buf[objsize-1], objsize
                #lhash = zlib.adler32(str(buf))
                #k.DeleteBuffer()
                #obj=k.ReadObj(); 
                #tm=ROOT.TMessage(ROOT.TMessage.kMESS_OBJECT)
                #tm.WriteObject(obj)
                lhash = ROOT.bufferhash2(k)
                #obj.IsA().Destructor(obj)
                #lhash = 0
            else:
                lhash = 0
            idxname = os.path.join(thispath, k.GetName())
            accounting[idxname] = (k.GetObjlen(), k.GetNbytes()-k.GetKeylen())
            hashes[idxname] = lhash
            types[idxname] = k.GetClassName()
            #print '%s,' % os.path.join(thispath, k.GetName()),
            #obj = k.ReadObj(); obj.IsA().Destructor(obj)
            #print 'OK'
    for k in subdirs:
        dumpdir(k.ReadObj())

f = ROOT.TFile.Open(args.filename)
if args.path:
    d = f.Get(args.path.rstrip('/'))
    if not d:
        print "Can't access path", args.path, "- exiting"
        sys.exit(1)
else:
    d = f
dumpdir(d)

#sortedl = sorted(accounting.items(), key=operator.itemgetter(0,1), reverse=True)
if ordering == 'onfile':
    key=lambda x: (x[1][1], x[1][0], x[0])
elif ordering == 'uncompressed':
    key=lambda x: (x[1][0], x[1][1], x[0])
else:
    key=lambda x: (x[0], x[1][1], x[1][0])
sortedl = sorted(accounting.items(), key=key, reverse=True)
if args.hash:
    print '\n'.join('%s %s: %d uncompressed, %d on file (hash %s)' % (types[a], a, b, c, hashes[a]) for a, (b, c) in  sortedl)
else:
    print '\n'.join('%s %s: %d uncompressed, %d on file' % (types[a], a, b, c) for a, (b, c) in  sortedl)

