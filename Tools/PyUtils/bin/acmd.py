#!/usr/bin/env python

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
    
    acmdlib.Plugins.loadAll()

    parser = acmdlib.ACMD_PARSER
    args = parser.parse_args()

    msg.info('running sub-command [%s]...', args.command)
    cmd_name = args.command

    sys_args = sys.argv[1:]
    if sys_args[0] != cmd_name:
        # special case of a sub(sub,...) command:
        # acmd a b c cmd arg1 arg2 ...
        # -> a.b.c.cmd
        idx = sys_args.index(cmd_name)
        cmd_name = '.'.join(sys_args[:idx+1])

    cmd = acmdlib.Plugins.load(cmd_name)
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
