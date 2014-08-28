#!/bin/env python

from optparse import OptionParser
parser = OptionParser(usage="usage: checkPAUContents.py [options]")
parser.add_option("-r", dest="ReferenceFile", metavar="FILE", default="PAUReco.root", help="ROOT file which contains the PAUReco TTree.")
parser.add_option("-f", dest="InputFile", metavar="FILE", default="PAUReco.root", help="ROOT file which contains the PAUReco TTree.")
(options, args) = parser.parse_args()

import sys
import os

def branchesFileName(infile):
    return infile+".branches"

def make_var_file(infile):
    outfile=branchesFileName(infile)
    os.system("rm "+outfile)
    from ROOT import gROOT
    execute="TFile f(\""+infile+"\"); TTree* t = (TTree*)f.Get(\"photon\"); t->Print(); >> "+outfile
    gROOT.ProcessLine(execute)


make_var_file(options.InputFile    )
make_var_file(options.ReferenceFile)

orig_file=open(branchesFileName(options.ReferenceFile),"r")
new_file=open(branchesFileName(options.InputFile),"r")

new_file_lines=new_file.readlines()
new_file.close()

for line in orig_file:
    words=line.split()
    if words[0] != "*Br":
        continue
    branchname=words[2][1:]

    foundBranch=False
    for i in range(len(new_file_lines)):
        if new_file_lines[i].find(branchname) != -1:
            foundBranch=True
            break
    if not foundBranch:
        print "New file is missing branch ", branchname
