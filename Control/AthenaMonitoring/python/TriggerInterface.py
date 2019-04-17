#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TriggerInterface.py
@author P. Onyisi
@date 2019-04-12
@brief Simple new configuration framework functions for getting a TrigDecisionTool. Probably do not work except for reading ESD and AOD. Will be superseded by proper code from Trigger.
'''

def getTrigConfigSvc(inputFlags):
    ''' Setup a TrigConfigSvc with DS information. Works on AOD, not vetted for anything else! '''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TrigConfigSvc.TrigConfigSvcConfig import DSConfigSvc, SetupTrigConfigSvc
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg

    rv = ComponentAccumulator()
    rv.addService(DSConfigSvc('DSConfigSvc'))
    tcs = SetupTrigConfigSvc()
    tcs.SetStates(["ds"])
    tcssvc = tcs.GetConfigurable()
    rv.addService(tcssvc)

    rv.merge(addFolders(inputFlags, ['/TRIGGER/HLT/Menu',
                                     '/TRIGGER/HLT/HltConfigKeys',
                                     '/TRIGGER/LVL1/Lvl1ConfigKey',
                                     '/TRIGGER/LVL1/Menu',
                                     '/TRIGGER/LVL1/Prescales',
                                     '/TRIGGER/LVL1/BunchGroupKey',
                                     '/TRIGGER/LVL1/BunchGroupDescription',
                                     '/TRIGGER/LVL1/BunchGroupContent',
                                     '/TRIGGER/HLT/Prescales',
                                     '/TRIGGER/HLT/PrescaleKey',
                                     '/TRIGGER/LVL1/ItemDef'],
                        'TRIGGER', tag='HEAD'))
    
    # the following should probably be set up by IOVDbSvc configuration?
    rv.merge(TagInfoMgrCfg(inputFlags)[0])
    return rv

def getTrigDecisionTool(inputFlags):
    ''' Setup a TrigDecisionTool. Short-cuts deduplication with memoization.'''
    if getTrigDecisionTool.rv:
        return rv
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
 
    rv = ComponentAccumulator()
    rv.merge(getTrigConfigSvc(inputFlags))
    
    tdt = Trig__TrigDecisionTool('TrigDecisionTool', TrigConfigSvc=rv.getService('TrigConfigSvc'))
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    rv.addPublicTool(tdt)
    getTrigDecisionTool.rv = rv
    return rv
getTrigDecisionTool.rv = None
