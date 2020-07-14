#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#copied from DQPostProcessi.py and modified
# Sami Kama
from __future__ import print_function
import sys
def _dolsrwrapper(fname):
    import ROOT
    rf = ROOT.TFile.Open(fname, 'READ')
    if not rf or not rf.IsOpen():
        print('   %s is empty or not accessible' % fname)
        return 3
    if rf.TestBit(ROOT.TFile.kRecovered):
        print(" %s is recovered. It means it was corrupt" % fname)
        return 7
    cleancache = ROOT.gROOT.MustClean(); ROOT.gROOT.SetMustClean(False)
    RV=_dolsr(rf)
    print("dolsr returned %s "%(RV))
    rf.Close()
    ROOT.gROOT.SetMustClean(cleancache)
    return RV
def _dolsr(dir):
    import ROOT
    resultdir = dir.Get('Results'); statusobj = None
    if resultdir:
        statusobj = resultdir.Get('Status')
        if statusobj:
            l = statusobj.GetListOfKeys()
            l[0].GetName()
    keys = dir.GetListOfKeys()
    for key in keys:
        name = key.GetName()
        #print "Reading key named %s"%name
        keyClass=ROOT.TClass.GetClass(key.GetClassName())
        if keyClass.InheritsFrom('TDirectory'):
            dirobj = key.ReadObj()
            resultdir = dirobj.Get('Results'); statusobj = None
            if resultdir:
                statusobj = resultdir.Get('Status')
            # is a check? is a summary? or recurse?
            if name[-1] == '_' and resultdir:
                hist = dir.Get(name[:-1])
                subkeys = resultdir.GetListOfKeys()
                for subkey in subkeys:
                    ressub = subkey.GetName()
                    if ressub not in ('Status', 'Reference', 'ResultObject') and ROOT.TClass.GetClass(subkey.GetClassName()).InheritsFrom('TDirectory'):
                        subObj=subkey.ReadObj()
                        l = subObj.GetListOfKeys()
                        l[0].GetName()
                # status flag
                l = statusobj.GetListOfKeys()
                l[0].GetName()
                
            else:
                _dolsr(dirobj)
            del dirobj
        elif keyClass.InheritsFrom("TTree"):
            currObj=key.ReadObj()
            if currObj is None:
                print("WARNING TTree Object \"%s\" in file:directory \"%s\" is corrupt "\
                "keylen=%s numbytes=%s objlen=%s fseekkey=%s"%(name,dir.GetPath(),key.GetKeylen(),
                                                              key.GetNbytes(),key.GetObjlen(),key.GetSeekKey()))
                return 9
            else:
                nentries=currObj.GetEntriesFast()
                #print "Scanning tree Object \"%s\" in file:directory \"%s\" is corrupt "\
                #"keylen=%s numbytes=%s objlen=%s fseekkey=%s "%(name,dir.GetPath(),key.GetKeylen(),
                #                                              key.GetNbytes(),key.GetObjlen(),key.GetSeekKey()),
                #print "Scanning tree %s"%name,
                for j in range(nentries):
                    ientry=currObj.LoadTree(j)
                    if ientry<0:
                        break
                    nb=currObj.GetEntry(j)
                    if nb<=0:
                        continue
                #print " Done"
            currObj.Delete()
            del currObj
        else:
            currObj=key.ReadObj()
            if currObj is None:
                print("WARNING Object \"%s\" in file:directory \"%s\" is corrupt "\
                "keylen=%s numbytes=%s objlen=%s fseekkey=%s"%(name,dir.GetPath(),key.GetKeylen(),
                                                              key.GetNbytes(),key.GetObjlen(),key.GetSeekKey()))
                return 5
            currObj.Delete()
            del currObj
    return 0
def scanFiles(globstr='testOutFile*'):
    RV=_dolsrwrapper(globstr)
    return RV
if __name__ == "__main__" :
    if len(sys.argv)<2:
        RV=scanFiles()
    else:
        RV=scanFiles("%s"%(sys.argv[1]))
    sys.exit(RV)
