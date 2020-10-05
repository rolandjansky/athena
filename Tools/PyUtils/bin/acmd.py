#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# @file PyUtils.acmd
# @purpose main command line script for the general purpose athena scripts
# @author Sebastien Binet
# @date January 2010

__author__ = "Sebastien Binet"
__doc__ = "main command line script for the general purpose athena scripts"

import PyUtils.acmdlib as acmdlib
import sys

def main():
    import PyUtils.scripts   # noqa: F401  (register all plugins)
    import PyUtils.Logging as L
    msg = L.logging.getLogger('Acmd')
    msg.setLevel(L.logging.INFO)
    
    # To avoid loading all plugins every time, first try to find
    # a specific plugin that matches the first N arguments:
    cmd = None
    for i in range(1,len(sys.argv)):
        cmd_name = '.'.join(sys.argv[1:i+1])
        if acmdlib.Plugins.exists(cmd_name):
            cmd = acmdlib.Plugins.load(cmd_name)
            break

    # Otherwise load all plugins and leave the rest to the parser:
    if cmd is None:
        acmdlib.Plugins.loadAll()

    parser = acmdlib.ACMD_PARSER
    args = parser.parse_args()

    exitcode = 1
    try:
        exitcode = cmd.main(args)
    except Exception:
        exitcode = 1
        print(sys.exc_info()[0])
        print(sys.exc_info()[1])
        raise
    
    return exitcode


if __name__ == "__main__":
    sys.exit(main())
