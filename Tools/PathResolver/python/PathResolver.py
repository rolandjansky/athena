# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#usage is:
#from PathResolver import PathResolver
#PathResolver.FindCalibFile("blah")


import PyCintex
PyCintex.loadDict('libPathResolverDict')
FindCalibFile = PyCintex.gbl.PathResolverFindCalibFilePython
FindCalibDirectory = PyCintex.gbl.PathResolverFindCalibDirectoryPython

