# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# helpers for ExtraVariables appended to content of FTAG derivations

## ----------------------------------------------------------------------------
## b-jet triggers:
## tracks: keep variables as used for the tracking CP group smart-list:
FTExtraVars_bjetTriggerTracks=[]
from DerivationFrameworkInDet.InDetTrackParticlesCPContent import InDetTrackParticlesCPContent as FTExtraVars_InDetTrackParticlesCPContent
FT_IDTPstring="InDetTrackParticlesAux"
for FT_IDTPitem in FTExtraVars_InDetTrackParticlesCPContent:
    if (FT_IDTPstring == FT_IDTPitem.split(".")[0]):
        # tracking used to do primary 
        FTExtraVars_bjetTriggerTracks.append(FT_IDTPitem.replace(FT_IDTPstring,"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF"))
        # tracking used to seed the precision 
        FTExtraVars_bjetTriggerTracks.append(FT_IDTPitem.replace(FT_IDTPstring,"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF"))
        # tracking to do the actual b-tagging
        FTExtraVars_bjetTriggerTracks.append(FT_IDTPitem.replace(FT_IDTPstring,"HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig"))       

## ----------------------------------------------------------------------------
