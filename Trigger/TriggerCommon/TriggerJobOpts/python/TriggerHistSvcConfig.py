#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def setTHistSvcOutput(outputList):
    """Build the Output list of the THistSvc. This is used to
    configure athenaHLT when running from the online DB but with the
    offline THistSvc"""

    if 1 not in [ o.count('SHIFT') for o in outputList ]:
        outputList += [ "SHIFT DATAFILE='shift-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('EXPERT') for o in outputList ]:
        outputList += [ "EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('run_1') for o in outputList ]:
        outputList += [ "run_1 DATAFILE='lbn-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('RUNSTAT') for o in outputList ]:
        outputList += [ "RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'"]
    if 1 not in [ o.count('DEBUG') for o in outputList ]:
        outputList += [ "DEBUG DATAFILE='debug-monitoring.root' OPT='RECREATE'"]

    return

def TriggerHistSvcConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()
    histSvc = CompFactory.THistSvc()
    setTHistSvcOutput(histSvc.Output)
    acc.addService( histSvc )
    return acc
