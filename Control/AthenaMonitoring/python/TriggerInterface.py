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
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODConfigTool


 
    rv = ComponentAccumulator()

    if flags.DQ.isReallyOldStyle:
        from AthenaCommon.AppMgr import ToolSvc
        rv.addPublicTool(ToolSvc.TrigDecisionTool)
        getTrigDecisionTool.rv = rv
        return getTrigDecisionTool.rv
    
    tdt = Trig__TrigDecisionTool('TrigDecisionTool')
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    
    # Other valid option "TriggerElement" for Run 2 navigation. 
    # This option to be removed and "TrigComposite" the only valid choice once a R2->R3 converter is put in place. 
    tdt.NavigationFormat = "TrigComposite"
    tdt.OutputLevel = 1
    print("TIMM")

    cfgtool = TrigConf__xAODConfigTool('xAODConfigTool')
    cfgtool.OutputLevel = 1

    rv.addPublicTool(tdt)
    rv.addPublicTool(cfgtool)

    getTrigDecisionTool.rv = rv
    return rv
getTrigDecisionTool.rv = None
