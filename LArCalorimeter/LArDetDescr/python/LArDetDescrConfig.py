"""ComponentAccumulator configuration utilities for LAr Detector Description

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from CaloConditions.CaloConditionsConfig import CaloTriggerTowerCfg
from LArRawConditions.LArRawConditionsConfig import LArRawConditionsMCCfg
from LArRawConditions.LArRawConditionsConfig import LArRawConditionsDataCfg

def LArDetDescrCfg(flags):
    """Return ComponentAccumulator configured for LAr readout geometry"""
    # replaces LArDetDescr_joboptions.py
    acc = CaloTriggerTowerCfg(flags)
    # not migrated not RecFlags.rec.doAODMerging requirement
    if flags.Input.isMC:
        acc.merge(LArRawConditionsMCCfg(flags))
    else:
        acc.merge(LArRawConditionsDataCfg(flags))
    return acc

