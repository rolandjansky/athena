#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file PyUtils.acmd
# @purpose main command line script for the general purpose athena scripts
# @author Sebastien Binet
# @date January 2010

from __future__ import with_statement

__version__ = "$Revision: 276499 $"
__author__ = "Sebastien Binet"
__doc__ = "main command line script for the general purpose athena scripts"

import PyUtils.acmdlib as acmdlib

def main():
    import PyUtils.scripts
    import PyUtils.Logging as L
    msg = L.logging.getLogger('Acmd')
    msg.setLevel(L.logging.INFO)
    
    ## if 0:
    ##     acmdlib.register_file('acmd_plugins.cfg')
    ## else:
    ##     import os
    ##     if os.path.exists('acmd_plugins.py'):
    ##         execfile('acmd_plugins.py')
            
    commands = {}
    plugins = list(acmdlib.ext_plugins.get(group=acmdlib.ACMD_GROUPNAME))
    #print plugins
    for i, plugin in enumerate(plugins):
        #print i, plugin.name
        commands[plugin.name] = plugin

    if 1:
        acmdlib._load_commands()

    parser = acmdlib.ACMD_PARSER
    args = parser.parse_args()

    msg.info('running sub-command [%s]...', args.command)
    cmd_name = args.command

    import sys
    sys_args = sys.argv[1:]
    if sys_args[0] != cmd_name:
        # special case of a sub(sub,...) command:
        # acmd a b c cmd arg1 arg2 ...
        # -> a.b.c.cmd
        idx = sys_args.index(cmd_name)
        cmd_name = '.'.join(sys_args[:idx+1])

    cmd = commands[cmd_name].load()
    exitcode = 1
    try:
        exitcode = cmd(args)
    except Exception:
        exitcode = 1
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        raise
    
    return exitcode


if __name__ == "__main__":
    import sys
    sys.exit(main())
    
