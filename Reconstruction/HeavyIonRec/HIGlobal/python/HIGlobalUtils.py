# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def AppendOutputList(HIAODItemList) :
    """Adds HIGlobalAODOutputList to the list passed in as an argument"""
    #need to add procedure for straight copy of input
    from HIGlobal.HIGlobalFlags import jobproperties
    keys=[jobproperties.HIGlobalFlags.EventShapeKey()]
    if jobproperties.HIGlobalFlags.DoSummary() : keys+=[jobproperties.HIGlobalFlags.EventShapeSummaryKey()]

    existing_keys=[k.split('#')[1] for k in jobproperties.HIGlobalFlags.HIGlobalItemList() ]

    for shape_key in keys :
        if shape_key not in existing_keys :
            jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeContainer#"+shape_key]
            jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::HIEventShapeAuxContainer#"+shape_key+"Aux."]

    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleContainer#MBTSModules"]
    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::MBTSModuleAuxContainer#MBTSModulesAux."]

    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::ForwardEventInfoContainer#MBTSForwardEventInfo"]
    jobproperties.HIGlobalFlags.HIGlobalItemList+=["xAOD::ForwardEventInfoAuxContainer#MBTSForwardEventInfoAux."]

    HIAODItemList+=jobproperties.HIGlobalFlags.HIGlobalItemList()
