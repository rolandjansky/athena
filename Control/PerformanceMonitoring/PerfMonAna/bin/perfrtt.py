#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file: perfrtt.py
# @purpose: thin layer on top of @c perfmon.py for RTT harvesting
# @author: Sebastien Binet <binet@.cern.ch>
# $Id: perfrtt.py,v 1.1 2007-07-18 23:02:25 binet Exp $

__author__  = "Sebastien Binet"
__version__ = "$Revision: 1.1 $"
__doc__     = "thin layer on top of @c perfmon.py for RTT harvesting"

import os
import sys
import traceback
import glob

def importRoot( batch = True ):
    import sys
    _oldArgs = sys.argv
    if batch: sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
    import ROOT
    sys.argv = _oldArgs
    del _oldArgs
    return ROOT

class Rtt:
    Home = "/afs/cern.ch/atlas/project/RTT"
    Nightlies = [ 'bugfix', 'val', 'mig0', 'dev' ]
    Builds    = [ '10', '11', '12', '13' ]
    Platforms = [ 'slc3', 'slc4' ]
    
    class Release( object ):
        """A simple modeling of an Athena release (nightly,release,platform,...)
        """
        def __init__(self, id):
            object.__init__(self)

            self.name = None
            self.platform = None
            self._parseId(id)
            return

        def _parseId(self, id):
            id = [ i.lower() for i in id.split(',') ]
            print id
            name     = None
            relName  = []
            archName = []
            isStable = False
            for i in id:
                if 'rel' in i:
                    relName.append(i)
                elif i in Rtt.Nightlies:
                    relName.append(i)
                elif i.replace('.','').isdigit(): # 13.0.20 -> 13020'
                    isStable = True
                    name = i
                else:
                    archName.append(i)
            
            if len(relName) == 0:
                assert(isStable)
                self.name = name
            else:
                self.name = [ i for i in relName if i not in Rtt.Nightlies ]+\
                            [ i for i in relName if i     in Rtt.Nightlies ]

            
            if len(archName) == 0:
                archName.append('opt')
            self.platform = '-'.join(archName)

            print "name:",self.name
            print "arch:",self.platform
            return

        def isNightly(self):
            return True

        def isStable(self):
            return not self.isNightly()
        
        def _id(self):
            if self.isNightly(): return self.name
            else:                return [self.name]

        def root(self):
            return [ Rtt.Home, 'Results' ] + self._id()

        def cmtconfig(self):
            return "i686-slc4-gcc34-"+self.platform
        
        pass # class RttRelease

    pass # class Rtt

def rttPath( rel, pkgName = 'RecExRecoTest' ):
    root = os.sep.join(
        rel.root() + ['build', rel.cmtconfig(), 'offline', pkgName,'*']
        )
    rtt = [f for f in glob.glob(root) \
           if os.path.isdir(f) and os.path.basename(f) != "AthenaATN" ]
    assert(len(rtt)==1)
    return rtt[0]

def main():
    """main entry point"""
    sc = 0
    from optparse import OptionParser
    parser = OptionParser( usage = "usage: %prog [options]" )
##     parser.add_option(
##         "-f",
##         "--file",
##         dest = "chkFileName",
##         help = "path to the performance monitoring file to analyze"
##         )
##     parser.add_option(
##         "-r",
##         "--ref",
##         dest    = "refFileName",
##         default = "",
##         help = "path to the (reference) performance monitoring file (if any)"
##         )

##     parser.add_option(
##         "-o",
##         "--out",
##         dest    = "outFileName",
##         default = "",
##         help = "path to the output file which will contain analyzed performance monitoring data/infos"
##         )

    parser.add_option(
        "--no-batch",
        action  = "store_false",
        dest    = "rootBatch",
        default = False,
        help    = "Switch to tell ROOT to load graphics libraries"
        )

    parser.add_option(
        "-b",
        "--batch",
        action  = "store_true",
        dest    = "rootBatch",
        default = True,
        help    = "Switch to tell ROOT _NOT_ to load graphics libraries"
        )

    (options, args) = parser.parse_args()

##     if len(args) > 0 and args[0][0] != "-":
##         options.chkFileName = args[0]
##         pass

##     if len(args) > 1 and args[1][0] != "-":
##         options.refFileName = args[1]
##         pass

##     if len(args) > 2 and args[2][0] != "-":
##         options.outFileName = args[2]
##         pass

##     ROOT = importRoot( batch = options.rootBatch )
    
##     from PerfMonAna.PerfMonProcessing import ExitCodes
##     if options.chkFileName == None:
##         str(parser.print_help() or "ERROR: no help to print !!")
##         return ExitCodes.ERROR

    rel = Rtt.Release( "rel_2,opt,val" )
    rtt= rttPath(rel)
    print rtt
    print os.listdir(rtt)
##     chkFileName = os.path.expandvars(os.path.expanduser(options.chkFileName))
##     refFileName = os.path.expandvars(os.path.expanduser(options.refFileName))
##     outFileName = os.path.expandvars(os.path.expanduser(options.outFileName))

##     try:
##         from PerfMonAna import PerfMonProcessing as pm
##         ana = pm.AnaMgr( chkFileName, refFileName, outFileName )
##         sc = ana.run()
##     except Exception, err:
##         print "::: Caught:",err
##         traceback.print_exc( file = sys.stdout )
##         sc = ExitCodes.ERROR
##         pass
    
    return sc



if __name__ == "__main__":
    print ":"*80
    print "::: perfRTT analysis script :::"
    print ""
    
    sc = main()
    
    print ""
    print "::: bye"
    print ":"*80
    sys.exit( sc )
    
