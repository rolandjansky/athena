#!/usr/bin/env python

# @file:    dumpRunNumber.py
# @purpose: read an Event file and dump the run number.
#           This requires a EventInfo object to exist in the first event.
# @author:  RD Schaffer <R.D.Schaffer@cern.ch>
# @date:    June 2008
#
# @example:
# @code
# dumpRunNumber.py aod.pool.root
# dumpRunNumber.py -f aod.pool.root
# @endcode
#
__version__ = "$Id: dumpRunNumber.py,v 1.4 2008-08-25 10:17:35 schaffer Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"

import user
import sys, traceback
import os, re
from time import ctime
# Turn off X11 requirement in ROOT
sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
import cppyy
import ROOT
# Turn off X11 requirement in ROOT
sys.argv.remove('-b')
#import AthenaROOTAccess.transientTree

from optparse import OptionParser

if __name__ == "__main__":

    # read in and treat options 
    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool")
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the POOL file for dumping the run number" )
    parser.add_option( "-n",
                       "--nEvents",
                       type = "int",
                       dest = "nEvents",
                       help = "'<number of event to print out>', default is 1" )
    parser.add_option( "-s",
                       "--startEvent",
                       type = "int",
                       dest = "startEvent",
                       help = "'<event to start with>', default is 0 - first event" )
    parser.add_option( "-d",
                       "--detailed-dump",
                       action  = "store_true",
                       dest    = "doDetailedDump",
                       default = False,
                       help = "flag to dump out lumiblock, event number and time" )
    (options, args) = parser.parse_args()

    fileNames = []
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    # Files
    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)
    fileNames = set( fileNames )

    # Number of events
    nEvents    = 1
    startEvent = 0
    if options.nEvents != None:
        nEvents = options.nEvents
    if options.startEvent != None:
        startEvent = options.startEvent

    # Detailed dump
    doDetailedDump = False
    if options.doDetailedDump:
        doDetailedDump = True

    print "files:", fileNames

    print "evts, start, doDetailed", nEvents, startEvent, doDetailedDump
    i1 = startEvent
    i2 = nEvents + startEvent 
    #print "i1, i2", i1, i2
    
    sc = 0
    for fileName in fileNames:
        try:
            print "open file", fileName
            f = ROOT.TFile.Open (fileName)
            assert f.IsOpen()

            # Look for the different possible trees that we might be
            # dealing with:
            #   new data:  CollectionTree
            #   old data:  EventInfo_p2
            #   old data:  POOLContainer_EventInfo_p1_McEventInfo
            #

            try:
                treeName  = "CollectionTree"
                treeKey = f.FindKey( treeName )
                # print "found tree ", treeKey
                if treeKey != None:
                    # Found tree, print out run number
                    eitree = treeKey.ReadObj()
                    print "read ei from tree:", eitree.GetName(), 'size:', eitree.GetEntries()
                    eibr = None
                    # Get EventInfo branch
                    eiP3 = False
                    isPileupEI = False
                    for br in eitree.GetListOfBranches():
                        # print br.GetName()
                        if re.match(r'EventInfo_p3_.*?', br.GetName()):
                            eibr = br
                            eiP3 = True
                            break
                        elif re.match(r'PileUpEventInfo_p4.*?', br.GetName()):
                            eibr = br
                            eiP3 = True
                            break
                        elif re.match(r'EventInfo_p2_.*?', br.GetName()):
                            eibr = br
                            break
                        elif re.match(r'PileUpEventInfo_p5_.*?', br.GetName()):
                            eibr = br
                            isPileupEI = True
                            # print "match PileUpEventInfo_p5"
                            break
                        pass
                    # read events
                    for i in range (i1, i2):
                        # print 'event',i
                        if eibr.GetEntry(i) <= 0:
                            print "Could not read event", i, "May be end of file"
                            break
                            pass
                        #  access event info
                        ei = getattr( eitree, eibr.GetName() )
                        #  print run-event
                        if eiP3:
                            # p3 version of EventInfo
                            data = ei.m_AllTheData
                            # print "len data", len(data)
                            ioffset = 1
                            if doDetailedDump:
                                nskip = data[ioffset + 8] >>16
                                nsk = (nskip + 2)/4 + 1
                                # print nskip, nsk
                                nskip1 = data[ioffset + 8 + nsk] >>16
                                nsk1 = (nskip1 + 16)/32 + 1
                                nsk = nsk + nsk1
                                # print nskip1, nsk1, nsk
                                print "run, lb, evt, time1, time2, mcChan, mcEvt, mcWt: ", data[ioffset], data[ioffset + 4], data[ioffset + 1], data[ioffset + 2], data[ioffset + 3], ctime(data[ioffset + 2]), data[ioffset + 9 + nsk], data[ioffset + 10 + nsk], data[ioffset + 8 + nsk]

                            else:
                                print "run number: ", data[ioffset]
                                pass
                        elif isPileupEI:
                            if doDetailedDump:
                                # print "Found pileUpEventInfo"
                                eid    = ei.m_event_ID
                                etyp   = ei.m_event_type
                                eflags = ei.m_event_flags
                                # print "eid, etyp, eflags ", eid, etyp, eflags
                                # print "flags ", len(eflags)
                                if len(eflags) == 0:
                                    print "run, lb, evt, mcChan, mcEvt, act/ave int: ", i, eid.m_run_number, eid.m_lumiBlock, eid.m_event_number, etyp.m_mc_channel_number, etyp.m_mc_event_number
                                else:
                                    # print "flags ", eflags[9],hex(eflags[9])
                                    interactions = eflags[9]
                                    actInt = float(interactions & 0xFFFF)
                                    actInt /= 100.
                                    aveInt = float((interactions & 0xFFFF000) >> 16);
                                    aveInt /= 100.
                                    print "run, lb, evt, mcChan, mcEvt, act/ave int: ", i, eid.m_run_number, eid.m_lumiBlock, eid.m_event_number, etyp.m_mc_channel_number, etyp.m_mc_event_number, actInt, aveInt
                                    pass
                            else:
                                # print "Found pileUpEventInfo1"
                                eid    = ei.m_event_ID
                                print "run number: ", eid.m_run_number
                        else:
                            eid  = ei.m_event_ID
                            etyp = ei.m_event_type;
                            if doDetailedDump:
                                print "run, lb, evt, time1, time2, mcChan, mcEvt, mcWt: ", eid.m_run_number, eid.m_lumiBlock, eid.m_event_number, eid.m_time_stamp, eid.m_time_stamp_ns_offset, ctime(eid.m_time_stamp), etyp.m_mc_event_weights, etyp.m_mc_channel_number, etyp.m_mc_event_number
                            else:
                                print "run number: ", eid.m_run_number
                    # must close file to avoid segfault
                    f.Close()
                    continue
            except Exception, e:
                print "Found CollectionTree but not EventInfo_p2_*. Try CollectionTreeEventInfo_p2" 
                # Some files have BOTH CollectionTree AND
                # CollectionTreeEventInfo_p2, we get here if
                # EventInfo_p2_McEventInfo is not found above
                treeName  = "CollectionTreeEventInfo_p2"
                treeKey = f.FindKey( treeName )
                #print "found ei ", treeKey
                if treeKey != None:
                    # Found tree, print out run number
                    eitree = treeKey.ReadObj()
                    print "read ei from tree:", eitree.GetName()
                    # Get EventInfo branch
                    eibr  = eitree.GetBranch( "EventInfo_p2" )
                    # read first event
                    eibr.GetEntry(0)
                    #  access event info
                    ei = eitree.EventInfo_p2
                    # print run-event
                    eid = ei.m_event_ID
                    print "run number: ", eid.m_run_number
                    continue

            treeName  = "POOLContainer_EventInfo_p1_McEventInfo"
            treeKey = f.FindKey( treeName )
            #print "found ei ", treeKey
            if treeKey != None:
                # Found tree, print out run number
                eitree = treeKey.ReadObj()
                print "read ei from tree:", eitree.GetName()
                # Get EventInfo branch
                eibr  = eitree.GetBranch( "EventInfo_p1" )
                # read first event
                eibr.GetEntry(0)
                #  access event info
                ei = eitree.EventInfo_p1
                # print run-event
                eid = ei.m_event_ID
                print "run number: ", eid.m_run_number
                continue

        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            traceback.print_exc(file=sys.stdout)
            sc = 1
            pass

    print "## Bye."
    sys.exit(sc)



