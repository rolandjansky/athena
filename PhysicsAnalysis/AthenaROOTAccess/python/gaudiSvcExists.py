# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##
# $Id: gaudiSvcExists.py,v 1.1 2008-08-26 18:34:50 ssnyder Exp $
#
# @file AthenaROOTAccess/python/gaudiSvcExists.py
# @author sss
# @date Aug 2008
# @brief Test to see if a given Gaudi service exists.
#        Try to be careful not to trigger autoloading of anything
#        that's not already loaded.
#


import ROOT
import cppyy


def gaudiSvcExists (svcname):
    """Test to see if a given Gaudi service exists.
    Try to be careful not to trigger autoloading of anything
    that's not already loaded.
"""

    # See if the GaudiKernel dictionary has been loaded.
    # Assume the service isn't there if not.
    if "Service" not in cppyy.getAllClasses():
        return False

    # Try to get the service locator.
    slfunc = ROOT.Gaudi.__dict__.get('svcLocator')
    if not slfunc:
        return False
    sl = slfunc()

    # Test for the service.
    return sl.existsService (svcname)
