# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#usage is:
#from PathResolver import PathResolver
#PathResolver.FindCalibFile("blah")


import cppyy
cppyy.loadDict('libPathResolver')
cppyy.loadDict('libPathResolverDict') #why do we have to load both the library and the dict :-( To be fixed by ROOT...
FindCalibFile = cppyy.gbl.PathResolverFindCalibFile
FindCalibDirectory = cppyy.gbl.PathResolverFindCalibDirectory

