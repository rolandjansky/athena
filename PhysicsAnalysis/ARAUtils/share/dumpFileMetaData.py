#!/usr/bin/env python

# @file:    dumpIOVFileMetaData.py
# @purpose: read an Event file and dump the IOV file meta data requested
# @author:  RD Schaffer <R.D.Schaffer@cern.ch>
# @date:    June 2008
#
# @example:
# @code
# dumpIOVFileMetaData.py aod.pool.root -k <folder key>
# dumpIOVFileMetaData.py -f aod.pool.root -k <folder key>
# @endcode
#
__version__ = "$Id: dumpFileMetaData.py,v 1.4 2009-04-29 07:29:04 schaffer Exp $"
__author__  = "RD Schaffer <R.D.Schaffer@cern.ch>"

import user
import sys, traceback
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

def dumpAll(tt, doDetailedDump):
    # Loop over branches and print them all out
    if doDetailedDump: print "Dumping all file meta data"
    else:
        print "List of meta data objects in file: (type : key )"
        print "  ==> To dump any single container: use '-k <key>'"
        print "  ==> To dump all: use '-d'"
    brs = tt.GetListOfBranches()
    for br in brs:
        print "Type:",br.GetObjClassName(), "key:", br.GetName()
        if br.GetObjClassName() == 'LumiBlockCollection':
            if doDetailedDump: doLumi(tt, br.GetName())
        elif br.GetObjClassName() == 'IOVMetaDataContainer':
            if doDetailedDump: dumpMeta(tt, br.GetName())
        elif br.GetObjClassName() == 'EventStreamInfo':
            if doDetailedDump: doEventStreamInfo(tt, br.GetName())
        else:
            print "Found file meta data branch of type:",br.GetObjClassName(), " Do not know how to print out information!! Please update the script!!"
        if doDetailedDump: print " "

def dumpMeta(tt, name):

    # Get meta data branch
    br  = tt.GetBranch( name )
    if br == None: print "Could not find branch for ",name; return
    try:
        if br.GetEntry( 0 ) <= 0:
            print ":: Could not get meta data for first event for branch", anem
            pass
        print "Dumping class", br.GetObjClassName(), " for key ", name
        #  access meta data
        metaData = getattr(tt, br.GetName())
        print "folder name:", metaData.folderName()
        print "folder description:", metaData.folderDescription()
        payLoad = metaData.payloadContainer()
        npl = 0
        print "folder payLoads (size): (",payLoad.size(),")"
        for attrListColl in payLoad.iter():
            print "payLoad", npl, ":"
            npl += 1
            #print attrListColl
            #attrListColl.dump()
            # Print out minRange
            iov  = attrListColl.minRange()
            output = iov.iovPrint() 
            if output != "Invalid":
                print "Overall payload IOV", output
            else:
                print "Overall payload IOV is INVALID!!"
            # Loop over channels
            for i in range(attrListColl.size()):
                chan = attrListColl.chanNum(i)
                print "chan", chan, 
                chanName = attrListColl.chanName(chan)
                if len(chanName) > 0: print "chan name", chanName, 
                iov  = attrListColl.iovRange(chan)
                output = iov.iovPrint() 
                if output != "Invalid":
                    print "IOV", output,
                attrList = attrListColl.attributeList(chan)
                print "attribute list:", attrList,
                print ' '
            
            #for chan, attr in attrListColl.attr_iter():
            #    print "chan, attr", chan, attr
            #for chan, attrName in attrListColl.name_iter():
            #    print "chan, attr name", chan, attrName
            #for chan, attrIOV in attrListColl.iov_iter():
            #    print "chan, attr iov", chan, attrIOV

    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__)
        print "## What:",e
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        sc = 1
        pass

def doLumi(tt, name):
    # Get meta data branch
    br  = tt.GetBranch( name )
    if br == None: print "Could not find branch for LumiBlocks "; pass
    try:
        if br.GetEntry( 0 ) <= 0:
            print ":: Could not get meta data for first event for branch LumiBlocks"
            pass
        #  access meta data
        print "Dumping class", br.GetObjClassName(), " for key ", name
        lbs = getattr(tt, name)
        print "Dumping lumiblocks", lbs.size()
        for l in lbs:
            print l.iovPrint()
            #print "start %x, stop %x" % (l.start().re_time(), l.stop().re_time())

    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__)
        print "## What:",e
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        sc = 1
        pass

