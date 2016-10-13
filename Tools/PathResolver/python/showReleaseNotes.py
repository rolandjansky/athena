#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file showReleaseNotes.py
# @purpose Display important notes/announcments (if any) about the current release
# @author Will Buttinger
# @date April 2016


def main():
   import os
   #we don't use the 'from PathResolver import PathResolver' because that will force Gaudi ApplicationMgr to display
   #Until we have a way to 'silently' start the applicationmgr, we will do it the 'bad' way
   import cppyy
   cppyy.loadDict('libPathResolver')
   cppyy.loadDict('libPathResolverDict')
   FindCalibFile = cppyy.gbl.PathResolverFindCalibFile
   cppyy.gbl.PathResolverSetOutputLevel(6) #suppress all possible warnings/errors

   notesFile = FindCalibFile("dev/ReleaseNotes/%s/%s" % (os.environ['AtlasProject'],os.environ['AtlasVersion']))
   if notesFile=="": return 0 #no notes to display

   with open(notesFile) as f: 
      print f.read()




if __name__ == "__main__":
    import sys
    sys.exit(main())
