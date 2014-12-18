#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: checkxAOD.py 619905 2014-10-03 16:11:12Z krasznaa $
#
# This is a modified version of PyUtils/bin/checkFile.py. It has been taught
# how to sum up the sizes of all the branches belonging to a single xAOD
# object/container.
#

__version__ = "$Revision: 619905 $"
__author__  = "Sebastien Binet <binet@cern.ch>, " \
    "Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>"

import sys
import os
import re

from optparse import OptionParser

if __name__ == "__main__":

    parser = OptionParser( usage = "usage: %prog [-f] my.xAOD.file.pool.root" )
    p = parser.add_option
    p( "-f",
       "--file",
       dest = "fileName",
       help = "The path to the POOL file to analyze" )
    p( "-c",
       "--csv",
       dest = "csvFileName",
       help = "Output CSV file name, to use with spreadsheets" )
    ( options, args ) = parser.parse_args()

    fileNames = []

    if len( args ) > 0:
        fileNames = [ arg for arg in args if arg[ 0 ] != "-" ]
        pass

    if options.fileName == None and len( fileNames ) == 0:
        str( parser.print_help() or "" )
        sys.exit( 1 )

    if options.fileName != None:
        fileName = os.path.expandvars( os.path.expanduser( options.fileName ) )
        fileNames.append( fileName )
        pass

    fileNames = set( fileNames )

    # Check the consistency with the CSV output:
    if len( fileNames ) > 1 and options.csvFileName:
        print( "WARNING  CSV output is only available when processing a single "
               "input file" )
        pass

    # Loop over the specified file(s):
    for fileName in fileNames:

        # Open the file:
        import PyUtils.PoolFile as PF
        poolFile = PF.PoolFile( fileName )

        # Loop over all the branches of the file, and sum up the information
        # about them in a smart way...
        summedData = {}
        for d in poolFile.data:
            # Skip metadata/TAG/etc. branches:
            if d.dirType != "B": continue
            # The name of this branch:
            brName = d.name
            # Check if this is a static auxiliary store:
            m = re.match( "(.*)Aux\..*", d.name )
            if m:
                # Yes, it is. And the name of the main object/container is:
                brName = m.group( 1 )
                pass
            # Check if this is a dynamic auxiliary variable:
            m = re.match( "(.*)AuxDyn\..*", d.name )
            if m:
                # Oh yes, it is. Let's construct the name of the main
                # object/container:
                brName = m.group( 1 )
                pass
            # Check if we already know this container:
            if brName in summedData.keys():
                summedData[ brName ].memSize  += d.memSize
                summedData[ brName ].diskSize += d.diskSize
            else:
                summedData[ brName ] = \
                    PF.PoolRecord( brName,
                                   d.memSize,
                                   d.diskSize,
                                   d.memSizeNoZip,
                                   d.nEntries,
                                   d.dirType )
                pass
            pass

        # Order the records by size:
        orderedData = []
        for br in summedData.keys():
            orderedData += [ summedData[ br ] ]
            pass
        sorter = PF.PoolRecord.Sorter.DiskSize
        import operator
        orderedData.sort( key = operator.attrgetter( sorter ) )

        # Access the CollectionTree directly:
        import ROOT
        tfile = ROOT.TFile.Open( fileName )
        ttree = tfile.Get( "CollectionTree" )

        # Print a header:
        print( "" )
        print( "=" * 80 )
        print( "         Event data" )
        print( "=" * 80 )
        print( PF.PoolOpts.HDR_FORMAT %
               ( "Mem Size", "Disk Size", "Size/Evt", "Compression",
                 "Items", "Container Name (Type)" ) )
        print( "-" * 80 )

        # Now, let's print the event-wise info that we gathered:
        memSize = 0.0
        diskSize = 0.0
        for d in orderedData:
            if d.nEntries != poolFile.dataHeader.nEntries: continue
            nameType = "%s (%s)" % \
                ( d.name, ttree.GetBranch( d.name ).GetClassName() )
            print( PF.PoolOpts.ROW_FORMAT %
                   ( d.memSize,
                     d.diskSize,
                     ( d.diskSize / d.nEntries ),
                     ( d.memSize / d.diskSize ),
                     d.nEntries,
                     nameType ) )
            memSize = memSize + d.memSize
            diskSize = diskSize + d.diskSize
            pass
        print( "-" * 80 )
        print( PF.PoolOpts.ROW_FORMAT %
               ( memSize,
                 diskSize,
                 ( diskSize / poolFile.dataHeader.nEntries ),
                 0.0,
                 poolFile.dataHeader.nEntries,
                 "Total" ) )
        print( "=" * 80 )
        print( "         Meta data" )
        print( "=" * 80 )
        print( "     Mem Size       Disk Size         Container Name" )
        print( "-" * 80 )

        # Now print the info about the metadata:
        memSize = 0.0
        diskSize = 0.0
        for d in orderedData:
            if d.nEntries == poolFile.dataHeader.nEntries: continue
            print( "%12.3f kb %12.3f kb       %s" %
                   ( d.memSize, d.diskSize, d.name ) )
            memSize = memSize + d.memSize
            diskSize = diskSize + d.diskSize
            pass
        print( "-" * 80 )
        print( "%12.3f kb %12.3f kb       %s" %
               ( memSize, diskSize, "Total" ) )
        print( "=" * 80 )

        # Write out a CSV file if one was requested:
        if options.csvFileName and ( len( fileNames ) == 1 ):
            # Open the output file:
            import csv
            with open( options.csvFileName, "wb" ) as f:
                writer = csv.writer( f )
                # Set up the formatting of the file:
                writer.writerow( [ "Name (Type)", "Size/Evt" ] )
                # Write all entries to it:
                for d in orderedData:
                    # Skip metadata items:
                    if d.nEntries != poolFile.dataHeader.nEntries: continue
                    # Construct the name of the entry:
                    nameType = "%s (%s)" % \
                        ( d.name, ttree.GetBranch( d.name ).GetClassName() )
                    # Write the entry:
                    writer.writerow( [ nameType, d.diskSize / d.nEntries ] )
                    pass
                pass
            pass

        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames

    print "## Bye."
    sys.exit( 0 )
