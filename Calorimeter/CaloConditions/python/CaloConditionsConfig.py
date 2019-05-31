"""ComponentAccumulator configuration utilities for Calorimeter Conditions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def CaloTriggerTowerCfg(flags):
    """Return ComponentAccumulator configured for Trigger Tower"""
    # replaces CaloConditions_jobOptions.py or CaloDetMgrDetDescrCnv_joboptions.py
    acc = ComponentAccumulator()
    if flags.GeoModel.Layout != "ctbh8":
        acc.merge(LArTTCellMapCfg(flags))
        acc.merge(CaloTTIdMapCfg(flags))
    return acc

def LArTTCellMapCfg(flags):
    """Return ComponentAccumulator configured for LAr Trigger Tower"""
    # replaces LArTTCellMap_ATLAS_jobOptions.py
    folder = "/LAR/Identifier/LArTTCellMapAtlas" + flags.LAr.DBConnection
    acc = addFoldersSplitOnline(flags, "LAR", folder, folder)
    # override not migrated
    #DetDescr = flags.Global.DetDescrVersion
    #if DetDescr.startswith("ATLAS-CSC-01") or DetDescr.startswith("ATLAS-DC"):
    #    conddb.addOverride("/LAR/Identifier/LArTTCellMapAtlas","LARIdentifierLArTTCellMapAtlas-DC3-02")
    return acc

def CaloTTIdMapCfg(flags):
    """Return ComponentAccumulator configured for Calo Trigger Tower"""
    # replaces CaloTTIdMap_ATLAS_jobOptions.py
    acc = ComponentAccumulator()
    folders = ["CaloTTOnOffIdMapAtlas", "CaloTTOnAttrIdMapAtlas", "CaloTTPpmRxIdMapAtlas"]
    for f in folders:
        endf = "/Identifier/" + f
        acc.merge(addFoldersSplitOnline(flags, "CALO", "/CALO" + endf, "/CALO/Ofl" + endf))
    return acc

