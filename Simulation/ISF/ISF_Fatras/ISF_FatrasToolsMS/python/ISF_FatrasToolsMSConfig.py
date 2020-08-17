# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from ISF_Algorithms.collection_merger_helpers import generate_mergeable_collection_name
from ISF_FatrasServices.ISF_FatrasJobProperties import ISF_FatrasFlags

################################################################################
# TRACK CREATION SECTION
################################################################################
def getFatrasSimHitCreatorMS(name="ISF_FatrasSimHitCreatorMS", **kwargs):
    mergeable_collection_suffix = "_Fatras"
    region = 'MUON'

    mdt_bare_collection_name = "MDT_Hits"
    mdt_merger_input_property = "MDTHits"
    mdt_hits_collection_name = generate_mergeable_collection_name(mdt_bare_collection_name,
                                                                  mergeable_collection_suffix,
                                                                  mdt_merger_input_property,
                                                                  region)
    rpc_bare_collection_name = "RPC_Hits"
    rpc_merger_input_property = "RPCHits"
    rpc_hits_collection_name = generate_mergeable_collection_name(rpc_bare_collection_name,
                                                                  mergeable_collection_suffix,
                                                                  rpc_merger_input_property,
                                                                  region)
    tgc_bare_collection_name = "TGC_Hits"
    tgc_merger_input_property = "TGCHits"
    tgc_hits_collection_name = generate_mergeable_collection_name(tgc_bare_collection_name,
                                                                  mergeable_collection_suffix,
                                                                  tgc_merger_input_property,
                                                                  region)
    csc_bare_collection_name = "CSC_Hits"
    csc_merger_input_property = "CSCHits"
    csc_hits_collection_name = generate_mergeable_collection_name(csc_bare_collection_name,
                                                                  mergeable_collection_suffix,
                                                                  csc_merger_input_property,
                                                                  region)

    from G4AtlasApps.SimFlags import simFlags
    kwargs.setdefault("RandomNumberService" , simFlags.RandomSvc() )
    kwargs.setdefault("RandomStreamName"    , ISF_FatrasFlags.RandomStreamName())
    kwargs.setdefault("Extrapolator" , 'ISF_FatrasExtrapolator')
    kwargs.setdefault("MDTCollectionName", mdt_hits_collection_name)
    kwargs.setdefault("RPCCollectionName", rpc_hits_collection_name)
    kwargs.setdefault("TGCCollectionName", tgc_hits_collection_name)
    kwargs.setdefault("CSCCollectionName", csc_hits_collection_name)

    from ISF_FatrasToolsMS.ISF_FatrasToolsMSConf import iFatras__SimHitCreatorMS
    return iFatras__SimHitCreatorMS(name, **kwargs )
