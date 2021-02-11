#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def setTHistSvcOutput(outputList):
    """Build the Output list of the THistSvc ensuring minimum set of output files"""

    if not any('EXPERT' in o for o in outputList):
        outputList += ["EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]

    return

def TriggerHistSvcConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()
    histSvc = CompFactory.THistSvc()
    setTHistSvcOutput(histSvc.Output)
    acc.addService( histSvc )
    return acc
