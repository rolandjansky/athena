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

# yeah, this is sloppy, but hey, this is just a utility script.
from ROOT import * 

# command line options
parser = OptionParser(usage="usage: checkPAUContents.py [options]")
parser.add_option("-f", dest="InputFile", metavar="FILE", default="PAUReco.root", help="ROOT file which contains the PAUReco TTree.")
parser.add_option("-n", dest="NumBranches", type="int", default=10, help="Value of X in the 'Top X Branches' lists")
parser.add_option("-t", dest="TreeName", default="PAUReco", help="Name of TTree in TFile")
parser.add_option("-u", dest="PrintCompressionInfo", default=False, help="Print information related to file compression")
(options, args) = parser.parse_args()


# Get the TFile and the TTree
input_file = TFile(options.InputFile,"RO")
PAUReco = input_file.Get(options.TreeName)


print "########################################################################"
print "# General Information                                                   "
print "#                                                                       "

ttree_branches = PAUReco.GetListOfBranches()
num_branches = ttree_branches.GetEntries()

print "Number of Branches       : %15d" % (num_branches)
print "Uncompressed Size [Bytes]: %15d" % (PAUReco.GetTotBytes())
print "File Size [Bytes]        : %15d" % (PAUReco.GetZipBytes())

print "Number of Events         : %15d" % (PAUReco.GetEntries())
print "Number of Photons        : %15d" % (PAUReco.GetEntries("ph_pt>0"))

print "kB/event                 : %17.1f"% (PAUReco.GetZipBytes()/(PAUReco.GetEntries()*1000))

branch_totalbytes_dict = {}
branch_zipbytes_dict = {}
branch_compress_dict = {}
longest_name = 0
for b in ttree_branches:
    name = b.GetName()
    bytes = b.GetTotalSize()
    branch_totalbytes_dict[name] = bytes
    branch_zipbytes_dict[name] = b.GetZipBytes()
    branch_compress_dict[name] = (bytes+.00001)/b.GetZipBytes()

    if len(name) > longest_name:
        longest_name = len(name)



print "########################################################################"
print "# Top " , options.NumBranches , " largest branches, file size           "

for i in nlargest(options.NumBranches, branch_zipbytes_dict.iteritems(), key=itemgetter(1)):
    print '        %s        %10d Bytes (%2d%%)' % (i[0].ljust(longest_name),i[1],100*i[1]/PAUReco.GetZipBytes())

                                              
if options.PrintCompressionInfo:
    print "########################################################################"
    print "# Top " , options.NumBranches , " largest branches, total bytes         "

    for i in nlargest(options.NumBranches, branch_totalbytes_dict.iteritems(), key=itemgetter(1)):
        print '        %s        %10d Bytes' % (i[0].ljust(longest_name),i[1])

                                              
    print "########################################################################"
    print "# Top " , options.NumBranches , " most compressed branches              "

    for i in nlargest(options.NumBranches, branch_compress_dict.iteritems(), key=itemgetter(1)):
        print '        %s        %10d times' % (i[0].ljust(longest_name),i[1])

                                              
    print "########################################################################"
    print "# Top " , options.NumBranches , " least compressed branches             "

    for i in nsmallest(options.NumBranches, branch_compress_dict.iteritems(), key=itemgetter(1)):
        print '        %s        %10d times' % (i[0].ljust(longest_name),i[1])


print "########################################################################"
print "# Block Sizes (disk space in file)                                      "

# now, let's go through and figure out the block structure
block_zipbytes_dict = {}
for (bname, bsize) in branch_zipbytes_dict.items():

    # find the first "_", which will define the block
    blockname=bname[0:bname.find('_')]
    if blockname == '':
        continue
    elif blockname in block_zipbytes_dict:
        block_zipbytes_dict[blockname] += bsize
    else:
        block_zipbytes_dict[blockname] = bsize

for i in sorted(block_zipbytes_dict.iteritems(), key=itemgetter(1), reverse=True):
    print '        %s        %10d Bytes (%2d%%)' % (i[0].ljust(longest_name),i[1],100*i[1]/PAUReco.GetZipBytes())
                                              
print "########################################################################"

# What else do you want to know?  Add it here!


