#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def LVL1CaloMonitoringConfig(flags):
    '''Function to call l1calo DQ monitoring algorithms'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    import logging

    # local printing
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    info('In LVL1CaloMonitoringConfig')

    result = ComponentAccumulator()

    # monitoring algorithm configs
    # do not run in RAW->ESD, or AOD-only
    if flags.DQ.Environment not in ('tier0Raw', 'AOD'):
        from TrigT1CaloMonitoring.CpmMonitorAlgorithm import CpmMonitoringConfig
        result.merge(CpmMonitoringConfig(flags))


    return result
