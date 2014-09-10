#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file: AtlasPolicy/cmt/fragments/gencliddb.py
# @purpose: little wrapper around the genCLIDDB.exe application to make sure
#           lockfiles are correctly deleted at the end of the day
# @author: Sebastien Binet <binet@cern.ch>

import commands
import fcntl
import logging
import os
import sys
import time

class ExitCodes:
    InputError = 1
    GaudiError = 2
    CopyError =  3

## Lock a file.
#  The file for the lock is created if it doesn't exists and it the "temporary"
#  argument is set to True it will also be deleted when the lock is not needed.
#  The unlocking is done in the destructor (RAII pattern - basically this is
# a C++11 lock guard in python).
class LockFile(object):
    def __init__(self, name, temporary = False):
        self.name = name
        self.temporary = temporary
        self.file = None
        self.log = logging.getLogger("LockFile")
        self.log.info("%s - Locking on %s", time.strftime("%Y-%m-%d_%H:%M:%S"), self.name)
        if not os.path.exists(name):
            mode = "w"
        else:
            self.temporary = False # I do not want to delete a file I didn't create
            mode = "r+"
        try:
            self.file = open(self.name, mode)
            self.lock()
        except:
            self.log.warning("Cannot acquire lock on %s", self.name)

    def lock(self):
        # Lock with a simple call to lockf() - this blocks until the lock is aquired
        try:
            fcntl.lockf( self.file, fcntl.LOCK_EX )
        except IOError, exc_value:
            print "Problem when trying to lock {0}, IOError {1}".format(self.file, exc_value[0])
            raise
        return
        
    def unlock(self):
        fcntl.lockf( self.file, fcntl.LOCK_UN )
        return
            
    def __del__(self):
        if self.file:
            self.unlock()
            self.file.close()
            if self.temporary:
                try:
                    os.remove(self.name)
                except:
                    pass
            self.log.info("%s - Lock on %s released", time.strftime("%Y-%m-%d_%H:%M:%S"), self.name)
            self.file = None # Don't unlock twice!


    # The following methods are needed to allow the use of python's "with" statement, i.e,
    #     with LockFile("myFile") as mylock:
    def __enter__(self):
        return self
            
    def __exit__(self, type, value, traceback):
        self.__del__()


    
def main():
    """main entry point"""

    from optparse import OptionParser
    parser = OptionParser( usage = "usage: %prog [options]" )
    p = parser.add_option
    p( "-p",
       "--package",
       dest    = "package",
       default = None,
       help    = "package we want to load clids from"
       )

    p( "-i",
       "--input",
       dest    = "input",
       default = None,
       help    = "optional path to input clid db file"
       )

    p( "-o",
       "--output",
       dest    = "output",
       default = None,
       help = "optional path to resulting clid db file"
       )

    p( "-j", "--jobopts",
       dest    = "jobopts",
       default = None,
       help    = "name of optional job options txt file, " \
                 "located at ../share/jobopts"
       )
    
    (options, args) = parser.parse_args()

    if options.package is None:
        print "::: Please specify a package using option --package !"
        parser.print_help()
        sys.exit( ExitCodes.InputError )

    if options.output is None:
        options.output = options.package + "_clid.db"
        
    sc,gencliddb = commands.getstatusoutput( "which genCLIDDB.exe" )
    if sc != 0:
        print "::: Could not find 'genCLIDDB' executable :"
        print gencliddb
        sys.exit(sc)
        pass

    print "------> %s [%s]" % ( os.path.basename(gencliddb), options.package )

    ## build args for the C++ application
    args = " ".join(sys.argv[1:])

    sc = 4 # Ensure that sc is set
    ## require sequential access to clid.db file. LockFile
    #  uses RAII and releases lock in its destructor
    with LockFile(options.input) as mylock:
        try:
            ## Execute C++ app...
            sc,out = commands.getstatusoutput( gencliddb + " " + args )

        except Exception, err:
            print "::: Caught:",err
            import traceback
            traceback.print_exc( file = sys.stdout )
            sc = ExitCodes.GaudiError
            pass

        except:
            print "::: Caught an unknown exception"
            import traceback
            traceback.print_exc( file = sys.stdout )
            sc = ExitCodes.GaudiError
            pass

        ## copy options.output into options.input...
        try:
            import shutil
            shutil.copy(options.output, options.input)
        except Exception, err:
            print "::: Caught:",err
            import traceback
            traceback.print_exc( file = sys.stdout )
            sc = ExitCodes.CopyError
            pass

    if sc != 0:
        print "::: ERROR during '%s' execution... (exit code=%s)" % (
            os.path.basename(gencliddb), sc )
        print out
    return sc



if __name__ == "__main__":
    sys.exit( main() )
    