def doEventStreamInfo(tt, name):
    # Get helper for dumping EventStreamInfo
    import AthenaPython.PyAthena as PyAthena
    esi1 = PyAthena.EventStreamInfo

    # Get EventStreamInfo
    br  = tt.GetBranch( name )
    if br == None: print "Could not find branch for ",name; return
    try:
        if br.GetEntry( 0 ) <= 0:
            print ":: Could not get meta data for first event for branch", anem
            pass
        print "Dumping class", br.GetObjClassName(), " for key ", name
        esi = getattr(tt, name)
        if esi == None:
            print "Could not get object for EventStreamInfo: ", name
            pass
        #  print out event stream info
        print "EventStreamInfo key:               ", name
        print "EventStreamInfo number of events:  ", esi.getNumberOfEvents()
        print "EventStreamInfo Run Numbers:       ",
        for run in esi.run_numbers():
            print run,
        print ' '
        print "EventStreamInfo LumiBlock Numbers: ",
        for lb in esi.lumi_blocks():
            print lb,
        print ' '
        print "EventStreamInfo Processing Tags:   ",
        for pt in esi.processing_tags():
            print pt,
        print ' '
        iet = 0
        for et in esi.evt_types():
            print "EventStreamInfo Event Types:       ",
            print "iet: %d, %s, mc chan: %d, user type: %s" % (iet, et.typeToString(), et.mc_channel_number(), et.user_type())
        print ' '
        print "EventStreamInfo Item list:         ",
        for il in esi.item_list():
            print il,
        print ' '

    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__)
        print "## What:",e
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        sc = 1
        pass



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
            print "## Caught exception [%s] !!" % str(e.__class__)
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
    t = cppyy.makeClass("std::set<unsigned int>")
    t.iter  = iter
    #print "t type ",t
    IOVRange = cppyy.makeClass('IOVRange')
    IOVRange.iovPrint = iovPrint

    # read in and treat options 
    parser = OptionParser(usage="usage: %prog [options] [-f] my.file.pool -k '<meta data key1> <meta data key2> ...'")
    parser.add_option( "-f",
                       "--file",
                       dest = "fileName",
                       help = "The path to the POOL file for dumping the run number" )
    parser.add_option( "-k",
                       "--key",
                       dest = "keys",
                       help = "'<meta data key> [<meta data key>'] ..., i.e. space-separated list of keys with CheckFile.py following 'IOVMetaDataContainer_p1_' " )
    parser.add_option( "-d",
                       "--detailed-dump",
                       action  = "store_true",
                       dest = "doDetailedDump",
                       default = False,
                       help = "Switch to activate the detailed dump of all keys" )

    (options, args) = parser.parse_args()

    #print "options, args", options, args

    
    fileNames = []
    key       = None
    doDetailedDump = options.doDetailedDump
    #print "doDetailedDump",doDetailedDump
    
    if len(args) > 0:
        fileNames = [ arg for arg in args if arg[0] != "-" ]
        pass

    if options.fileName == None and len(fileNames) == 0:
        str(parser.print_help() or "")
        sys.exit(1)

    if options.keys == None:
        print "No key given: dumping all file meta data"

    if options.fileName != None:
        fileName = os.path.expandvars(os.path.expanduser(options.fileName))
        fileNames.append(fileName)

    keys = None
    if options.keys != None:
        keys = os.path.expandvars(os.path.expanduser(options.keys)).split()
        #print "keys", keys

    fileNames = set( fileNames )

    #print "files:", fileNames
    
    sc = 0
    for fileName in fileNames:
        try:
            print "open file", fileName
            f = ROOT.TFile.Open (fileName)
            assert f.IsOpen()
            print "opened file", fileName

            isNewDataHdr = True
            tree = f.Get ('MetaDataHdr')
            # print "MetaDataHdr",tree
            if not isinstance(tree, ROOT.TTree):
                isNewDataHdr = False
                tree = f.Get ('MetaDataHdrDataHeader')
                # print "MetaDataHdrDataHeader",tree
                if not isinstance(tree, ROOT.TTree):
                    print "File does NOT contain meta data Tree"
                    keys = f.GetListOfKeys()
                    for k in keys:
                        name = k.GetName()
                        if not '#' in name: print "Found TTree:", name
                        pass
                    continue
                pass
            
            # print "::: entries:",tree.GetEntries()
            if isNewDataHdr:
                branches = set([ 'DataHeader' ])
            else:
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
            if isNewDataHdr:
                tt = AthenaROOTAccess.transientTree.makeTree(f, persTreeName='MetaData',
                                                             dhTreeName='MetaDataHdr',
                                                             dhBranchName=dh_name, 
                                                             branchNames = branchNames)
            else:
                tt = AthenaROOTAccess.transientTree.makeTree(f, persTreeName='MetaData',
                                                             dhTreeName='MetaDataHdrDataHeader',
                                                             dhBranchName=dh_name, 
                                                             branchNames = branchNames)
                pass
            
            if keys == None:
                # No keys, dump all
                try:
                    dumpAll(tt, doDetailedDump)
                    f.Close()
                    continue
                except Exception, e:
                    print "## Caught exception [%s] !!" % str(e.__class__)
                    print "## What:",e
                    import sys
                    print sys.exc_info()[0]
                    print sys.exc_info()[1]
                    sc = 1
                    pass
                f.Close()
                continue
            for key in keys:
                if key == 'LumiBlocks' or key == 'IncompleteLumiBlocks':
                    # special method for lumiblocks
                    try:
                        doLumi(tt, key)
                    except Exception, e:
                        print "## Caught exception [%s] !!" % str(e.__class__)
                        print "## What:",e
                        import sys
                        print sys.exc_info()[0]
                        print sys.exc_info()[1]
                        sc = 1
                        pass
                else:
                    # Test if we have an EventStreamInfo object
                    try:
                        esibr = tt.GetBranch(key)
                        if esibr.GetObjClassName() == "EventStreamInfo": doEventStreamInfo(tt, key)
                        else:
                            #print "Trying to dump file meta data for key:", key
                            dumpMeta(tt, key)
                    except Exception, e:
                        print "## Caught exception [%s] !!" % str(e.__class__)
                        print "## What:",e
                        import sys
                        print sys.exc_info()[0]
                        print sys.exc_info()[1]
                        sc = 1
                        pass
            f.Close()
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            import sys
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass
