#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# Usage: python RunProcessWithMonitor.py [args]
# Purpose: workaround to calling asetup via subprocess in athena

import sys
import subprocess
from select import select

TIMEOUT = 10
print sys.argv[1:]

while True:
    # start a new process to monitor
    child = subprocess.Popen(sys.argv[1:], stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)
    while True:
        try:
            rlist,_,_ = select([child.stdout], [], [], TIMEOUT)
            if rlist:
                print child.communicate()[0]
            else:
                # timeout occurred, did the process finish?
                if child.poll() is not None:
                    # child process completed (or was killed, but didn't hang), we are done
                    print 'Process complete'
                else:
                    # otherwise, kill the child
                    child.communicate(input='quit()')[0]
                    break
        except Exception as e:
            if str(e) != 'I/O operation on closed file':
                print type(e)
                print e
            # now we need to exit via os._exit rather than sys.exit, as sys.exit hangs if we are running in athena
            import os
            os._exit(0)
