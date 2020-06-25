#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file:    print_auditor_callgraph.py
# @purpose: print the callgraph sequence of a job stage (ini/exe/fin), parsing
#           the logfile of a job run with NameAuditor enabled.
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    May 2008
#
# @example:
# @code
# print_auditor_callgraph recexcommon.log
# @endcode
#

from __future__ import print_function

__author__  = "Sebastien Binet <binet@cern.ch>"

import os
import re
import sys

class Steps:
    ini = 'Initialization'
    exe = 'Execute'
    fin = 'Finalization'
    ALLOWED = ('ini', 'exe', 'fin')
    
def parse_log_file(fname, step=Steps.ini):
    beg_pat = re.compile(r"NameAuditor.*?About to Enter "
                         r"(?P<CompName>.*?) "
                         r"%s Method"%step)
    end_pat = re.compile(r"NameAuditor.*?Just Exited "
                         r"(?P<CompName>.*?) "
                         r"%s Method"%step)

    stack = 0
    graph = []

    for l in open(fname, 'r'):
        l = l.strip()
        beg = re.match(beg_pat, l)
        end = re.match(end_pat, l)
        if not (beg or end):
            continue

        if beg:
            component = beg.group('CompName')
            #print ("  "*stack,component)
            stack += 1
            graph += [ (stack, component) ]

        if end:
            component = end.group('CompName')
            #print ("  "*stack,component)
            stack -= 1

    return graph


if __name__ == '__main__':
    if len(sys.argv) < 2:
        raise SystemExit(1, "You have to provide a path to a logfile to parse")
    
    fname = os.path.expandvars(os.path.expanduser(sys.argv[1]))
    if not os.path.exists(fname):
        raise SystemExit(
            1, "You have to provide a VALID path to a logfile to parse")
    
    step = Steps.ini
    if len(sys.argv) > 2:
        step = sys.argv[2].lower()
        if step not in Steps.ALLOWED:
            raise SystemExit(
                2, "Invalid step name [%s] allowed=%r"%(step, Steps.ALLOWED))

        step = getattr(Steps,step)
        
    gr = parse_log_file(fname,step)
    #print (gr)
    for i in gr:
        print (".."*(i[0]+1),i[1])
    
