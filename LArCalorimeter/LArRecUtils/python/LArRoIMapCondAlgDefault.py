# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from LArCabling.LArCablingAccess import LArOnOffIdMapping, LArFebRodMapping
from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory


def LArRoIMapCondAlgDefault (name = 'LArRoIMapCondAlg'):
   
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq, name):
        return getattr(condSeq, name)

    LArOnOffIdMapping()
    LArFebRodMapping()

    from AthenaCommon.Include import include
    include ('CaloConditions/LArTTCellMap_ATLAS_jobOptions.py')
    include ('CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py')

    CaloTriggerTowerService = CompFactory.CaloTriggerTowerService # CaloTriggerTool
    larRoIMapCondAlg = CompFactory.LArRoIMapCondAlg (name,
                                                     TriggerTowerSvc = CaloTriggerTowerService())
        
    condSeq += larRoIMapCondAlg
    return larRoIMapCondAlg
