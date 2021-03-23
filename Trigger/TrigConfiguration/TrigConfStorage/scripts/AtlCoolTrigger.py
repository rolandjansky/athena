#!/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from optparse import OptionParser
from TrigConfStorage.AtlCoolTriggerTool import AtlCoolTriggerTool

if __name__ == '__main__':

    p = OptionParser(usage="usage: %prog options", version="1.1",
                     epilog="this program uses the cool database as source of information")
    
    p.add_option( "-r",
                  "--run",
                  default = "-",
                  dest = "runlist",
                  metavar='RUNS',
                  help = "A list of run ranges e.g. '1-4,6,17-23,89-'" )
    
    p.add_option( "-c",
                  "--dbconn",
                  default = "COMP",
                  dest = "db",
                  help = "The cool db [default: COMP]" )

    p.add_option( "-t",
                  "--time",
                  action="store_true",
                  default=False,
                  dest = "time",
                  help = "Print the start time of the run" )
    
    p.add_option( "-m",
                  "--menu",
                  action='store_true',
                  dest = "menu",
                  help = "Print menu" )
    
    p.add_option( "--l1",
                  action='store_true',
                  default=False,
                  dest = "l1",
                  help = "Only for l1 (in conjunction with -m, -x, or -d)" )

    p.add_option( "--l2",
                  action='store_true',
                  default=False,
                  dest = "l2",
                  help = "Only for l2 (in conjunction with -m, -x, or -d)" )

    p.add_option( "--ef",
                  action='store_true',
                  default=False,
                  dest = "ef",
                  help = "Only for ef (in conjunction with -m, -x, or -d)" )

    p.add_option( "-s",
                  "--streams",
                  action = "store_true",
                  default=False,
                  dest = "streams",
                  help = "Print the streams in the menu" )
    
    p.add_option( "-v",
                  "--verbose",
                  action='count',
                  default=0,
                  dest = "verbosity",
                  help = "Set verbosity [-v,-vv,...]" )
    
    p.add_option( "-p",
                  "--processing",
                  default="",
                  dest = "processing",
                  help = "Returns psk string for processing [possible 'hlt']" )
    
    
    (opt, args) = p.parse_args()

    AtlCoolTrigger = AtlCoolTriggerTool(options=opt)

    AtlCoolTrigger.execute()
