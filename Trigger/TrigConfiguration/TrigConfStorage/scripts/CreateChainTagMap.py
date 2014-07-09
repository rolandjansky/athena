#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
from TrigConfStorage.ChainTagMapTool import ChainTagMapTool

if __name__ == '__main__':

    p = OptionParser(usage="usage: %prog options", version="1.1",
                     epilog="this program uses the cool database to provide xml file(s) that map between chain names and counters for usage by the tag db")
    
    p.add_option( "-r",
                  "--run",
                  dest = "run",
                  metavar='RUN',
                  help = "the last run" )
    
    p.add_option( "-s",
                  "--split",
                  action = "store_true",
                  dest = "split",
                  metavar='SPLIT',
                  default = False,
                  help = "the last run" )
    
    p.add_option( "-d",
                  "--dir",
                  default = ".",
                  dest = "dir",
                  help = "The path of the map files" )

    p.add_option( "-v",
                  "--verbose",
                  action='count',
                  default=0,
                  dest = "verbosity",
                  help = "Set verbosity [-v,-vv,...]" )
    
    
    (opt, args) = p.parse_args()

    ChainTagMap = ChainTagMapTool(options=opt)

    ChainTagMap.buildMap()
