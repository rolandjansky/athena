#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    magnifyPoolFile.py
# @purpose: produce a new POOL file with N times the content of an input one.
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    May 2007
#
# @example:
#
# magnifyPoolFile.py 1000 aod.pool
# magnifyPoolFile.py 1000 aod.pool my.magnified.aod.pool
#

from __future__ import print_function

__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser(
        usage = "usage: %prog [-n] nMagnify [-i] input.pool [-o output.pool]"
        )

    parser.add_option(
        "-n",
        dest = "nMagnify",
        help = "The number of times the input file will be 'replicated'"
        )
    parser.add_option(
        "-i",
        "--input",
        dest = "inPoolFile",
        help = "Path to the input POOL file to be 'replicated'/'magnified'"
        )
    parser.add_option(
        "-o",
        "--output",
        dest = "outPoolFile",
        default = None,
        help = "Path to the output POOL file containing the replicated data"
        )

    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.nMagnify = args[0]
        pass

    if len(args) > 1 and args[1][0] != "-":
        options.inPoolFile = args[1]
        pass

    if len(args) > 2 and args[2][0] != "-":
        options.outPoolFile = args[2]
        pass

    if not options.nMagnify or \
       not options.inPoolFile :
        str(parser.print_help() or "ERROR")
        sys.exit(1)
        pass
    
    nMagnify = int(options.nMagnify)
    if nMagnify <= 1:
        print ("ERROR: you have to give an integer > 1 for the magnifier !!")
        str(parser.print_help() or "ERROR")
        sys.exit(1)
        pass
    
    inPoolFile = os.path.expandvars(os.path.expanduser(options.inPoolFile))

    if not options.outPoolFile:
        options.outPoolFile = os.path.join( [
            os.path.dirname(inPoolFile),
            "magnified."+os.path.basename(inPoolFile)
            ] )

    outPoolFile = os.path.expandvars(os.path.expanduser(options.outPoolFile))

    
    print ("#"*80)
    print ("## Magnifying POOL files...")
    print ("##  - replicator parameter:",options.nMagnify)
    print ("##  - input: ",inPoolFile)
    print ("##  - output:",outPoolFile)
    print ("##")

    oldArgs = sys.argv
    sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
    print ("## importing ROOT...")
    import ROOT
    print ("## importing ROOT... [DONE]")
    import RootUtils.PyROOTFixes  # noqa: F401

    sys.argv = oldArgs
    
    print ("## opening input Pool file...")
    inPoolFile = ROOT.TFile.Open( inPoolFile, "READ" )
    assert( inPoolFile.IsOpen() )
    print ("## opening input Pool file... [DONE]")

    trees = [ k.ReadObj() for k in inPoolFile.GetListOfKeys() ]

    print ("## creating output Pool file...")
    outPoolFile = ROOT.TFile.Open( outPoolFile, "RECREATE" )
    assert( outPoolFile.IsOpen() )
    print ("## creating output Pool file... [DONE]")
    
    print ("## initialize input file trees' branch status...")
    for tree in trees: tree.SetBranchStatus("*", 0)

    print ("## create output trees...")
    outTrees = [ ]
    for tree in trees:
        tree.SetBranchStatus("*", 1)
        outTrees.append( tree.CloneTree(0) )

    print ("## magnifying...")
    for m in range( nMagnify ):
        if nMagnify<10 or m % (nMagnify/10) == 0:
            print ("  ... %s" % str(m).zfill(8))
        for i in range(len(trees)):
            for j in range(trees[i].GetEntries()):
                trees[i].GetEntry(j)
                outTrees[i].Fill()
    print ("## magnifying... [DONE]")

    print ("## committing output file...")
    outPoolFile.Write()
    print ("## committing output file... [DONE]")

    inPoolFile.Close()
    del inPoolFile

    outPoolFile.Close()
    del outPoolFile

    print ("## Bye.")
    sys.exit(0)
    
