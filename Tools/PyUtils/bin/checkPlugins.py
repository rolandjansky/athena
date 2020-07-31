#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @author: Sebastien Binet
## @file : checkPlugins.py
## @brief: A script to check the definitions of plugins across multiple
##           so-called 'rootmap' files

from __future__ import print_function

__author__  = "Sebastien Binet"

import os
import sys
from PyUtils import Dso

_suppression_dct = {
    'TMath' : ('libCore.so', 'libMathCore.so'),
    'string': ('libGaudiKernelDict.so',
               'libCore.so',
               'liblcg_PyCoolDict.so',
               'libSTLAddRflx.so'),
    'vector<vector<double> >': ('libMathCore.so',
                                'libAtlasSTLAddReflexDict.so'),
    'RooStats': ('libHistFactory.so',
                 'libRooStats.so'),
    'vector<unsigned int>': ('libSTLRflx.so',
                             'libSTLAddRflx.so'),
    'vector<double>': ('libSTLRflx.so',
                       'libSTLAddRflx.so'),
    }

def _currentProject():
    return os.environ.get('AtlasProject')


def printDb( db, detailedDump = False ):
    if detailedDump : fct = lambda x: x
    else:             fct = os.path.basename
    keys = list(db.keys())
    keys.sort()
    for k in keys:
        print ("%s:" % k)
        libs = db[k]
        libs.sort()
        for lib in libs:
            print ("  ",fct(lib))
    return

if __name__ == "__main__":

    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options]")
    parser.add_option(
        "--capabilities",
        dest = "capabilities",
        default = None,
        help = "Dump the capabilities of a given library (ex: libAthenaServices.so)"
        )
    parser.add_option(
        "--dups",
        dest = "checkDups",
        default = None,
        help = "Check if there is any duplicates among dictionaries for a given library"
        )
    parser.add_option(
        "--dump-content",
        dest = "dumpContent",
        action = "store_true",
        default = False,
        help = "Dump the content of all the known plugins (dicts. and components)"
        )
    parser.add_option(
        "--dso",
        dest = "dumpDso",
        action = "store_true",
        default = False,
        help = "Dump all the dsomap/rootmap files known to the Dso repository"
        )
    parser.add_option(
        "--libs",
        dest = "dumpLibs",
        action = "store_true",
        default = False,
        help = "Dump all the libraries known to the Dso repository"
        )
    parser.add_option(
        "--check-dict-dups",
        action = "store_true",
        default = False,
        dest = "checkDictDuplicates",
        help = "Check if there is any duplicates among dictionaries"
        )
    parser.add_option(
        "--check-pf-dups",
        action = "store_true",
        default = False,
        dest = "checkPfDuplicates",
        help = "Check if there is any duplicates among components declared to the PluginSvc"
        )
    parser.add_option(
        "--check-all-dups",
        dest = "checkAllDuplicates",
        action = "store_true",
        default = False,
        help = "Check dictionaries *and* components"
        )
    parser.add_option(
        "--detailed-dump",
        action = "store_true",
        dest = "detailedDump",
        default = False,
        help = "Performs a detailed dump if duplicates are found"
        )
    parser.add_option(
        "--pedantic",
        action = "store_true",
        dest = "isPedantic",
        default = False,
        help = "Pedantic mode: if a component is found in 2 libraries which have the same name (usual case of a developer working on a (set of) package(s)), it is still being reported as being duplicated"
        )
    parser.add_option(
        "-l",
        "--level",
        dest = "logLvl",
        default = "INFO",
        help = "Logging level (aka verbosity)"
        )

    (options, args) = parser.parse_args()

    print (":"*80)
    print ("::: checkPlugins :::")
    sc = 0
    dsoDb = Dso.DsoDb()

    if len(args) > 0 and args[0][0] != "-":
        options.capabilities = args[0]
        pass
    
    if options.capabilities:
        libName = options.capabilities
        try:
            capabilities = dsoDb.capabilities(libName)
            print ("::: capabilities of [%s]" % libName)
            print (os.linesep.join( [ "  "+str(c) for c in capabilities ] ))
        except ValueError:
            sc = 1
            pass

    if options.checkDups:
        libName = options.checkDups
        try:
            print ("::: checking duplicates for [%s]..." % libName)
            dups = dsoDb.duplicates(libName, pedantic = options.isPedantic)
            for k in dups:
                print (" -",k)
                print (os.linesep.join( [ "  "+str(v) for v in dups[k] ] ))
            if len(dups.keys())>0: sc = 1
        except ValueError:
            sc = 1
            pass
        
    if options.dumpContent:
        print ("::: dumping content of all known plugins...")
        entries = dsoDb.content( pedantic = options.isPedantic )
        printDb(entries, options.detailedDump)
        print ("::: known entries:",len(entries.keys()))
        
    if options.dumpLibs:
        print ("::: dumping all known libraries...")
        libs = dsoDb.libs(options.detailedDump)
        for lib in libs:
            print (" -",lib)
        print ("::: known libs:",len(libs))
        
    if options.dumpDso:
        print ("::: dumping all known dsomap/rootmap files...")
        dsoFiles = [ dso for dso in dsoDb.dsoFiles]
        dsoFiles.sort()
        for dsoFile in dsoFiles:
            if not options.detailedDump: dsoFile = os.path.basename(dsoFile)
            print (" -",dsoFile)
        print ("::: known dsos:",len(dsoFiles))
        
    if options.checkDictDuplicates or options.checkAllDuplicates:
        print (":: checking dict. duplicates...")
        dups = dsoDb.dictDuplicates( pedantic = options.isPedantic )
        # restrict to just this project
        #currProj = _currentProject()
        #restrictedDups = {}
        #for label, libPaths in dups.items():
        #    paths = [l for l in libPaths if ('/%s/' % currProj) in l]
        #    if paths:
        #        restrictedDups[label] = paths
        #dups = restrictedDups

        sc = 0
        suppression_log = []
        for k in dups:
            v = dups[k]

            # mark as error only if it isn't a know dup'
            if k in _suppression_dct:
                suppressed = [os.path.basename(ii) in _suppression_dct[k]
                              for ii in v]
                if all(suppressed):
                    msg = "---> ignoring [%s]" % k
                    suppression_log.append(k[:])
                    #print (msg)
                    pass
                else:
                    # that's a new one !
                    sc = 1
            else:
                # that's a new one !
                sc = 1
                #print ("---> NOT ignoring [%s]" % k)
        printDb(dups, options.detailedDump)
        if len(suppression_log):
            print ("-"*40)
            print ("## ignoring the following dups':")
            for k in suppression_log:
                print (" -",k)
            print ("-"*40)
        print ("## all dups:",len(dups.keys()))
        print ("##     dups:",len(dups.keys())-len(suppression_log))
    if options.checkPfDuplicates or options.checkAllDuplicates:
        print (":: checking (plugin factories) components duplicates...")
        dups = dsoDb.pfDuplicates( pedantic = options.isPedantic )
        if len(dups.keys()) > 0: sc = 1
        printDb(dups, options.detailedDump)
        print ("## dups:",len(dups.keys()))

    if sc != 0: print (":: ERROR !!")
    else:       print (":: All good.")

    print (":"*80)
    sys.exit(sc)
