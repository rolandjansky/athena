#!/bin/env python

# for smart division
from __future__ import division

import os
import glob
import sys

# for command line options
from optparse import OptionParser

# for sorting and such
from operator import itemgetter
from heapq import nlargest, nsmallest

# for buffer objects
import numpy

# yeah, this is sloppy, but hey, this is just a utility script.
from ROOT import ROOT, TFile, TTree, TH1F, gDirectory, gROOT

# command line options
parser = OptionParser(usage="usage: checkPAUContents.py [options]")
parser.add_option("-r", dest="ReferenceFile", metavar="FILE", default="PAUReco.root", help="ROOT file which contains the PAUReco TTree.")
parser.add_option("-f", dest="InputFile", metavar="FILE", default="PAUReco.root", help="ROOT file which contains the PAUReco TTree.")
parser.add_option("-t", dest="TreeName", default="PAUReco", help="Name of TTree in TFile")
(options, args) = parser.parse_args()


# Get the TFile and the TTree
input_file = TFile(options.InputFile,"RO")
refer_file = TFile(options.ReferenceFile,"RO")



PAUNew = input_file.Get(options.TreeName)
new_branches = PAUNew.GetListOfBranches()
num_new_branches = new_branches.GetEntries()

PAURef = refer_file.Get(options.TreeName)
ref_branches = PAURef.GetListOfBranches()
num_ref_branches = ref_branches.GetEntries()

branch_dict={}

########################################################################
# First things first - make sure both trees have all the same branches.
inconsistent=False
for nb in new_branches:
    new_name = nb.GetName()

    existsMatch=False
    for rb in ref_branches:
        ref_name = rb.GetName()
        if ref_name == new_name:
            branch_dict[new_name] = True
            existsMatch=True
            break
    if existsMatch == False:
        branch_dict[new_name] = False
        inconsistent=True
        print 'Could not find branch name ', new_name, ' in reference file.'

for rb in ref_branches:
    ref_name = rb.GetName()

    existsMatch=False
    for nb in new_branches:
        new_name = nb.GetName()
        if new_name == ref_name:
            branch_dict[ref_name] = True
            existsMatch=True
            break
    if existsMatch == False:
        branch_dict[ref_name] = False
        inconsistent=True
        print 'Could not find branch name ', new_name, ' in new file.'

if inconsistent == True:
    print 'Trees are inconsistent.  Checking branches that exist in both reference and test trees....'
########################################################################

########################################################################
# Next up, check the number of entries, just to be certain
if PAUNew.GetEntries() != PAURef.GetEntries():
    print 'Trees have mismatching entries.  Aborting.'
    exit(0)
########################################################################

########################################################################
# Now, the real work.  For each variable, draw all the results into a histogram.
# Compare the histograms for the two cases, allowing no differences between the two.
inconsistent=False
for b in new_branches:
    bname = b.GetName()

    if not bname in branch_dict or branch_dict[bname]==False:
        continue

    ref_hist_name = "ref_hist_"+bname
    new_hist_name = "new_hist_"+bname

    ref_cmd = bname+">>"+ref_hist_name
    new_cmd = bname+">>"+new_hist_name
    
    PAURef.Draw(ref_cmd,"","goff");
    PAUNew.Draw(new_cmd,"","goff");

    useHists=False
    if useHists:
        ref_hist = gROOT.FindObject(ref_hist_name)
        new_hist = gROOT.FindObject(new_hist_name)

        if ref_hist.GetNbinsX() != new_hist.GetNbinsX():
            print 'Variable name ', bname, ' has binning that is inconsistent across two trees: ', ref_hist.GetNbinsX(), ' != ', new_hist.GetNbinsX()
            inconsistent=True

        for i in range(0,ref_hist.GetNbinsX()+1):
            if ref_hist.GetBinContent(i) != ref_hist.GetBinContent(i):
                print 'Variable name ', bname, ' is inconsistent across two trees.'
                inconsistent=True
    elif PAURef.GetSelectedRows()>=0 and PAUNew.GetSelectedRows()>=0:
        ref_points = numpy.ndarray(PAURef.GetSelectedRows(), dtype=numpy.double, buffer=PAURef.GetV1())
        new_points = numpy.ndarray(PAUNew.GetSelectedRows(), dtype=numpy.double, buffer=PAUNew.GetV1())

        if len(ref_points) != len(new_points):
            print 'Variable name ', bname, ' has ', len(ref_points), ' reference points and ', len(new_points), ' new points.  Inconsistent.'
            inconsistent=True

        #if not inconsistent:
        for i in range(0, len(ref_points)):
            if(len(new_points)<i):
                print 'Variable name ', bname, ' doesn\'t have an entry at point ', i, ' in new tree, but exists in reference.  Inconsistent.'
                continue
            
            diff = float(ref_points[i] - new_points[i])
            if diff < 0: diff=-diff;
            if diff > .00001:
                print 'Variable name ', bname, ' at point ', i, ' has reference=', ref_points[i], ' and new=', new_points[i], '.  Inconsistent.'
                inconsistent=True
    elif PAURef.GetSelectedRows()<0:
        print "No entries for reference for variable "+bname
    elif PAUNew.GetSelectedRows()<0:
        print "No entries for new file for variable "+bname

if inconsistent:
    print 'Trees are not consistent.'
else:
    print 'Trees are consistent.'
########################################################################
