#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def LVL1InterfacesMonitoringCfg(flags):
    '''Function to call T1 interfaces DQ monitoring algorithms'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    import logging

    # local printing
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    info('In LVL1InterfacesMonitoringCfg')

    result = ComponentAccumulator()

    # If we're not putting trigger objects in event store, can't monitor them
    if not flags.DQ.triggerDataAvailable:
        return result

    # to deal with trigger menu updates                                                                                                    
    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    result.merge(L1ConfigSvcCfg(flags))

    isData = not flags.Input.isMC

    # monitoring algorithm configs
    # do not run on MC or  RAW->ESD(tier0), or AOD-only
    if isData and flags.DQ.Environment not in ('tier0Raw', 'AOD'):
        from TrigT1Monitoring.L1CaloL1TopoMonitorAlgorithm import L1CaloL1TopoMonitoringConfig
        result.merge(L1CaloL1TopoMonitoringConfig(flags))

    return result