#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

def TriggerHistSvcConfig(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = ComponentAccumulator()
    from GaudiSvc.GaudiSvcConf import THistSvc
    histSvc = THistSvc()
    histSvc.Output = ["SHIFT DATAFILE='shift-monitoring.root' OPT='RECREATE'", 
                      "EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'", 
                      "run_1 DATAFILE='lbn-monitoring.root' OPT='RECREATE'", 
                      "RUNSTAT DATAFILE='runstat-monitoring.root' OPT='RECREATE'", 
                      "DEBUG DATAFILE='debug-monitoring.root' OPT='RECREATE'"]
    acc.addService( histSvc )
    return acc
