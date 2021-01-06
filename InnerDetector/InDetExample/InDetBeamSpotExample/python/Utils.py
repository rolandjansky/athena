#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
This module defines miscellaneous utility functions.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'

import os, re


def getRunFromName(name,default='',asInt=False):
    """Extract the run number from a file name whose first part is a standard dataset name.
       If the run number cannot be determined, returns the default value, which, by
       default is an empty string."""
    name = os.path.basename(name)
    try:
        run = re.sub('^0*','',name.split('.')[1])
        if not re.search(r'^\d+$',run):
            # Probably wasn't the run number, so use the default instead
            run = default
        else:
            run = int(run) if asInt else run
    except Exception:
        run = default
    return run


def fileListSnippet(files,dsName,taskName,jobDir=None):
    s = ''
    if files:
        for f in files.split():
            if jobDir and not os.path.exists('%s/%s/%s/%s' % (jobDir,dsName,taskName,f)):
                s += '%s (archived)<br>' % f
            else:
                s += '<a href="/jobfiles/%s/%s/%s">%s</a><br>' % (dsName,taskName,f,f)
    return s


def blankIfNone(s):
    if s:
        return s
    else:
        return ''


def getUserName(default='UNKNOWN'):
    """Get login name in a platform-independent manner."""
    user = ''
    try:
        user = os.getlogin()     # this doesn't seem to work with acrontab
    except Exception:
        pass
    if not user:
        user = os.getenv('USER',default)
    return user


def getHostName():
    import platform
    return platform.node()
