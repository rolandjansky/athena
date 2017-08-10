# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id$
#
# File: SGComps/python/AddressRemappingSvc.py
# Created: Sep 2016, sss
# Purpose: Helpers for configuring the AddressRemappingSvc.
#


def getAddressRemappingSvc():
    """Return the AddressRemappingSvc configurable."""
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from SGComps.SGCompsConf import AddressRemappingSvc, ProxyProviderSvc
    pps = ProxyProviderSvc()
    ars = AddressRemappingSvc()
    svcMgr += pps
    svcMgr += ars
    if 'AddressRemappingSvc' not in pps.ProviderNames:
        pps.ProviderNames += [ 'AddressRemappingSvc' ]
    return ars


def addInputRename (type, from_name, to_name):
    """Add a new input renaming.

    For example:

      addInputRename ("Foo", "foo", "bar")

    to rename the object of type Foo named `foo' to `bar'.

    May also be used to rename dynamic (but NOT static) auxiliary variables:

      addInputRename ("Foo", "foo.d1", "foo.x1")

    If both are combined, write it like this:

      addInputRename ("Foo", "foo", "bar")
      addInputRename ("Foo", "foo.d1", "bar.x1")
"""
    ars = getAddressRemappingSvc()
    ars.TypeKeyRenameMaps += [ '%s#%s->%s' % (type, from_name, to_name) ]
    return


def addOverwrite (from_type, from_name, to_type, to_name):
    """Add a new type overwrite mapping."""
    ars = getAddressRemappingSvc()
    ars.TypeKeyRenameMaps += [ '%s#%s->%s#%s' % (from_type, from_name,
                                                 to_type, to_name) ]
    return

