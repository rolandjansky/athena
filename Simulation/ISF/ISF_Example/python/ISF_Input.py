# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


## Get the logger
from AthenaCommon.Logging import logging
isfiplog = logging.getLogger('ISF_Input')

# almost all the contents of this file is stolen
# from G4AtlasApps/PyG4Atlas.py

class ISF_Input:
    """
     Place to handle the persistency.
    """

    isfiplog.info("in ISF_Input")

    """ Place to read evgen events.
    """
    from G4AtlasApps.SimFlags import simFlags
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    #G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: starting')

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

    if athenaCommonFlags.PoolEvgenInput.statusOn:
        ## Tell the event selector about the evgen input files and event skipping
        if not hasattr(svcMgr, 'EventSelector'):
            import AthenaPoolCnvSvc.ReadAthenaPool
        svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
        if athenaCommonFlags.SkipEvents.statusOn:
            svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()
        from ISF_Example.ISF_Metadata import inputFileValidityCheck
        inputFileValidityCheck()
    else:
        ## No input file so assume that we are running a Generator in the same job
        if not hasattr(svcMgr, 'EventSelector'):
            import AthenaCommon.AtlasUnixGeneratorJob
        # TODO: Check that there is at least one algorithm already in the AlgSequence?
        ## Warn if attempting to skip events in a generator job
        if athenaCommonFlags.SkipEvents.statusOn:
            msg = "athenaCommonFlags.SkipEvents set in a job without an active "
            msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
            isfiplog.warning(msg)

    isfiplog.verbose('ISF_Input done')
