#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This is a standalone implementation of the xAOD checking script. It just needs
# a functional ROOT installation to work.
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
#

# Import(s):
import os, re, operator, ROOT
from optparse import OptionParser

## C(++) style main function
#
# I just prefer writing executable Python code like this...
#
# @returns 0 on success, something else on failure
#
def main():

    # Set up the command line option parser:
    parser = OptionParser( usage = "usage: %prog [-f] xAOD.pool.root" )
    parser.add_option( "-f", "--file",
                       dest = "fileName",
                       help = "The path to the xAOD file to analyse" )
    ( options, args ) = parser.parse_args()

    # Set up categorization matching strings:
    categoryStrings = {
        "MetaData" : ["^DataHeader", "(.*)_mems$", "(.*)_timings$", "^Token$", "^RawInfoSummaryForTag$"],
        "Trig"     : ["^HLT", "^LVL1", "^xTrig", "^Trig", "^CTP_Decision", "^TrigInDetTrackTruthMap", "^TrigNavigation", ".*TriggerTowers", "TileTTL1MBTS", "^TileL2Cnt", "RoIBResult"],
        "MET"      : ["^MET", "^METMAP", "JEMEtSums"],
        "EvtId"    : ["^ByteStreamEventInfo", "^EventInfo", "^McEventInfo", "^LumiBlockN", "^EventWeight", "^RunNumber", "^ConditionsRun", "^EventTime", "^BunchId", "^EventNumber"],
        "tau"      : ["^Tau", "^DiTauJets"],
        "PFO"      : ["(.*)EventShape$", "^AntiKt4EMPFlowJets", "^JetETMissChargedParticleFlowObjects", "^JetETMissNeutralParticleFlowObjects", "^JetETMissLCNeutralParticleFlowObjects"],
        "egamma"   : ["^GSF", "^ForwardElectron", "^egamma", "^Electron", "^Photon"],
        "Muon"     : ["^Muon", "^TileMuObj", "^MS", "^SlowMuons", ".*Stau", "(.*)MuonTrackParticles$", "MUCTPI_RDO", "^RPC", "^TGC", "^MDT", "^CSC", "^sTGC", "^Micromegas", ".*MuonMeasurements$", "^ExtrapolatedMuonTracks", "^CombinedMuonTracks"],
        "BTag"     : ["^BTag"],
        "InDet"    : ["^InDet", "^PrimaryVertices", "^ComTime_TRT", "^Pixel", "^TRT", "^SCT", "^BCM", "^CTP", "^Tracks", "^ResolvedForwardTracks", "^SplitClusterAmbiguityMap"],
        "Jet"      : ["^CamKt", "^AntiKt", "^Jet"],
        "CaloTopo" : ["CaloCalTopoCluster"],
        "Calo"     : ["^LAr", "^AODCellContainer", "^MBTSContainer", "^CaloCompactCellContainer", "^E4prContainer", "^TileCellVec", "^TileDigits"],
        "Truth"    : ["^Truth", "Truth$", "TruthMap$", "TruthCollection$", "^PRD_MultiTruth", "TracksTruth$", ".*TrackTruth$", "TrackTruthCollection"]
        }
    # Get the file name(s), taking all options into account:
    fileNames = []
    if len( args ) > 0:
        fileNames = [ arg for arg in args if arg[ 0 ] != "-" ]
        pass
    if ( options.fileName == None ) and ( len( fileNames ) == 0 ):
        parser.print_help()
        return 1
    if options.fileName != None:
        fileNames.append(
            os.path.expandvars( os.path.expanduser( options.fileName ) ) )
        pass
    fileNames = set( fileNames )

    # Set up ROOT:
    ROOT.gErrorIgnoreLevel = ROOT.kError

    # Loop over the files:
    for fileName in fileNames:
        # Print their contents using the helper function:
        printFileInfo( fileName, categoryStrings )
        pass
    
    # Return gracefully:
    return 0

## Class for holding information about a single container
#
# Modeled on PoolRecord from the offline code...
#
class ContainerInfo( object ):
    ## Constructor
    def __init__( self, name, memSize, diskSize, nEntries ):
        # Call the base class's constructor:
        object.__init__( self )
        # Remember the properties:
        self._name     = name
        self._memSize  = memSize
        self._diskSize = diskSize
        self._nEntries = nEntries
        return
    ## The name of the container
    def name( self ):
        return self._name
    ## Size of the container in memory in kilobytes
    def memSize( self ):
        return float( self._memSize ) / 1024.
    ## Size of the container on disk in kilobytes
    def diskSize( self ):
        return float( self._diskSize ) / 1024.
    ## Average size of the container per event in kilobytes
    def diskSizePerEntry( self ):
        return self.diskSize() / self._nEntries
    ## Compression factor for the container
    def compression( self ):
        return self.memSize() / self.diskSize()
    ## The number of entries saved for this container
    def nEntries( self ):
        return self._nEntries

