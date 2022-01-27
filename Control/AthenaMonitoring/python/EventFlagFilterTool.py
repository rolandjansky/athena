# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from functools import cache

# Set up the event cleaning filter tool
# Cache instances that are already created
@cache
def GetEventFlagFilterTool(name, doLAr=True, doTile=True, doSCT=True, doCore=True, alwaysReturnTrue=False):
    """
    Configure an instance of the bad LB filter tool.  If called twice with the same options, will return the same instance.
    Arguments:
        - name: name of instance to create
        - doLAr: do LAr cleaning (optional; default=True)
        - doTile: do Tile cleaning (optional; default=True)
        - doSCT: do SCT cleaning (optional; default=True)
        - doCore: do Core event building cleaning (optional; default=True)
        - alwaysReturnTrue: short-circuit all checks and return True (optional; default=False)
    """
    from AthenaCommon.Logging import logging
    log = logging.getLogger('EventFlagFilterTool')
    log.debug('Setting up DQEventFlagFilter tool with arguments doLAr=%s, doTile=%s, doSCT=%s, doCore=%s, alwaysReturnTrue=%s)', 
              doLAr, doTile, doSCT, doCore, alwaysReturnTrue)

    from AthenaMonitoring.AthenaMonitoringConf import DQEventFlagFilterTool
    monFilterTool = DQEventFlagFilterTool(name, doLAr=doLAr, doTile=doTile,
                                          doSCT=doSCT, doCore=doCore,
                                          alwaysReturnTrue=alwaysReturnTrue)

    return monFilterTool
