#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import ROOT, os

#def main(filename, **args):
def main(**args):

    tool = ROOT.PMGTools.PMGCrossSectionTool('MyXSectionTool')

    tool.readInfosFromDir(os.getenv('ROOTCOREBIN')+'/data/PMGTools/')
    
    #take a ttbar sample as example ( user should get this from the EventInfo of course )
    sample_id = 410000

    print
    print 'sample name     = ',tool.getSampleName(sample_id)
    print 'xsection [pb]   = ',tool.getSampleXsection(sample_id)
    print 'filter eff      = ',tool.getFilterEff(sample_id)
    print 'branching ratio = ',tool.getBR(sample_id)
    print 'k factor        = ',tool.getKfactor(sample_id)
    print

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')
    main()
