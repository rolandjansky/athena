#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#----------------------------------------------------------------------
#stand-alone script to merge specific directories of NTUP_PHYSVAL files 
#author: philipp.mogg@cern.ch
#16 May 2016
#----------------------------------------------------------------------

from __future__ import print_function
import os,glob,argparse,ROOT,time

start = time.clock()

parser = argparse.ArgumentParser(description='Merge specific folder(s) in root files.')
parser.add_argument("-i", "--input", help="path to the folder holding the samples (default: ./)", default=os.getcwd())
parser.add_argument("-o", "--output", help="path for the output (default: ./merge.root", default=os.getcwd()+"/merge.root")
parser.add_argument("-d", "--dir", nargs='+', help="ROOT directory to be merged, multiple arguments are possible (default: Summary)", default=["Summary"])
parser.add_argument("-p", "--pattern", help='pattern of files to merge (default: "*PHYSVAL*")', default="*PHYSVAL*")
args = parser.parse_args()
folder = os.path.abspath(args.input)
mergeDirs = args.dir
origDir = os.getcwd()
out = args.output
pattern = args.pattern
output_file = os.path.abspath(args.output)
os.chdir(folder)

files = glob.glob(folder + "/" + pattern)

f = ROOT.TFile(output_file, "recreate")
folder = os.getcwd()
f2 = ROOT.TFile(files[1])

print("Target file: " + output_file)
for infile in files:
    print("Found input file: " + infile)
    if os.path.samefile(output_file, infile):
        print("Please make sure that the output file is not part of the input files! Stopping.")
        quit()

errors = []

def mergeFolder(path) :
    print("Merging folder " + path)
    d = f2.Get(path)
    if not d:
        error = "ERROR: Cannot find directory " + path + ". Omitting."
        print(error)
        errors.append(error)
        return
    dirlist = d.GetListOfKeys()
    for subdir in dirlist:
        obj = subdir.ReadObj()
        if obj.IsA().InheritsFrom(ROOT.TH1.Class()):
            print("Now merging "+obj.GetName())
            h1 = obj
            hpath = d.GetPath()
            hname = hpath[hpath.find(":")+2:]+"/"+obj.GetName()
            print("Path: "+hname)
            for tup in files:
                if tup==files[1]: continue
                nextfile = ROOT.TFile(tup)
                h2 = nextfile.Get(hname)
                if not h2:
                    error = "ERROR: Cannot find " + hname + " in file " + tup + ". Omitting."
                    print(error)
                    errors.append(error)
                    continue
                h1.Add(h2)
            subfolder = f.Get(hpath[hpath.find(":")+2:])
            subfolder.cd()
            h1.Write()
        if obj.IsA().InheritsFrom(ROOT.TDirectory.Class()):
            print("Found subdirectory "+obj.GetName())
            hpath = obj.GetPath()
            subfolder = f.mkdir(hpath[hpath.find(":")+2:],obj.GetTitle())
            subfolder.cd()
            mergeFolder(hpath[hpath.find(":")+2:])

for mergeDir in mergeDirs:
    newfolder = f.mkdir(mergeDir,mergeDir)
    ROOT.TH1.AddDirectory(False)
    mergeFolder(mergeDir)

f.Close()
if len(errors)>0:
    print("Summary of all errors:")
    for phrase in errors:
        print(phrase)

end = time.clock()
print("Wall time used: %s sec" % (end - start))
