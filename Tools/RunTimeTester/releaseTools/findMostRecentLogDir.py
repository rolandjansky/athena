# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os

# example root: '/afs/cern.ch/atlas/project/RTT/prod/Peter/logs'

def findMostRecentLogDir(root):

    logdirs = [os.path.join(root,l) for l in os.listdir(root)]
    logdirs = [(os.stat(l).st_atime, l) for l in logdirs]
    logdirs.sort()
    return logdirs[-1][1]
