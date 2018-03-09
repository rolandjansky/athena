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
__version__ = "$Id: dumpVersionTags.py,v 1.5 2009-04-29 07:29:04 schaffer Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"

import user
import sys
import os, re
# Turn off X11 requirement in ROOT
sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
import ROOT
# Turn off X11 requirement in ROOT
sys.argv.remove('-b')
from PyCool import coral
from time import ctime
import cppyy
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


# Add in iterators for classes
def iter(self) :
  sequential = self.begin()
  end        = self.end()
  while sequential != end :
    yield sequential.__deref__()
    sequential.__preinc__()
  raise StopIteration

def set_iter(self) :
  sequential = self.begin()
  end        = self.end()
  while sequential != end :
    yield sequential.__deref__()
    #sequential.__preinc__()
  raise StopIteration

def attr_iter(self) :
  sequential = self.begin()
  end        = self.end()
  while sequential != end :
    yield sequential.__deref__().first,sequential.__deref__().second
    sequential.__preinc__()
  raise StopIteration

def iov_iter(self) :
  sequential = self.iov_begin()
  end        = self.iov_end()
  while sequential != end :
    yield sequential.__deref__().first,sequential.__deref__().second
    sequential.__preinc__()
  raise StopIteration

def name_iter(self) :
  sequential = self.name_begin()
  end        = self.name_end()
  while sequential != end :
    yield sequential.__deref__().first,sequential.__deref__().second
    sequential.__preinc__()
  raise StopIteration

def iovPrint(self):
    start = self.start()
    stop  = self.stop()
    iovStart = " "
    iovStop  = " "
    # Do not print out if IOVRange is empty - both time stamps == 0
    if not start.isValid(): return "Invalid"
    if start.isRunEvent():
        iovStart = "since [r,l]: [" + str(start.run()) + ',' + str(start.event()) + '] '
        if stop.run() == 2147483647 and stop.event() == 4294967295:
            iovStop  = "until MAX " # [r,l]: [" + str(stop.run())  + ',' + str(stop.event())  + ']'
        else:
            if stop.event() == 4294967295:
                iovStop  = "until [r,l]: [" + str(stop.run())  + ',MAX]'
            else:
                iovStop  = "until [r,l]: [" + str(stop.run())  + ',' + str(stop.event())  + ']'
    else:
        try:
            if start.timestamp() == 0:
                iovStart = "since MIN "
            else:
                iovStart = "since " + str(ctime(start.timestamp() / 1000000000)) + ' '
            if stop.timestamp() == 9223372036854775807:
                iovStop  = "until MAX (" + str(stop.timestamp()) + ")"
            else:
                iovStop  = "until " + str(ctime(stop.timestamp()  / 1000000000))
        except Exception, e:
            print "## Caught1 exception [%s] !!" % str(e.__class__)
            print "## What:",e
            import sys
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            return "Invalid" 
    output =  iovStart + iovStop
    return output


