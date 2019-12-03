#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TriggerInterface.py
@author P. Onyisi
@date 2019-04-12
@brief Simple new configuration framework functions for getting a TrigDecisionTool. Probably do not work except for reading ESD and AOD. Will be superseded by proper code from Trigger.
'''

def getTrigDecisionTool(flags):
    ''' Setup a TrigDecisionTool. Short-cuts deduplication with memoization.'''
    if getTrigDecisionTool.rv:
        return getTrigDecisionTool.rv
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
 
    rv = ComponentAccumulator()

    if flags.DQ.isReallyOldStyle:
        from AthenaCommon.AppMgr import ToolSvc
        rv.addPublicTool(ToolSvc.TrigDecisionTool)
        getTrigDecisionTool.rv = rv
        return getTrigDecisionTool.rv

    from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool
    cfgtool = TrigConf__xAODConfigTool('xAODConfigTool')
    rv.addPublicTool(cfgtool)

    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    tdt = Trig__TrigDecisionTool('TrigDecisionTool')
    tdt.ConfigTool = cfgtool
    tdt.NavigationFormat = "TrigComposite" if 'HLTNav_Summary' in flags.Input.Collections else "TriggerElement"
    rv.addPublicTool(tdt)
    # Other valid option of NavigationFormat is "TriggerElement" for Run 2 navigation. 
    # This option to be removed and "TrigComposite" the only valid choice once a R2->R3 converter is put in place. 
    # For now, it is assumed that any Run 2 AOD will be used with the legacy monitoring

    from TrigEDMConfig.TriggerEDM import EDMLibraries
    tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]

    # Minimal config needed to read metadata: MetaDataSvc & ProxyProviderSvc
    from AthenaServices.AthenaServicesConf import MetaDataSvc
    mdSvc = MetaDataSvc( "MetaDataSvc" )
    mdSvc.MetaDataContainer = "MetaDataHdr"
    rv.addService(mdSvc)

    from SGComps.SGCompsConf import ProxyProviderSvc
    pdps = ProxyProviderSvc( "ProxyProviderSvc" )
    pdps.ProviderNames += [ "MetaDataSvc" ]
    rv.addService(pdps)

    getTrigDecisionTool.rv = rv
    return rv
getTrigDecisionTool.rv = None
