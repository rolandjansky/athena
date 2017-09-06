#!/usr/bin/env python

# @file:    dumpVersionTags.py
# @purpose: read an Event file and dump the version tags. 
#           This requires a EventInfo object to exist in the first event.
# @author:  RD Schaffer <R.D.Schaffer@cern.ch>
# @date:    February 2008
#
# @example:
# @code
# dumpVersionTags.py aod.pool.root
# dumpVersionTags.py -f aod.pool.root
# @endcode
#
__version__ = "$Id: dumpVersion.py,v 1.1 2009-03-30 08:31:49 johan Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"

import user
import sys
import os
# Turn off X11 requirement in ROOT
sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
import ROOT
# Turn off X11 requirement in ROOT
sys.argv.remove('-b')
import AthenaROOTAccess.transientTree

from optparse import OptionParser


# function selectEventInfo:
#
# Used to select only the Data Header element corresponding to the
# EventInfo branch when building the transient tree.
#
# This is needed to 1) be more efficient, 2) to be more robust, for
# example, be able to read ESD where not all branches can be converted
# to the transient tree
def selectEventInfo(elem):
    #print " selectEventInfo: clid ", elem.pClid()
    if elem.pClid() == 2101:
        #print " selectEventInfo: found clid "
        return elem
    return False

if __name__ == "__main__":


    # Skip branches with problems
    #AthenaROOTAccess.transientTree._skipBranches += ["Analysis::MuonContainer"]
    #AthenaROOTAccess.transientTree._skipBranches += ["MuonCaloEnergyContainer"]
    #AthenaROOTAccess.transientTree._skipBranches += ["MuidMuonCollection", "StacoMuonCollection"]
    #print "skip branches",
    #for b in AthenaROOTAccess.transientTree._skipBranches: print b,
    #if len(AthenaROOTAccess.transientTree._skipBranches) > 0: print " "

    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the POOL file for dumping tags" )
    parser.add_option( "-s",
                       "--skip",
                       dest = "skipBranches",
                       help = "'<list of collection keys to skip>', e.g. due to problems setting up the transient tree" )
    (options, args) = parser.parse_args()

    fileNames = []
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    if options.skipBranches != None:
        skipColls = os.path.expandvars(os.path.expanduser(options.skipBranches)).split()
        AthenaROOTAccess.transientTree._skipBranches += skipColls
        print "skip branches",
        for b in AthenaROOTAccess.transientTree._skipBranches: print b,
        if len(AthenaROOTAccess.transientTree._skipBranches) > 0: print " "
        print skipColls

    print "EXTRACTING TAGS!"
    
    fileNames = set( fileNames )
    sc = 0
    for fileName in fileNames:
        try:
            print "open file", fileName
            f = ROOT.TFile.Open (fileName)
            assert f.IsOpen()
            # Fill this in if you want to change the names of the transient branches.
            tt = AthenaROOTAccess.transientTree.makeTree(f, dhfilt = selectEventInfo)

            eibr = None
            for br in tt.GetListOfBranches():
                #print br
                if "EventInfo" == br.GetClassName(): eibr = br; print "Successfully found EventInfo"

            if eibr == None: print "Could not find branch for EventInfo";pass
            if eibr.GetEntry( 0 ) <= 0:
                print ":: Could not get McEventInfo for first event"
                pass

            #  access event info
            ei = getattr(tt, eibr.GetName())
            #  access to event id
            eid = ei.event_ID()
            #  access event type
            et = ei.event_type()
            ddt = et.get_detdescr_tags()
            tags = ddt.split()
            print " "
            print "run number: ", eid.run_number()
            print " "
            print "------------------------"
            print "tags: (tag name - value)"
            print "------------------------"
            print " "

            #print "tags size:", len(tags)
            #for i in range(len(tags)):
            #    print i,tags[i]
            tagDict = {}
            if len(tags) > 1:
                for i in range(0, len(tags), 2):
                    tagDict[tags[i]] = tags[i+1]
                keys = tagDict.keys()
                keys.sort()
                for key in keys:
                    print "%-50s  %s " % (key, tagDict[key])
            else:
                print "No version tags found - size:", len(tags)
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass

        except :
            print "## Caught something !! (don't know what)"
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 10
            pass
        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames
    
    print "## Bye."
    sys.exit(sc)


