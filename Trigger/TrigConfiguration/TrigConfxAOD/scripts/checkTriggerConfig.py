#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: checkTriggerConfig.py 657274 2015-03-27 10:15:06Z krasznaa $
#
# This is a script to conveniently print the trigger configuration of
# an xAOD file.
#

## C/C++ style main function
def main():

    # Set up the command line option parser:
    from optparse import OptionParser
    parser = OptionParser( usage = "%prog [options] [-f] xAOD.pool.root" )
    parser.add_option( "-f", "--file", dest = "fileName",
                       action = "store", type = "string", default = "",
                       help = "Path to the file to check" )
    parser.add_option( "-d", "--chainsdetail", dest = "chainsDetail",
                       action = "store_true",
                       help = "Print detailed chain definitions" )
    ( options, args ) = parser.parse_args()

    # Get the file name(s):
    fileNames = []
    if len( args ) > 0:
        fileNames = [ arg for arg in args if arg[ 0 ] != "-" ]
        pass
    if options.fileName != "":
        import os.path
        fileNames.append(
            os.path.expandvars( os.path.expanduser( options.fileName ) ) )
        pass
    if len( fileNames ) == 0:
        parser.print_help()
        return 1
    fileNames = set( fileNames )

    # Set up the environment for xAOD access:
    import ROOT
    ROOT.gROOT.Macro( "$ROOTCOREDIR/scripts/load_packages.C" )

    # Set up a TChain with the input file(s):
    chain = ROOT.TChain( "CollectionTree" )
    for fname in fileNames:
        chain.Add( fname )
        pass

    # And create the objects used to read this chain:
    tree = ROOT.xAOD.MakeTransientTree( chain, ROOT.xAOD.TEvent.kBranchAccess )
    from xAODRootAccess.TPyStore import TPyStore
    store = TPyStore()

    # Set up the tool used in the printout:
    tool = ROOT.TrigConf.xAODConfigTool( "xAODConfigTool" )
    if tool.initialize().isFailure():
        print( "ERROR Couldn't initialise the trigger configuration tool" )
        return 1

    # The keys of the configuration printed last:
    smk = 0
    l1psk = 0
    hltpsk = 0

    # Loop over the events of the file, to encounter all configurations in it:
    for entry in xrange( tree.GetEntries() ):
        # Load the event:
        if tree.GetEntry( entry ) < 0:
            print( "ERROR Failed to load entry %i from the input" % entry )
            return 1
        # Always print the configuration for the first event, or when the
        # configuration changes:
        if( ( entry == 0 ) or
            ( smk != tool.masterKey() ) or
            ( l1psk != tool.lvl1PrescaleKey() ) or
            ( hltpsk != tool.hltPrescaleKey() ) ):
            # Remember the keys:
            smk    = tool.masterKey()
            l1psk  = tool.lvl1PrescaleKey()
            hltpsk = tool.hltPrescaleKey()
            # Print the basic info:
            print( "Configuration for SMK: %i, L1PSK: %i, HLTPSK: %i" % \
                       ( smk, l1psk, hltpsk ) )
            detailLevel = 1
            if options.chainsDetail:
                detailLevel = 3
                pass
            getattr( tool.ctpConfig().menu(), "print" )( "", detailLevel )
            getattr( tool.chains(), "print" )( "", detailLevel )

            pass
        pass

    # Return gracefully:
    return 0

# Run the main() function:
if __name__ == "__main__":
    import sys
    sys.exit( main() )
