#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file:    tabnanny-checker.py
# @purpose: simple command-line utility wrapping tabnanny
# @author:  Sebastien Binet <binet@cern.ch>
# @date:    July 2009
#
# @example:
# @code
# tabnanny-checker
# tabnanny-checker AtlasCore
# @endcode
#
from __future__ import with_statement

__version__ = "$Revision$"
__author__  = "Sebastien Binet <binet@cern.ch>"

import sys
import os
import tabnanny
import StringIO

from optparse import OptionParser

import PyCmt.Logging as _L
import PyCmt.Cmt as PyCmt

def process_path(p):
    stdout = sys.stdout
    output = ''
    try:
        sys.stdout = StringIO.StringIO()
        tabnanny.check(p)
        output = sys.stdout.getvalue()
    finally:
        sys.stdout = stdout
    return output

if __name__ == "__main__":

    parser = OptionParser(usage="usage: %prog [options] [--project] AtlasCore")
    parser.add_option("--project",
                      dest = "project_name",
                      default=None,
                      help = "The name of the project to inspect" )

    sc = 0
    report = []
    
    (options, args) = parser.parse_args()
    msg = _L.logging.getLogger("TabNanny")
    msg.setLevel(_L.logging.INFO)
    msg.info('TabNanny checker')
    
    project_names = None
    if len(args) > 0:
        project_names = [ arg for arg in args if arg[0] != "-" ]
        pass

    cmt = PyCmt.CmtWrapper()
    if options.project_name is None:
        if project_names is None:
            # take the top-root project name
            options.project_name = cmt.projects_dag()[0].name
        else:
            # FIXME
            # take the first one... 
            options.project_name = str(project_names[0])

    # also allows to give a filename: early exit...
    import os.path as osp
    if osp.exists(options.project_name):
        path = options.project_name
        msg.info(':'*80)
        msg.info('checking [%s]...', path)
        output = process_path(path)
        if len(output) > 0:
            lines = output.splitlines()
            for l in lines:
                print l
                report.append(l)
            sc = 1
        else:
            msg.info('all good.')

    else:
        # retrieve the list of project names on which the
        # selected project depends
        projects = cmt.project_deps(options.project_name)

        for project_name in projects:
            project = cmt.projects_tree()[project_name]
            msg.info(':'*80)
            msg.info('checking [%s]...', project.name)
            output = process_path(project.path)
            if len(output) > 0:
                lines = output.splitlines()
                report.append("::: [%s] ::: (%i pbs)" % (
                    project_name, len(lines)))
                for l in lines:
                    print l
                    report.append(l)
                sc = 1
            else:
                msg.info('all good.')
        pass
    
    with open('tabnanny.report.txt', 'w') as out:
        print >> out, ':'*80
        print >> out, "::: TabNanny report"
        for l in report:
            print >> out, l
        print >> out, ':'*80
        pass
    
    msg.info("Bye.")
    sys.exit(sc)
