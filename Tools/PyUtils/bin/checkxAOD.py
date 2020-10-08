#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
#
# This is a modified version of PyUtils/bin/checkFile.py. It has been taught
# how to sum up the sizes of all the branches belonging to a single xAOD
# object/container.
#

from __future__ import print_function

__author__  = "Sebastien Binet <binet@cern.ch>, " \
    "Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>, " \
    "RD Schaffer R.D.Schaffer@cern.ch"

import sys
import os
import re
import six
import operator

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

    # Set up categorization matching strings:
    categoryStrings = {
        "MetaData" : ["^DataHeader", "(.*)_mems$", "(.*)_timings$", "^Token$", "^RawInfoSummaryForTag$", "^index_ref$"],
        "Trig"     : ["^HLT", "^LVL1", "^xTrig", "^Trig", "^CTP_Decision", "^TrigInDetTrackTruthMap", "^TrigNavigation", ".*TriggerTowers", "TileTTL1MBTS", "^TileL2Cnt", "RoIBResult","^_TRIGGER","^L1TopoRawData"],
        "MET"      : ["^MET", "^METMAP", "JEMEtSums"],
        "EvtId"    : ["^ByteStreamEventInfo", "^EventInfo", "^McEventInfo", "^LumiBlockN", "^EventWeight", "^RunNumber", "^ConditionsRun", "^EventTime", "^BunchId", "^EventNumber","^IsTestBeam", "^IsSimulation", "^IsCalibration", "^AvgIntPerXing", "^ActualIntPerXing", "^RandomNumber", "^McChannel"], 
        "tau"      : ["^Tau", "^CombinedStauTrackParticles", "^ExtrapolatedStauTrackParticles","^finalTauPi0s","^DiTauJets"],
        "PFO"      : ["(.*)EventShape$", "^AntiKt4EMPFlowJets", "^JetETMissChargedParticleFlowObjects", "^JetETMissNeutralParticleFlowObjects", "^CHSChargedParticleFlowObjects", "^CHSNeutralParticleFlowObjects"],
        "egamma"   : ["^GSF", "^ForwardElectron", "^egamma", "^Electron", "^Photon"],
        "Muon"     : ["^Muon", "^TileMuObj", "^MS", "^SlowMuons", "^Staus", "(.*)MuonTrackParticles$", "MUCTPI_RDO", "^RPC", "^TGC", "^MDT", "^CSC", ".*MuonMeasurements$", "^ExtrapolatedMuonTracks", "^CombinedMuonTracks", "^NCB_MuonSegments"],
        "BTag"     : ["^BTag"],
        "InDet"    : ["^InDet", "^PrimaryVertices", "^ComTime_TRT", "^Pixel", "^TRT", "^SCT", "^BCM", "^CTP", "^Tracks", "^ResolvedForwardTracks", "^SplitClusterAmbiguityMap"],
        "Jet"      : ["^CamKt", "^AntiKt", "^Jet(?!.*ParticleFlowObjects$)","^LCOriginTopoClusters","^EMOriginTopoClusters"],
        "CaloTopo" : ["CaloCalTopoCluster"],
        "Calo"     : ["^LAr", "^AllCalo", "^AODCellContainer", "^MBTSContainer", "^CaloCompactCellContainer", "^E4prContainer", "^TileCellVec", "^TileDigits"],
        "Truth"    : ["^Truth", "Truth$", "TruthMap$", "TruthCollection$", "^PRD_MultiTruth", "TracksTruth$", ".*TrackTruth$", "TrackTruthCollection"],
        "AFP"      : ["^AFP"]
        }
    
    fileNames = []

    if len( args ) > 0:
        fileNames = [ arg for arg in args if arg[ 0 ] != "-" ]
        pass

    if options.fileName is None and len( fileNames ) == 0:
        str( parser.print_help() or "" )
        sys.exit( 1 )

    if options.fileName is not None:
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
        categData  = {}
        for d in poolFile.data:
            # Skip metadata/TAG/etc. branches:
            # if d.dirType != "B": continue
            # The name of this branch:
            brName = d.name
            # Check if this is a static auxiliary store:
            m = re.match( r"(.*)Aux\..*", d.name )
            if m:
                # Yes, it is. And the name of the main object/container is:
                brName = m.group( 1 )
                pass
            # Check if this is a dynamic auxiliary variable:
            m = re.match( r"(.*)AuxDyn\..*", d.name )
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
            # keep branches with either the same number of entries as the number of events, or the
            # special tlp branches with extra event information
            mtlp = re.match( "(.*)_tlp.$", d.name ) or re.match( "(.*)DataHeader(.*)", d.name )
            if d.nEntries != poolFile.dataHeader.nEntries and not mtlp: continue
            # print d.name

            br = ttree.GetBranch( d.name )
            d_name = d.name
            if br:
                m = re.match( "(.*)_[pv]._", d.name )
                m1 = re.match( "(.*)_tlp._", d.name )
                m2 = re.match( "(.*)_v.>_", d.name )
                m3 = re.match( "([a-zA-Z]+)_(.*_[lL]inks?)", d.name )
                if m:
                    nameType = "%s (%s)" % ( d.name[m.end():], br.GetClassName() )
                    d_name   = d.name[m.end():]
                elif m1:
                    nameType = "%s (%s)" % ( d.name[m1.end():], br.GetClassName() )
                    d_name   = d.name[m1.end():]
                elif m2:
                    nameType = "%s (%s)" % ( d.name[m2.end():], br.GetClassName() )
                    d_name   = d.name[m2.end():]
                elif m3:
                    nameType = "%s (%s)" % ( m3.group(2), br.GetClassName() )
                    d_name   = m3.group(2)
                else:
                    nameType = "%s (%s)" % ( d.name, br.GetClassName() )
            else:
                m = re.match( "(.*)_v._", d.name )
                m1 = re.match( "(.*)(_tlp.$)", d.name )
                # print "match",m,m1
                if m:
                    nameType = "%s (%s)" % ( d.name[m.end():], (d.name[:m.end()-1]) )
                    d_name   = d.name[m.end():]
                elif m1:
                    # print "m1:",m1.group(),m1.group(1)
                    nt = m1.group(1).replace("_",":") + m1.group(2)
                    n  = m1.group(1).replace("_",":")
                    nameType = "%s (%s)" % ( n, nt )
                    d_name   = n
                else:
                    nameType = "%s (%s)" % ( d.name, "()" )

            # Find category:
            found = False
            catName = '*Unknown*'
            for categ in categoryStrings:
                for pattern in categoryStrings[ categ ]:
                    # print d.name, d_name, pair, type(d.name), type(d_name), type(pair[0])
                    m = None
                    try:
                        m = re.match(pattern, d_name)
                    except TypeError:
                        pass
                    if m:
                        found = True
                        catName = categ
                        break
                        # print d.name, categ
                        pass
                    pass
                if not found:
                    # print "Did not find category for:", d.name, d_name, br
                    pass
                pass
            # Add on category to name/type
            nameType += ' [' + catName + ']'

            # Now sum up the sizes according to the category
            # Check if we already know this category:
            if catName in categData.keys():
                categData[ catName ].memSize  += d.memSize
                categData[ catName ].diskSize += d.diskSize
            else:
                categData[ catName ] = \
                    PF.PoolRecord( catName,
                                   d.memSize,
                                   d.diskSize,
                                   d.memSizeNoZip,
                                   d.nEntries,
                                   d.dirType )
                pass
            pass

            print( PF.PoolOpts.ROW_FORMAT %
                   ( d.memSize,
                     d.diskSize,
                     ( d.diskSize / poolFile.dataHeader.nEntries ),
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
        print( "" )

        # Now print out the categorized information
        # Order the records by size:
        categorizedData = []
        for br in categData.keys():
            categorizedData += [ categData[ br ] ]
            pass
        sorter = PF.PoolRecord.Sorter.DiskSize
        categorizedData.sort( key = operator.attrgetter( sorter ) )

        print( "=" * 80 )
        print( "         Categorized data" )
        print( "=" * 80 )
        print( "     Disk Size         Fraction    Category Name" )
        print( "-" * 80 )
        totDiskSize = 0.0
        frac        = 0.0
        ds     = []
        dsFrac = []
        dsName = []
        for d in categorizedData:
            dsPerEvt     = d.diskSize / poolFile.dataHeader.nEntries
            dsPerEvtFrac = d.diskSize / diskSize
            totDiskSize += dsPerEvt
            frac        += dsPerEvtFrac
            ds          += [dsPerEvt]
            dsFrac      += [dsPerEvtFrac]
            dsName      += [d.name]
            print( "%12.3f kb %12.3f       %s" % ( dsPerEvt, dsPerEvtFrac, d.name ) )
            pass
        print( "%12.3f kb %12.3f       %s" % ( totDiskSize , frac, "Total" ) )
        ds     += [totDiskSize]
        dsFrac += [frac]
        dsName += ["Total"]
        
        print( "" )
        print( "=" * 80 )
        print( "CSV for categories disk size/evt and fraction:" )
        # print out comment separated list in descending order
        print (",".join(dsName[::-1]))
        b = ['{:<0.3f}'.format(i)  for i in ds[::-1]]
        print (",".join(b))
        b = ['{:<0.3f}'.format(i)  for i in dsFrac[::-1]]
        print (",".join(b))
        print( "=" * 80 )
        print( "" )

        
        print( "=" * 80 )
        print( "         Meta data" )
        print( "=" * 80 )
        print( "     Mem Size       Disk Size         Container Name" )
        print( "-" * 80 )

        # Now print the info about the metadata:
        memSize = 0.0
        diskSize = 0.0
        for d in orderedData:
            mtlp = re.match( "(.*)_tlp.$", d.name ) or re.match( "(.*)DataHeader(.*)", d.name )
            if d.nEntries == poolFile.dataHeader.nEntries or mtlp: continue
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
            args = {} if six.PY2 else {'newline' : ''}
            with open( options.csvFileName, "w", **args ) as f:
                writer = csv.writer( f )
                # Set up the formatting of the file:
                writer.writerow( [ "Name (Type)", "Size/Evt" ] )
                # Write all entries to it:
                for d in orderedData:
                    # Skip metadata items:
                    if d.nEntries != poolFile.dataHeader.nEntries: continue
                    # Construct the name of the entry:
                    br = ttree.GetBranch(d.name)
                    if not br: continue
                    nameType = "%s (%s)" % \
                        ( d.name, br.GetClassName() )
                    # Write the entry:
                    writer.writerow( [ nameType, d.diskSize / d.nEntries ] )
                    pass
                pass
            pass

        if len(fileNames) > 1:
            print ("")
        pass # loop over fileNames

    print ("## Bye.")
    sys.exit( 0 )
