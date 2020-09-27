# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags

################################################################################
# HIT CREATION SECTION
################################################################################

#   Fatras Hadronic Interaction Processor
#   hadronic interaction creator
def getFatrasHitCreatorPixel(name="ISF_FatrasHitCreatorPixel", **kwargs):
    bare_collection_name = "PixelHits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "PixelHits"
    region = 'ID'
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)

    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("IdHelperName"    , 'PixelID')
    kwargs.setdefault("CollectionName"  , hits_collection_name)
    kwargs.setdefault("UseConditionsSvc", False)

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorSilicon
    return iFatras__HitCreatorSilicon(name, **kwargs )


def getFatrasPileupHitCreatorPixel(name="ISF_FatrasPileupHitCreatorPixel", **kwargs):
    kwargs.setdefault("CollectionName"  , 'PileupPixelHits')
    return getFatrasHitCreatorPixel(name, **kwargs)


def getFatrasHitCreatorSCT(name="ISF_FatrasHitCreatorSCT", **kwargs):
    bare_collection_name = "SCT_Hits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "SCTHits"
    region = 'ID'
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("IdHelperName"    , 'SCT_ID')
    kwargs.setdefault("CollectionName"  , hits_collection_name)
    kwargs.setdefault("UseConditionsSvc", False)

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorSilicon
    return iFatras__HitCreatorSilicon(name, **kwargs )


def getFatrasPileupHitCreatorSCT(name="ISF_FatrasPileupHitCreatorSCT", **kwargs):
    kwargs.setdefault("CollectionName"  , 'PileupSCT_Hits')
    return getFatrasHitCreatorSCT(name, **kwargs)


def getFatrasHitCreatorTRT(name="ISF_FatrasHitCreatorTRT", **kwargs):
    bare_collection_name = "TRTUncompressedHits"
    mergeable_collection_suffix = "_Fatras"
    merger_input_property = "TRTUncompressedHits"
    region = 'ID'
    hits_collection_name = generate_mergeable_collection_name(bare_collection_name,
                                                              mergeable_collection_suffix,
                                                              merger_input_property,
                                                              region)
    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("CollectionName"  , hits_collection_name)

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__HitCreatorTRT
    return iFatras__HitCreatorTRT(name, **kwargs )


def getFatrasPileupHitCreatorTRT(name="ISF_FatrasPileupHitCreatorTRT", **kwargs):
    kwargs.setdefault("CollectionName"  , 'PileupTRTUncompressedHits')
    return getFatrasHitCreatorTRT(name, **kwargs)


################################################################################
# TRACK CREATION SECTION
################################################################################
def getFatrasSimHitCreatorID(name="ISF_FatrasSimHitCreatorID", **kwargs):
    kwargs.setdefault("PixelHitCreator" , 'ISF_FatrasHitCreatorPixel')
    kwargs.setdefault("SctHitCreator"   , 'ISF_FatrasHitCreatorSCT')
    kwargs.setdefault("TrtHitCreator"   , 'ISF_FatrasHitCreatorTRT')
    kwargs.setdefault("OutputLevel"     , ISF_FatrasFlags.OutputLevelGeneral())

    from ISF_FatrasToolsID.ISF_FatrasToolsIDConf import iFatras__SimHitCreatorID
    return iFatras__SimHitCreatorID(name, **kwargs )


def getFatrasPileupSimHitCreatorID(name="ISF_FatrasPileupSimHitCreatorID", **kwargs):
    kwargs.setdefault("PixelHitCreator" , 'ISF_FatrasPileupHitCreatorPixel')
    kwargs.setdefault("SctHitCreator"   , 'ISF_FatrasPileupHitCreatorSCT')
    kwargs.setdefault("TrtHitCreator"   , 'ISF_FatrasPileupHitCreatorTRT')
    return getFatrasSimHitCreatorID(name, **kwargs )

