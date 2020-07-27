#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Print number of histograms in ROOT file"""

import sys
import ROOT

opts = None

def filledBins(h):
    N = h.GetSize()
    return sum([1 for i in range(N) if h.At(i)!=0])

def hasLabels(h):
    return (h.GetXaxis().GetLabels() is not None)

def missingLabels(h):
    l = h.GetXaxis().GetLabels()    
    return (l is not None and h.GetXaxis().GetNbins()!=l.GetSize())
            
def addDirList(dir,path,hists):
    list=dir.GetListOfKeys()
    for key in list:
        name=key.GetName()
        cname=key.GetClassName()
        if cname=='TDirectoryFile':
            addDirList(dir.GetDirectory(name),path+name+'/',hists)
        else:
            h = key.ReadObj()
            if not h.InheritsFrom('TH1'): continue
            if opts.labeled is True and not hasLabels(h): continue
            if opts.misslabel is True and not missingLabels(h): continue
            if opts.empty is True and h.GetEntries()>0: continue
            b = filledBins(h) if opts.filled else h.GetSize()
            hists[path+name]=(cname,b)
        
def list(name,path='/'):
    file=ROOT.TFile(name)
    file.cd(path)
    hists={}
    addDirList(ROOT.gDirectory,path,hists)
    return hists

def byName(hists, nameFunc):
    """Return dict {Alg : (nHists,Bins)}"""
    algs = {}
    for entry in sorted(hists):
        name = nameFunc(entry)        
        bins = hists[entry][1]

        if name in algs:
            algs[name][0] += 1
            algs[name][1] += bins
        else:
            algs[name] = [1,bins]

    return algs

def byAlg(hists):
    return byName(hists, lambda h : h.split('/',2)[1])


def main():    
   import optparse
   parser = optparse.OptionParser(description=__doc__,
                                  usage='%prog [Options] file [path]')

   parser.add_option('-t', '--total', action='store_true',
                     help='Print total number of histograms and bins')

   parser.add_option('-b', '--bins', action='store_true',
                     help='Sort by number of bins')

   parser.add_option('-n', '--byName', action='store_true',
                     help='Show histograms [default]')
   
   parser.add_option('-a', '--byAlg', action='store_true',      
                     help='Show total histograms per algorithm')

   parser.add_option('-f', '--filled', action='store_true',      
                     help='Show number of filled bins instead of total bins')

   parser.add_option('-l', '--labeled', action='store_true',      
                     help='Only show histograms with text labels')

   parser.add_option('-m', '--misslabel', action='store_true',      
                     help='Only show labeled histograms with at least one unlabeled bin')

   parser.add_option('-e', '--empty', action='store_true',      
                     help='Only show histograms with zero entries')
   
   global opts
   (opts, args) = parser.parse_args()
   
   if len(args)==1:
       path = '/'
   elif len(args)==2:
       path = args[1]
   else:
       parser.print_help()
       return 1

   hists = list(args[0],path)
   if opts.bins: sortKey = lambda x : x[1][1]
   else: sortKey = None

   if not opts.byAlg: opts.byName = True

   if opts.byName is True:
       for h,v in sorted(hists.items(), key=sortKey):
           print('%-80s %10s %10s' % (h,v[0],v[1]))

   if opts.byAlg is True:
       algs = byAlg(hists)
       for h,v in sorted(algs.items(), key=sortKey):
           print('%-80s %10s %10s' % (h,v[0],v[1]))

   if opts.total:
       print()
       print("Total histograms: %15s" % len(hists))
       print("Total %sbins:       %15s" % ("filled " if opts.filled else "", sum([h[1] for h in hists.values()])))

       
if __name__ == '__main__':
   try:
      sys.exit(main())
   except IOError as e:
      (code, msg) = e
      if (code==32): pass   # ignore broken pipe exception
      else: raise e
   except KeyboardInterrupt:
      sys.exit(1)

