# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def AppendOutputList(HIAODItemList) :
    """Adds HIGlobalAODOutputList to the list passed in as an argument"""
    HIAODItemList+= ["xAOD::TriggerTowerContainer#ZdcTriggerTowers",
                     "xAOD::TriggerTowerAuxContainer#ZdcTriggerTowersAux.",
                     "xAOD::ZdcModuleContainer#ZdcModules",
                     "xAOD::ZdcModuleAuxContainer#ZdcModulesAux.",
                     "xAOD::ZdcModuleContainer#ZdcSums",
                     "xAOD::ZdcModuleAuxContainer#ZdcSumsAux."] 
