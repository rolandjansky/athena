# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def LArG4FastSimSvcInit():

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'LArG4FastSimSvcInit' )

    # get parametrisation level
    from G4AtlasApps.SimFlags import simFlags

    if not hasattr(simFlags, 'LArParameterization'):
        log.warning("JobProperty LArParameterization is not defined! Can not set up fast simulation!")
        return

    if simFlags.LArParameterization() == 0:
        log.warning("JobProperty LArParameterization is zero! No fast simulation requested!")
        return

    # get service manager
    from AthenaCommon.AppMgr import ServiceMgr

    # add EMB
    if not hasattr(ServiceMgr, 'LArG4EMBFastSimSvc' ):
        log.info("Adding LArG4EMBFastSimSvc to ServiceMgr")
        from LArG4FastSimSvc.LArG4FastSimSvcConfig import LArG4EMBFastSimSvc
        ServiceMgr += LArG4EMBFastSimSvc()

    # add EMEC
    if not hasattr(ServiceMgr, 'LArG4EMECFastSimSvc' ):
        log.info("Adding LArG4EMECFastSimSvc to ServiceMgr")
        from LArG4FastSimSvc.LArG4FastSimSvcConfig import LArG4EMECFastSimSvc
        ServiceMgr += LArG4EMECFastSimSvc()
            
    # add FCAL
    if not hasattr(ServiceMgr, 'LArG4FCALFastSimSvc' ):
        log.info("Adding LArG4FCALFastSimSvc to ServiceMgr")
        from LArG4FastSimSvc.LArG4FastSimSvcConfig import LArG4FCALFastSimSvc
        ServiceMgr += LArG4FCALFastSimSvc()

    # add FCAL2
    if not hasattr(ServiceMgr, 'LArG4FCAL2FastSimSvc' ):
        log.info("Adding LArG4FCAL2FastSimSvc to ServiceMgr")
        from LArG4FastSimSvc.LArG4FastSimSvcConfig import LArG4FCAL2FastSimSvc
        ServiceMgr += LArG4FCAL2FastSimSvc()

    # add Dead Material
    if not hasattr(ServiceMgr, 'DeadMaterialFastSimSvc' ):
        log.info("Adding DeadMaterialFastSimSvc to ServiceMgr")
        from LArG4FastSimSvc.LArG4FastSimSvcConfig import DeadMaterialFastSimSvc
        ServiceMgr += DeadMaterialFastSimSvc()

    if not hasattr( ServiceMgr, 'LArG4ShowerLibSvc' ):
        log.info("Adding ShowerLibSvc to ServiceMgr")
        from LArG4ShowerLibSvc.LArG4ShowerLibSvcConf import LArG4ShowerLibSvc
        ServiceMgr += LArG4ShowerLibSvc()
