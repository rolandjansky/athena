#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import EventAnalyzer
import optparse

ROOT.gROOT.ProcessLine('.L Loader.C+')


import time
import sys




def EventLoop(tree, outputhistfile, nDarkPhotons):

    eventanalyzer = EventAnalyzer.EventAnalyzer( histControlName=outputhistfile, numberOfDarkPhotons=nDarkPhotons )
    nBadOnes = 0
    nEvents = tree.GetEntries()

    # setup toolbar
    toolbar_width = 50
    sys.stdout.write("Process: [%s]" % (" " * toolbar_width))
    sys.stdout.flush()
    sys.stdout.write("\b" * (toolbar_width+1)) # return to start of line, after '['

    nEvents = tree.GetEntries()
    nEventsForUpdate = int(nEvents/toolbar_width)
    
    for (nevent,event) in enumerate(tree):
        if (nevent %nEventsForUpdate) == 0:
            sys.stdout.write('#')
            sys.stdout.flush()
            pass
        if not eventanalyzer.processEvent(event):
            nBadOnes += 1
            pass
        pass
    
    sys.stdout.write("\n")

    print 'Total events:', nEvents
    print 'Total bad events:', nBadOnes

    eventanalyzer.finalize()


if __name__=='__main__':
    '''Helper function to quickly inspect the NTUP_TRUTH file, created from the EVGEN file.
    Before processing it through the entire simulation, digitization and reconstruction chain,
    it is wise to make sure the EVGEN file is as intended.
    '''
    
    parser = optparse.OptionParser()
    parser.add_option('-f', '--file',           dest='file_in',                               help='path to NTUP_TRUTH file to inspect' )
    parser.add_option('-t', '--tree',           dest='tree_name',                  default='truth',       help='Name of tree in NTUP_TRUTH file'    )
    parser.add_option('-n', '--nDarkPhotons',   dest='nDarkPhotons',   type='int', default=1,             help='Number of dark photons'    )
    parser.add_option('-o', '--outputhistfile', dest='outputhistfile',             default='histControl', help='name of ROOT file with histograms'  )

    (options, args) = parser.parse_args()

    if not options.file_in:
        print 'ERROR: input file (./inspect_truth_file.py -f <path to NTUP_TRUTH file>) needed'
        exit(0)
        pass

    ntup_file = ROOT.TFile.Open(options.file_in, 'r')
    if not ntup_file:
        print 'Path provided to NTUP_TRUTH file is incorrect. Or something is wrong with the file.'
        pass
    
    truth_tree = ROOT.TTree()

    try:
        ntup_file.GetObject(options.tree_name, truth_tree)
    except LookupError:
        print ' '
        print 'ERROR: ', options.tree_name, 'is not a TTree in', options.file_in
        print 'ERROR: ', 'provide the correct TTree name using (-t)'
        print 'ERROR: ', 'Stuff inside', options.file_in
        ntup_file.ls()
        exit(0)

    EventLoop(truth_tree, options.outputhistfile, options.nDarkPhotons)