## Function printing information about a single xAOD file
#
# This function is used to print the contents of a given xAOD file.
#
# @param fileName The name of the file to print information about
#
def printFileInfo( fileName, categoryStrings ):

    # Open the file:
    f = ROOT.TFile.Open( fileName, "READ" )
    if not f or f.IsZombie():
        raise "Couldn't open file %s" % fileName

    # Get the main event tree from the file:
    t = f.Get( "CollectionTree" )
    if not t:
        raise "Couldn't find 'CollectionTree; in file %s" % fileName

    # The collected data:
    infoForCont = {}
    categData   = {}

    # The entries in the TTree, for cross-checking:
    entries = t.GetEntries()

    # Get all the branches of the file:
    branches = t.GetListOfBranches()
    for i in range( branches.GetEntries() ):
        # Get the branch:
        branch = branches.At( i )
        # A little security check:
        if branch.GetEntries() != entries:
            raise "Found %i entries in branch %s instead of %i" % \
              ( branch.GetEntries(), branch.GetName(), entries )
        # "Decode" the name of the branch:
        brName = branch.GetName()
        # Check if this is a static auxiliary branch:
        m = re.match( "(.*)Aux\..*", branch.GetName() )
        if m:
            brName = m.group( 1 )
            pass
        # Check if this is a dynamic auxiliary branch:
        m = re.match( "(.*)AuxDyn\..*", branch.GetName() )
        if m:
            brName = m.group( 1 )
            pass
        # Get the information that we need:
        if brName in infoForCont.keys():
            infoForCont[ brName ]._memSize  += branch.GetTotBytes( "*" )
            infoForCont[ brName ]._diskSize += branch.GetZipBytes( "*" )
        else:
            infoForCont[ brName ] = ContainerInfo( brName,
                                                   branch.GetTotBytes( "*" ),
                                                   branch.GetZipBytes( "*" ),
                                                   entries )
            pass
        pass

    # Sort the collected info based on the on-disk size of the containers:
    orderedData = []
    for cName in infoForCont.keys():
        orderedData += [ infoForCont[ cName ] ]
        pass
    orderedData.sort( key = operator.attrgetter( "_diskSize" ) )

    # Finally, print the collected information:
    memSize  = 0.0
    diskSize = 0.0
    print( "=" * 120 )
    print( " File: %s" % fileName )
    print( "-" * 120 )
    print( "    Memory size        Disk Size       Size/Event  Compression "
           "Entries  Name (Type)" )
    print( "-" * 120 )
    for d in orderedData:
        # Try to get the type of the object/container:
        intBr = t.GetBranch( d.name() )
        className = "<!!!Interface missing!!!>"
        if intBr:
            className = intBr.GetClassName()
            pass
        # The name and type of the branch in question:
        nameType = "%s (%s)" % ( d.name(), className )


        # Find category:
        found = False
        catName = '*Unknown*'
        # for pair in categoryStrings:
        for categ in categoryStrings:
            for pattern in categoryStrings[ categ ]:
                # print d.name, d_name, pair, type(d.name), type(d_name), type(pair[0])
                m = None
                try:
                    m = re.match(pattern, d.name())
                except TypeError:
                    pass
                if m:
                    found = True
                    catName = categ
                    break
                    # print d.name, pair[1]
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
            categData[ catName ]._memSize  += d._memSize
            categData[ catName ]._diskSize += d._diskSize
        else:
            categData[ catName ] = \
                ContainerInfo( catName,
                               d._memSize,
                               d._diskSize,
                               d._nEntries )
            pass
        pass
        memSize  = memSize  + d.memSize()
        diskSize = diskSize + d.diskSize()
        
        print( " %12.2f kB  %12.2f kB  %7.2f kB/event   %5.2f  %8i   %s" % \
               ( d.memSize(), d.diskSize(),
                 d.diskSizePerEntry(),
                 d.compression(),
                 d.nEntries(), nameType ) )
        pass
    print( "-" * 120 )
    print( " %12.2f kB  %12.2f kB  %7.2f kB/event                     Total" % \
           ( memSize, diskSize, diskSize/entries ) )
    print( "=" * 120 )
    print( "" )

    # Now print out the categorized information
    # Order the records by size:
    categorizedData = []
    for br in categData.keys():
        categorizedData += [ categData[ br ] ]
        pass
    categorizedData.sort( key = operator.attrgetter( "_diskSize" ) )

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
        #print d.name(), d.diskSize(), d.nEntries()
        dsPerEvt     = d.diskSize() / d.nEntries()
        dsPerEvtFrac = d.diskSize() / diskSize
        totDiskSize += dsPerEvt
        frac        += dsPerEvtFrac
        ds          += [dsPerEvt]
        dsFrac      += [dsPerEvtFrac]
        dsName      += [d.name()]
        print( "%12.3f kb %12.3f       %s" % ( dsPerEvt, dsPerEvtFrac, d.name() ) )
        pass
    print( "%12.3f kb %12.3f       %s" % ( totDiskSize , frac, "Total" ) )
    ds     += [totDiskSize]
    dsFrac += [frac]
    dsName += ["Total"]
    
    print( "" )
    print( "=" * 80 )
    print( "CSV for categories disk size/evt and fraction:" )
    # print out comment separated list in descending order
    print( ",".join(dsName[::-1]))
    b = ['{:<0.3f}'.format(i)  for i in ds[::-1]]
    print( ",".join(b))
    b = ['{:<0.3f}'.format(i)  for i in dsFrac[::-1]]
    print( ",".join(b))
    print( "=" * 80 )
    print( "" )

    return

# Run the main function in "normal mode":
if __name__ == "__main__":
    import sys
    sys.exit( main() )
