#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import ROOT

def main():
    from PathResolver import PathResolver

    tool = ROOT.PMGTools.PMGCrossSectionTool('MyXSectionTool')

    fn = '/eos/atlas/atlascerngroupdisk/asg-calib/dev/PMGTools/PMGxsecDB_mc16.txt'
    fn = PathResolver.FindCalibFile(fn)
    vv = ROOT.std.vector('std::string')()
    vv.push_back(fn)

    tool.readInfosFromFiles(vv)

    # take a ttbar sample as example ( users should get this from the EventInfo )
    sample_id = 410000

    print '%d sample loaded' % tool.getLoadedDSIDs().size()
    print
    print 'Sample dsid               = ', sample_id
    print 'Sample name               = ', tool.getSampleName(sample_id)
    print 'xsection [pb]             = ', tool.getSampleXsection(sample_id)
    print 'filter eff                = ', tool.getFilterEff(sample_id)
    print 'k factor                  = ', tool.getKfactor(sample_id)
    print 'xsection uncertainty      = ', tool.getXsectionUncertainty(sample_id)
    print 'xsection uncertainty up   = ', tool.getXsectionUncertaintyUP(sample_id)
    print 'xsection uncertainty down = ', tool.getXsectionUncertaintyDOWN(sample_id)
       
    print

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine('.x $ROOTCOREDIR/scripts/load_packages.C')
    main()
