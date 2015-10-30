#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @file:    trig_tcSubmitValPkg.py
# @purpose: Copies the validation packages from the current to a target release
# @author:  Joerg Stelzer
#
# $Id:$

import sys
import os
import argparse
import subprocess
import shlex
from PyCmt import Cmt

cmt = Cmt.CmtWrapper()

def getValidationPkgs():
    return [
        "TrigP1Test",
        "TriggerTest",
        "TrigInDetValidation",
        "TrigValTools",
        "TriggerMenuXML",
        "TrigTransformOld"
        ]

def findPkg(pkg):
    """Find package version in release."""

    cmtPkg = cmt.find_pkg(name=pkg)

    if cmtPkg:
        #print cmtPkg
        return cmtPkg.version
        #return os.path.join(cmtPkg.path,cmtPkg.name)
    else:
        raise RuntimeError, "Package '%s' does not exist" % pkg         



def main():

   parser = argparse.ArgumentParser(description="Copies the mandatory validation packages from the current release to an open release in TagCollector. Current list of validation packages: %r" % getValidationPkgs())

   parser.add_argument('-a', '--add', action='append', metavar='PACKAGE', default=[],
                       help='Additional package to be copied (can be used multiple times)')

   parser.add_argument('-p', '--project', default='AtlasCAFHLT',
                       help='Project to submit to (default: %(default)s)')   

   args = parser.parse_args()
   
   packageVersions = map(findPkg, getValidationPkgs() + args.add)
   cmd = 'acmd.py tc submit-tag -p %s -b TrigValPkgs -s " " -j "Default trigger validation packages" %s' % (args.project," ".join(packageVersions))
   print cmd
   subprocess.call(shlex.split(cmd))



if __name__ == "__main__":
   try:   
      sys.exit(main())
   except RuntimeError, e:
      print e
      sys.exit(1)
   except KeyboardInterrupt:
      sys.exit(1)

        
