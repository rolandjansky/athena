# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/AthenaJobConfigRun3

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def McEventSelectorCfg(configFlags):
    cfg=ComponentAccumulator()

    from McEventSelector.McEventSelectorConf import McCnvSvc
    mcCnvSvc = McCnvSvc()
    cfg.addService(mcCnvSvc)
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    cfg.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mcCnvSvc.getFullJobOptName(),]))

    from McEventSelector.McEventSelectorConf import McEventSelector
    evSel=McEventSelector("EventSelector")
    rn = configFlags.Input.RunNumber
    if isinstance(rn, type([])):
        rn = rn[0]
    evSel.RunNumber = rn
    evSel.InitialTimeStamp = configFlags.Input.InitialTimeStamp
    cfg.addService(evSel)
    cfg.setAppProperty("EvtSel",evSel.getFullJobOptName())

    return cfg
