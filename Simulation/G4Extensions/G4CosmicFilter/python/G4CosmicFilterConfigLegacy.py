# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaCommon import CfgMgr
from G4AtlasApps.SimFlags import simFlags

def configCosmicFilterVolumeNames():

    # returns a list with volume names
    # depending on how many names are returned different filters must be called

    # more complex configs, in case real cosmic simulation is done
    collectionName="CaloEntryLayer"
    if simFlags.CosmicFilterVolumeName=="Muon":
        collectionName="MuonExitLayer"
    if simFlags.CosmicFilterVolumeName=="Calo":
       collectionName="MuonEntryLayer"
    if simFlags.CosmicFilterVolumeName=="TRT_Barrel":
        collectionName="TRTBarrelEntryLayer"
    if simFlags.CosmicFilterVolumeName=="TRT_EC":
        collectionName="TRTECAEntryLayer"
    if simFlags.CosmicFilterVolumeName=="SCT_Barrel":
        collectionName="SCTBarrelEntryLayer"
    if simFlags.CosmicFilterVolumeName=="Pixel":
        collectionName="PixelEntryLayer"

    collectionName2=''
    # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
    if simFlags.CosmicFilterVolumeName2.statusOn and simFlags.CosmicFilterVolumeName2 != 'NONE':
        if simFlags.CosmicFilterVolumeName2=="Muon":
            collectionName2="MuonExitLayer"
        if simFlags.CosmicFilterVolumeName=="Calo":
            collectionName="MuonEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="InnerDetector":
            collectionName2="CaloEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="TRT_Barrel":
            collectionName2="TRTBarrelEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="TRT_EC":
            collectionName2="TRTECAEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="SCT_Barrel":
            collectionName2="SCTBarrelEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="Pixel":
            collectionName2="PixelEntryLayer"


    if simFlags.CosmicFilterVolumeName=="TRT_EC":
        # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
        if (not simFlags.CosmicFilterVolumeName2.statusOn) or simFlags.CosmicFilterVolumeName2 == 'NONE' :
            return [collectionName,"TRTECBEntryLayer"]
    # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
    if simFlags.CosmicFilterVolumeName2.statusOn and  simFlags.CosmicFilterVolumeName2 != 'NONE':
        if simFlags.CosmicFilterVolumeName2=="TRT_EC":
            return [collectionName,collectionName2,"TRTECBEntryLayer"]
        else:
            return [collectionName,collectionName2]

    return [collectionName]

def getCosmicFilterTool(name="G4UA::G4CosmicFilterTool", **kwargs):

    volumes=configCosmicFilterVolumeNames()

    # use simple  cosmic filter
    if len(volumes)==1:

        if simFlags.CosmicFilterID.statusOn:
            kwargs.setdefault("PDGId", simFlags.CosmicFilterID.get_Value())
        if simFlags.CosmicFilterPTmin.statusOn:
            kwargs.setdefault("PtMin", simFlags.CosmicFilterPTmin.get_Value())
        if simFlags.CosmicFilterPTmax.statusOn:
            kwargs.setdefault("PtMax", simFlags.CosmicFilterPTmax.get_Value())
        kwargs.setdefault("CollectionName",volumes[0])

        print ('G4CosmicFilter: Filter volume is %s' % volumes[0])

        return  CfgMgr.G4UA__G4CosmicFilterTool(name, **kwargs)

    elif len(volumes)==2:
        # need a cosmic AND filter
        kwargs.setdefault("CollectionName",volumes[0])
        kwargs.setdefault("CollectionName2",volumes[1])
        return  CfgMgr.G4UA__G4CosmicAndFilterTool(name, **kwargs)

    else:
        # need a cosmic OR filter
        kwargs.setdefault("CollectionName",volumes[0])
        kwargs.setdefault("CollectionName2",volumes[1])
        kwargs.setdefault("CollectionName3",volumes[2])
        return  CfgMgr.G4UA__G4CosmicOrFilterTool(name, **kwargs)

def getStoppedParticleFilterTool(name="G4UA::StoppedParticleFilterTool", **kwargs):
    kwargs.setdefault("CollectionName",'StoppingPositions')
    return  CfgMgr.G4UA__G4CosmicFilterTool(name, **kwargs)
