# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
from G4AtlasApps import PyG4Atlas, AtlasG4Eng
from G4AtlasApps.SimFlags import simFlags

def configCosmicFilterVolumeNames():

    # returns a list with volume names
    # depending on how many names are returned different filters must be called

    # easier config, if StoppedParticleFile is on, use plain filter
    if simFlags.StoppedParticleFile.statusOn:
        return ['StoppingPositions']

    # more complex configs, in case real cosmic simulation is done
    volumeName="CaloEntryLayer"
    if simFlags.CosmicFilterVolumeName=="Muon":
        volumeName="MuonExitLayer"
    if simFlags.CosmicFilterVolumeName=="Calo":
       volumeName="MuonEntryLayer"
    if simFlags.CosmicFilterVolumeName=="TRT_Barrel":
        volumeName="TRTBarrelEntryLayer"
    if simFlags.CosmicFilterVolumeName=="TRT_EC":
        volumeName="TRTECAEntryLayer"
    if simFlags.CosmicFilterVolumeName=="SCT_Barrel":
        volumeName="SCTBarrelEntryLayer"
    if simFlags.CosmicFilterVolumeName=="Pixel":
        volumeName="PixelEntryLayer"

    volumeName2=''
    # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
    if simFlags.CosmicFilterVolumeName2.statusOn and simFlags.CosmicFilterVolumeName2 != 'NONE':
        if simFlags.CosmicFilterVolumeName2=="Muon":
            volumeName2="MuonExitLayer"
        if simFlags.CosmicFilterVolumeName=="Calo":
            volumeName="MuonEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="InnerDetector":
            volumeName2="CaloEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="TRT_Barrel":
            volumeName2="TRTBarrelEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="TRT_EC":
            volumeName2="TRTECAEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="SCT_Barrel":
            volumeName2="SCTBarrelEntryLayer"
        if simFlags.CosmicFilterVolumeName2=="Pixel":
            volumeName2="PixelEntryLayer"


    if simFlags.CosmicFilterVolumeName=="TRT_EC":
        # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
        if (not simFlags.CosmicFilterVolumeName2.statusOn) or simFlags.CosmicFilterVolumeName2 == 'NONE' :
            return [volumeName,"TRTECBEntryLayer"]
    # sometimes users turn CosmicFilterVolumeName2 on and set it to NONE
    if simFlags.CosmicFilterVolumeName2.statusOn and  simFlags.CosmicFilterVolumeName2 != 'NONE':
        if simFlags.CosmicFilterVolumeName2=="TRT_EC":
            return [volumeName,volumeName2,"TRTECBEntryLayer"]
        else:
            return [volumeName,volumeName2]

    return [volumeName]

def getCosmicFilter(name="G4CosmicFilter", **kwargs):

    volumes=configCosmicFilterVolumeNames()

    # use simple  cosmic filter
    if len(volumes)==1:

        if simFlags.CosmicFilterID.statusOn:
            kwargs.setdefault("PDGId", simFlags.CosmicFilterID.get_Value())
        if simFlags.CosmicFilterPTmin.statusOn:
            kwargs.setdefault("PtMin", simFlags.CosmicFilterPTmin.get_Value())
        if simFlags.CosmicFilterPTmax.statusOn:
            kwargs.setdefault("PtMax", simFlags.CosmicFilterPTmax.get_Value())
        kwargs.setdefault("VolumeName",volumes[0])

        print 'G4CosmicFilter: Filter volume is %s' % volumes[0]

        return  CfgMgr.G4CosmicFilter(name, **kwargs)

    elif len(volumes)==2:
        # need a cosmic AND filter
        kwargs.setdefault("VolumeName",volumes[0])
        kwargs.setdefault("VolumeName2",volumes[1])
        return  CfgMgr.G4CosmicAndFilter(name, **kwargs)

    else:
        # need a cosmic OR filter
        kwargs.setdefault("VolumeName",volumes[0])
        kwargs.setdefault("VolumeName2",volumes[1])
        kwargs.setdefault("VolumeName3",volumes[2])
        return  CfgMgr.G4CosmicOrFilter(name, **kwargs)
