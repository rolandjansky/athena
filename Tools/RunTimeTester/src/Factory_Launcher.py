# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Launchers govern the rate at which jobs are started.
BatchLauncher,       submits all the jobs in rapid succession.
InteractiveLauncher, waits for a job to finish before submitting the next.
"""

from Launcher               import ThrottleLauncher

def launcherFactory(mode, minders, logger):
    noToLaunch = 40
    if mode == 'LinuxInteractive': noToLaunch = 1
    return ThrottleLauncher(noToLaunch, minders, logger)
