#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import ROOT
import os


def main(**args):  # def main(filename, **args):

    tool = ROOT.PMGTools.PMGCrossSectionTool('MyXSectionTool')

    tool.readInfosFromFiles('/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PMGTools/PMGxsecDB_mc16.txt')

    # take a ttbar sample as example ( users should get this from the EventInfo )
    sample_id = 410000

    print
    print 'Sample name          = ', tool.getSampleName(sample_id)
    print 'xsection [pb]        = ', tool.getSampleXsection(sample_id)
    print 'filter eff           = ', tool.getFilterEff(sample_id)
    print 'k factor             = ', tool.getKfactor(sample_id)
    print 'xsection uncertainty = ', tool.getXsectionUncertainty(sample_id)
    print

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')
    main()
