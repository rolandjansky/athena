#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: ut_trigconfxaod_xaodconfigtool_test.py 656622 2015-03-25 09:33:48Z krasznaa $
#
# Unit test for the main tool of the package, in the analysis release.
#

## C/C++ style main function
def main():

    # Set up the environment:
    import ROOT
    if not ROOT.xAOD.Init( "trig_test" ).isSuccess():
        print ( "Failed to call xAOD::Init(...)" )
        return 1

    # Open an input file for reading:
    FNAME = "/afs/cern.ch/atlas/project/PAT/xAODs/r5787/" \
      "mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.merge.AOD." \
      "e2928_s1982_s2008_r5787_r5853_tid01597980_00/" \
      "AOD.01597980._000098.pool.root.1"
    ifile = ROOT.TFile.Open( FNAME, "READ" )
    if not ifile:
        print( "Couldn't open input file: %s" % FNAME )
        return 1

    # Create the necessary transient objects:
    tree = ROOT.xAOD.MakeTransientTree( ifile, "CollectionTree",
                                        ROOT.xAOD.TEvent.kBranchAccess )
    import xAODRootAccess.GenerateDVIterators
    from xAODRootAccess.TPyStore import TPyStore
    store = TPyStore()

    # Create the tool to be tested:
    tool = ROOT.TrigConf.xAODConfigTool( "xAODConfigTool" )

    # Load the first event, and check the configuration belonging to it:
    if tree.GetEntry( 0 ) < 0:
        print( "Couldn't load the first event of the input file" )
        return 1
    if tool.masterKey() != 0 or tool.lvl1PrescaleKey() != 0 or \
      tool.hltPrescaleKey() != 0:
        print( "Wrong configuration keys detected" )
        return 1
    print( "Number of LVL1 items: %i" % tool.ctpConfig().menu().items().size() )
    if tool.ctpConfig().menu().items().size() != 105:
        print( "Wrong number of LVL1 items loaded" )
        return 1
    print( "Number of HLT chain: %i" % tool.chains().size() )
    if tool.chains().size() != 331:
        print( "Wrong number of HLT chains loaded" )
        return 1

    # Return gracefully:
    return 0

# Run the main function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
