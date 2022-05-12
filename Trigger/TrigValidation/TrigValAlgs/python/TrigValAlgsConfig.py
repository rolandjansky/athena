# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigValAlgs.TrigValAlgsConf import TrigEDMChecker
from TrigValAlgs.TrigValAlgsConf import TrigEDMAuxChecker


# TrigEDMChecker configurable
class TrigEDMChecker ( TrigEDMChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMChecker"):
        super( TrigEDMChecker, self ).__init__( name )


def getEDMAuxList():
    from TrigEDMConfig.TriggerEDM import getTriggerEDMList
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    EDMVersion = ConfigFlags.Trigger.EDMVersion
    AODEDMSet = ConfigFlags.Trigger.AODEDMSet
    tlist=getTriggerEDMList(AODEDMSet,EDMVersion)
    objlist=[]
    for t,kset in tlist.items():
        for k in kset:
             if 'Aux.' in k:
                 s = k.split('.',1)[0] + "."
                 # Types of these collections from Run 2 do not inherit from xAOD::AuxContainerBase, so can't test them here
                 if EDMVersion == 2 and "HLT_xAOD__JetContainer" in s or "xTrigDecisionAux" in s or "TrigNavigationAux" in s:
                     continue
                 objlist += [s]
    return objlist

# TrigEDMChecker configurable
class TrigEDMAuxChecker ( TrigEDMAuxChecker ):
    __slots__ = []
    def __init__(self, name="TrigEDMAuxChecker"):
        super( TrigEDMAuxChecker, self ).__init__( name )

    def setDefaults(self, handle) :
        #self.AuxContainerList=['HLT_xAOD__PhotonContainer_egamma_PhotonsAux.']
        self.AuxContainerList=getEDMAuxList()
    
    
