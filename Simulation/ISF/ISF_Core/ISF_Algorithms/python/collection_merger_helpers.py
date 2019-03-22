from AthenaCommon.CfgGetter import getAlgorithm
from G4AtlasApps.SimFlags import simFlags
from ISF_Config.ISF_jobProperties import ISF_Flags
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

def generate_mergeable_collection_name(bare_collection_name,
                                       mergeable_collection_suffix,
                                       merger_input_property):
    """
    Generates and returns a collection name that is also registered to
    the ISF CollectionMerger algorithm.

    :param bare_collection_name: name of the collection if no merging
        is taking place.
    :param mergeable_collection_suffix: suffix to the collection in
        case merging is taking place.
    :param merger_input_property: name of the Input* property in the
        CollectionMerger algorithm to add the mergeable collection to.
    """
    hardscatterSG = "OriginalEvent_SG+"
    if simFlags.ISFRun() and ISF_Flags.HITSMergingRequired():
        mergeable_collection = '{bare}{suffix}'.format(
            bare=bare_collection_name,
            suffix=mergeable_collection_suffix
        )

        collection_merger = getAlgorithm('ISF_CollectionMerger')
        input_attribute_name = 'Input{merger_input_property}'.format(
            merger_input_property=merger_input_property)
        merger_input_collections = getattr(collection_merger,
                                           input_attribute_name)
        merger_input_collections.append(mergeable_collection)
    elif athenaCommonFlags.DoFullChain() and DetFlags.pileup.any_on():
        mergeable_collection = hardscatterSG+bare_collection_name
    else:
        mergeable_collection = bare_collection_name

    return mergeable_collection
