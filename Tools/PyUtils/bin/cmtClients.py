#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @author: Sebastien Binet
## @file : cmtClients.py
## @purpose: a little wrapper around cmt.exe show clients

__version__ = "$Revision: 1.4 $"
__author__  = "Sebastien Binet"

import sys
from PyCmt import Cmt

if __name__ == "__main__":

    from optparse import OptionParser
    parser = OptionParser(usage="usage: %prog [options] [-p] PkgName")
    parser.add_option(
        "-p",
        "--pkg",
        dest = "pkgName",
        help = "The name of the package one wants to see the clients"
        )
    parser.add_option(
        "--co",
        action = "store_true",
        dest   = "doCheckOut",
        default= False,
        help = "Switch to check out the list of client of the given package"
        )
    parser.add_option(
        "-l",
        "--level",
        dest = "logLvl",
        default = "INFO",
        help = "Logging level (aka verbosity)"
        )

    (options, args) = parser.parse_args()

    if len(args) > 0 and args[0][0] != "-":
        options.pkgName = args[0]
        pass
    
    if options.pkgName == None:
        str(parser.print_help() or "")
        sys.exit(1)

    options.logLvl = options.logLvl.upper()
    import PyCmt.Logging as L
    if hasattr(L.logging, options.logLvl):
        lvl = getattr(L.logging, options.logLvl)
    else:
        print "*"*80
        print "BootStrap: Unknown logging level [%s] !!" % options.logLvl
        print "BootStrap: Defaulting to [INFO]..."
        print "*"*80
        lvl = L.logging.INFO
    
    cmt = Cmt.CmtWrapper( lvl )
    cmt.msg.info( "cmt show clients [%s]", options.pkgName )
    clients = cmt.showClients( options.pkgName )

    if options.doCheckOut:
        def do_checkout (pkg):
            cmt.checkOut (pkg.fullName(), pkg.version)
        try:
            import multiprocessing as mp
            nworkers = min (mp.cpu_count(), 4)
            workers = mp.Pool(processes=nworkers)
            res = workers.map_async (func=do_checkout, iterable=clients)
            res = res.get()
        except ImportError: # no multiprocessing module
            for client in clients:
                res = do_checkout(client)
        
    sys.exit(0)
    pass

