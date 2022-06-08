#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

def CTPMonitoringConfig(flags):
    # local printing
    import logging
    local_logger = logging.getLogger('AthenaMonitoringCfg')
    info = local_logger.info
    info('In CTPMonitoringConfig')

    '''Function to call CTP DQ monitoring algorithms'''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()
    #get these for Data only
    if not flags.Input.isMC:
        info('CTPMonitoringConfig: attempting to add DATA COOL folders')
        #if flags.Common.isOnline:
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        result.merge(addFolders(flags,'/LHC/DCS/FILLSTATE','DCS_OFL',className='CondAttrListCollection'))
        ## see https://gitlab.cern.ch/czodrows/athena/-/blob/master/Database/IOVDbSvc/python/IOVDbSvcConfig.py#L171 for db shorthand list
        result.merge(addFolders(flags,'/TDAQ/RunCtrl/DataTakingMode','TDAQ',className='AthenaAttributeList'))
        info('CTPMonitoringConfig: added DATA COOL folders')

    # add CTP and MUCTPI data to ByteStreamAddressProviderSvc
    if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):
        #info('In CTPMonitoringConfig DQ.Environment: %s', flags.DQ.Environment )
        from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
        result.merge(ByteStreamReadCfg(flags, [
            "MuCTPI_RDO/MUCTPI_RDO",
            "CTP_RDO/CTP_RDO",
            #"MuCTPI_RIO/MUCTPI_RIO",
            "CTP_RIO/CTP_RIO"
        ]))
        info('In CTPMonitoringConfig added ByteStreamReadCfg MUCTPI_RDO,CTP_RDO,CTP_RIO' )

    # If we're not putting trigger objects in event store, can't monitor them
    if not flags.Trigger.Online.isPartition:
        if not flags.DQ.triggerDataAvailable:
            return result

    #isData = not flags.Input.isMC
    # monitoring algorithm configs
    # only when input is RAW
    #if flags.DQ.Environment in ('online', 'tier0', 'tier0Raw'):

    ## do not run on MC or RAW->ESD(tier0), or AOD-only
    #if isData and flags.DQ.Environment not in ('tier0Raw', 'AOD'):

    #info('before results_merge CTPMonitoringConfig')
    ## do not run on AOD-only, derivations and ESD (because CTP_RDO version number is missing)
    if flags.DQ.Environment not in ( 'AOD', 'DAOD_PHYS', 'tier0ESD'): #, 'tier0Raw' ): 
        from TrigT1CTMonitoring.BSMonitoringAlgorithm import BSMonitoringConfig
        result.merge(BSMonitoringConfig(flags))
        info('CTPMonitoringConfig: requested: result.merge(BSMonitoringConfig(flags))')
    info('after results_merge CTPMonitoringConfig')
    return result
