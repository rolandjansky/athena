#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from PyJobTransformsCore.full_trfarg import *
from PyJobTransformsCore.trf import *
from PyJobTransformsCore.AtlasErrorCodes import ErrorInfo

trf = JobTransform( Author("Martin Woudstra", "Martin.Woudstra@cern.ch"),
                    help = "Testing the DBReleaseArg",
                    skeleton = None,
                    name = "TestDBRelease" )
trf.add(DBReleaseArg())
trf.setLoggerLevel('ALL')

trf.setArgument("DBRelease", "bad_DBRelease.tar.gz" )

try:
    trf.doPreRunActions()
except TransformArgumentError,e:
    expectedException = 'TransformArgumentError'
    errorExpected = 'TRF_DBREL_TARFILE'
    excClass = e.__class__.__name__
    if excClass != expectedException:
        print "Test FAILED: Expected exception of type %s. Got instead: %s: %s" % \
              (expectedException,excClass,e)
        print ErrorInfo()
    elif e.error != errorExpected:
        print "Test FAILED: got correct exception type %s but with error=%s (expected %s)" % (e.__class__.__name__,e.error,expectedError)
        print ErrorInfo()
    else:
        # OK!
        print "Test OK: get expected exception: %s: %s" % (e.__class__.__name__,e)

