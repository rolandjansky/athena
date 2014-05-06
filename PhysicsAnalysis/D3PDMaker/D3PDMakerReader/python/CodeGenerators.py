# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: CodeGenerators.py 348546 2011-03-01 15:09:56Z krasznaa $

# Gaudi/Athena import(s):
from AthenaCommon.Logging import logging

# Local import(s):
import D3PDMakerReader

##
# Function for generating C++ D3PDReader source files.
#
# @param obj A configured D3PDObject for which the reader should be generated
# @param classname Name of the generated C++ class
# @param dir Directory where the source files should be put
def makeCppRootReaderV1( obj, classname, dir = "./" ):

    # Set up the D3PDSvc:
    __serviceName = "RootReaderD3PDSvcV1"
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr( ServiceMgr, __serviceName ):
        service = D3PDMakerReader.RootReaderD3PDSvc( __serviceName )
        service.Version = 1
        ServiceMgr += service
    service = getattr( ServiceMgr, __serviceName )

    # Create a custom ReaderAlg:
    alg = D3PDMakerReader.ReaderAlg( classname, Directory = dir,
                                     ClassName = classname,
                                     TuplePath = "D3PDReader/" + classname,
                                     D3PDSvc = service )

    # Add the specified object to the ReaderAlg:
    alg += obj
    return

##
# Function for generating C++ D3PDReader source files.
#
# @param obj A configured D3PDObject for which the reader should be generated
# @param classname Name of the generated C++ class
# @param dir Directory where the source files should be put
def makeCppRootReaderV2( obj, classname, dir = "./" ):

    # Set up the D3PDSvc:
    __serviceName = "RootReaderD3PDSvcV2"
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr( ServiceMgr, __serviceName ):
        service = D3PDMakerReader.RootReaderD3PDSvc( __serviceName )
        service.Version = 2
        ServiceMgr += service
    service = getattr( ServiceMgr, __serviceName )

    # Create a custom ReaderAlg:
    alg = D3PDMakerReader.ReaderAlg( classname, Directory = dir,
                                     ClassName = classname,
                                     TuplePath = "D3PDReader/" + classname,
                                     D3PDSvc = service )

    # Add the specified object to the ReaderAlg:
    alg += obj
    return
