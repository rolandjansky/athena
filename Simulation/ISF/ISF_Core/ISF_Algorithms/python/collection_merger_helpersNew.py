# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CollectionMergerCfg(ConfigFlags, bare_collection_name,
                                        mergeable_collection_suffix,
                                        merger_input_property):
  """Generates and returns a collection name that is also registered to
     the ISF CollectionMerger algorithm.
 
     :param bare_collection_name: name of the collection if no merging
         is taking place.
     :param mergeable_collection_suffix: suffix to the collection in
         case merging is taking place.
     :param merger_input_property: name of the Input* property in the
         CollectionMerger algorithm to add the mergeable collection to."""

  result = ComponentAccumulator()
  if ConfigFlags.Sim.ISF.Run and ConfigFlags.Sim.ISF.HITSMergingRequired: 
      mergeable_collection = '{bare}{suffix}'.format(
             bare=bare_collection_name,
             suffix=mergeable_collection_suffix)
         

      from AthenaCommon.CfgGetter import getAlgorithm
      algo = getAlgorithm('ISF_CollectionMerger')
      result.addEventAlgo(algo)

      input_attribute_name = 'Input{merger_input_property}'.format(
      merger_input_property=merger_input_property)
    
      merger_input_collections = getattr(algo,input_attribute_name) #empty list always?
      merger_input_collections.append(mergeable_collection)

  else:
      mergeable_collection = bare_collection_name
  print result
  print "#################################"
  return result, mergeable_collection