#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

def LVL1CaloMonitoringConfig(flags):
    '''Function to call l1calo DQ monitoring algorithms'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.Enums import Format
    import logging

    # local printing
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    info('In LVL1CaloMonitoringConfig')

    result = ComponentAccumulator()

    # If we're not putting trigger objects in event store, can't monitor them
    if not flags.DQ.triggerDataAvailable:
        return result

    #TODO restore proper config
    if flags.Trigger.triggerConfig == "INFILE":
        return result

    from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
    result.merge(L1ConfigSvcCfg(flags))

    isData = not flags.Input.isMC

    # monitoring algorithm configs
    # do not run on MC or  RAW->ESD(tier0), or AOD-only
    if isData and flags.DQ.Environment not in ('tier0Raw', 'AOD'):
        from TrigT1CaloMonitoring.CpmMonitorAlgorithm import CpmMonitoringConfig
        from TrigT1CaloMonitoring.CpmSimMonitorAlgorithm import CpmSimMonitoringConfig
        from TrigT1CaloMonitoring.PprMonitorAlgorithm import PprMonitoringConfig
        from TrigT1CaloMonitoring.JepJemMonitorAlgorithm import JepJemMonitoringConfig


        result.merge(CpmMonitoringConfig(flags))
        result.merge(CpmSimMonitoringConfig(flags))
        result.merge(PprMonitoringConfig(flags))
        result.merge(JepJemMonitoringConfig(flags))

        # For online running on bytestream data 
        if flags.Input.Format is Format.BS and flags.Trigger.Online.isPartition:
            from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
            result.merge(LVL1CaloRun2ReadBSCfg(flags))

    return result
