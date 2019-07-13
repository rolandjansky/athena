#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TriggerInterface.py
@author P. Onyisi
@date 2019-04-12
@brief Simple new configuration framework functions for getting a TrigDecisionTool. Probably do not work except for reading ESD and AOD. Will be superseded by proper code from Trigger.
'''

def getTrigConfigSvc(flags):
    ''' Setup a TrigConfigSvc with DS information. Works on AOD, not vetted for anything else! '''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TrigConfigSvc.TrigConfigSvcConfig import DSConfigSvc, SetupTrigConfigSvc
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from EventInfoMgt.TagInfoMgrConfig import TagInfoMgrCfg

    rv = ComponentAccumulator()
    # is MC ? then set up XML reading and get out of here
    if flags.Input.isMC:
        from TrigConfigSvc.TrigConfigSvcConfig import (HLTConfigSvc, LVL1ConfigSvc, L1TopoConfigSvc, 
                                                       findFileInXMLPATH, TrigConfigSvc)
        hltcs = HLTConfigSvc("HLTConfigSvc")
        hltcs.XMLMenuFile = findFileInXMLPATH(flags.Trigger.HLTConfigFile)
        print 'hltcs', hltcs.XMLMenuFile
        rv.addService(hltcs)
        lvl1cs = LVL1ConfigSvc("LVL1ConfigSvc")
        lvl1cs.XMLMenuFile = findFileInXMLPATH(flags.Trigger.LVL1ConfigFile)
        rv.addService(lvl1cs)
        l1topocs = L1TopoConfigSvc()
        l1topocs.XMLMenuFile = findFileInXMLPATH(flags.Trigger.LVL1TopoConfigFile)
        rv.addService(l1topocs)
        ts = TrigConfigSvc("TrigConfigSvc")
        # run3_dummy - temporary - until we have a proper HLT configuration source for the Run 3 trigger
        ts.PriorityList = ["run3_dummy", 'xml']
        rv.addService(ts)
        return rv
    rv.addService(DSConfigSvc('DSConfigSvc'))
    tcs = SetupTrigConfigSvc()
    # run3_dummy - temporary - until we have a proper HLT configuration source for the Run 3 trigger
    tcs.SetStates(["run3_dummy", "ds"])
    tcssvc = tcs.GetConfigurable()
    rv.addService(tcssvc)

    rv.merge(addFolders(flags, ['/TRIGGER/HLT/Menu',
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
    rv.merge(TagInfoMgrCfg(flags)[0])
    return rv

def getTrigDecisionTool(flags):
    ''' Setup a TrigDecisionTool. Short-cuts deduplication with memoization.'''
    if getTrigDecisionTool.rv:
        return getTrigDecisionTool.rv
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
 
    rv = ComponentAccumulator()

    if flags.DQ.isReallyOldStyle:
        from AthenaCommon.AppMgr import ToolSvc
        rv.addPublicTool(ToolSvc.TrigDecisionTool)
        getTrigDecisionTool.rv = rv
        return getTrigDecisionTool.rv

    rv.merge(getTrigConfigSvc(flags))
    
    tdt = Trig__TrigDecisionTool('TrigDecisionTool', TrigConfigSvc=rv.getService('TrigConfigSvc'))
    from TrigEDMConfig.TriggerEDM import EDMLibraries
    tdt.Navigation.Dlls = [e for e in  EDMLibraries if 'TPCnv' not in e]
    
    # Other valid option "TriggerElement" for Run 2 navigation. 
    # This option to be removed and "TrigComposite" the only valid choice once a R2->R3 converter is put in place. 
    tdt.NavigationFormat = "TrigComposite"

    rv.addPublicTool(tdt)
    getTrigDecisionTool.rv = rv
    return rv
getTrigDecisionTool.rv = None