if __name__ == "__main__":


    t = cppyy.makeClass("IOVPayloadContainer")
    # Add on iterators for the different containers
    t.iter  = iter
    #print "t type ",t
    t = cppyy.makeClass("CondAttrListCollection")
    # Add on iterators for the different containers
    t.attr_iter  = attr_iter
    t.iov_iter   = iov_iter
    t.name_iter  = name_iter
    #print "t type ",t
    IOVRange = cppyy.makeClass('IOVRange')
    IOVRange.iovPrint = iovPrint

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

    fileNames = set( fileNames )
    sc = 0
    for fileName in fileNames:
        try:
            print "open file", fileName
            f = ROOT.TFile.Open (fileName)
            assert f.IsOpen()

            # The version tags may either be in the file meta data or
            # in EventInfo (old way). We check first in file meta
            # data.

            foundTagInfo = False
            tree = f.Get ('MetaDataHdrDataHeader')
            if isinstance(tree, ROOT.TTree): 
                # print "::: entries:",tree.GetEntries()
                branches = set([ br.GetName() for br in tree.GetListOfBranches()
                                 if re.match(r'DataHeader_p.*?', br.GetName()) ])
                if len(branches) <= 0:
                    raise RuntimeError("Could not find a persistent DataHeader in MetaDataHdrDataHeader tree")
                if len(branches) > 1:
                    print "*** Warning *** Too many DataHeader_pX:",branches
                # get the DataHeader_pX name
                dh_name = branches.pop()
                # print "::: found persistent DataHeader:",dh_name

                if dh_name == 'DataHeader_p1':
                    err= "*** sorry, DataHeader_p1 is NOT supported by `extract_items` ***"
                    print err
                    raise RuntimeError(err)
                

                # Fill this in if you want to change the names of the transient branches.
                branchNames = {}
                tt = AthenaROOTAccess.transientTree.makeTree(f, persTreeName='MetaData',
                                                             dhTreeName='MetaDataHdrDataHeader',
                                                             dhBranchName=dh_name, 
                                                             branchNames = branchNames)

                # Now check for IOVMetaDataContainer for the /TagInfo folder
                brs = tt.GetListOfBranches()
                for br in brs:
                    print "Type:",br.GetObjClassName(), "key:", br.GetName()
                    if br.GetObjClassName() == 'IOVMetaDataContainer' and br.GetName() == '_TagInfo':
                        foundTagInfo = True
                        try:
                            if br.GetEntry( 0 ) <= 0:
                                print ":: Could not get meta data for first event for branch", anem
                                pass
                            # print "Dumping class", br.GetObjClassName(), " for key ", name
                            #  access meta data
                            metaData = getattr(tt, br.GetName())
                            # print "folder name:", metaData.folderName()
                            # print "folder description:", metaData.folderDescription()
                            payLoad = metaData.payloadContainer()
                            # print "folder payLoads (size): (",payLoad.size(),")"
                            for attrListColl in payLoad.iter():
                                # iov  = attrListColl.minRange()
                                # output = iov.iovPrint() 
                                # if output != "Invalid":
                                #     print "Overall payload IOV", output
                                # else:
                                #     print "Overall payload IOV is INVALID!!"
                                # Loop over channels
                                for i in range(attrListColl.size()):
                                    chan = attrListColl.chanNum(i)
                                    # print "chan", chan, 
                                    chanName = attrListColl.chanName(chan)
                                    # if len(chanName) > 0: print "chan name", chanName, 
                                    iov  = attrListColl.iovRange(chan)
                                    output = iov.iovPrint() 
                                    print "----------------------------------------------"
                                    print "IOV Range:", output
                                    print " "
                                    print "tags: (tag name - value):"
                                    print "----------------------------------------------"
                                    print " "
                                    attrList = attrListColl.attributeList(chan)
                                    tagDict = {}
                                    # print "attribute list:", attrList,
                                    for field in attrList:
                                        # print '\tfield:', field, 'type:', attrList.typeName(field), 'value:', repr(attrList[field])
                                        tagDict[field] = repr(attrList[field])

                                    keys = tagDict.keys()
                                    keys.sort()
                                    for key in keys:
                                        print "%-50s  %s " % (key, tagDict[key])
                                    pass
                        except Exception, e:
                            print "## Caught2 exception [%s] !!" % str(e.__class__)
                            print "## What:",e
                            import sys
                            print sys.exc_info()[0]
                            print sys.exc_info()[1]
                            sc = 1
                            pass

                pass

            if not foundTagInfo:

                # Old data: Look for version in EventInfo
                # Fill this in if you want to change the names of the transient branches.
                tt = AthenaROOTAccess.transientTree.makeTree(f, dhfilt = selectEventInfo)

                eibr = None
                for br in tt.GetListOfBranches():
                    # print br
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

                # print "tags size:", len(tags)
                # for i in range(len(tags)):
                #     print i
                #     print i,tags[i]
                tagDict = {}
                if len(tags) > 1:
                    start = 0
                    if len(tags) % 2 == 1: start = 1
                    for i in range(start, len(tags), 2):
                        tagDict[tags[i]] = tags[i+1]
                    keys = tagDict.keys()
                    keys.sort()
                    for key in keys:
                        print "%-50s  %s " % (key, tagDict[key])
                else:
                    print "No version tags found - size:", len(tags)
            f.Close()
        except Exception, e:
            print "## Caught3 exception [%s] !!" % str(e.__class__)
            print "## What:",e
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass

        if len(fileNames) > 1:
            print ""
        pass # loop over fileNames
    
    print "## Bye."
    sys.exit(sc)


