# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from collections import OrderedDict as odict
from TrigConfigSvc.TrigConfigSvcCfg import getHLTMenuFileName
from AthenaCommon.CFElements import getSequenceChildren, isSequence
from AthenaCommon.Logging import logging
__log = logging.getLogger( __name__ )

def getChildrenIfSequence( s ):
    return  getSequenceChildren( s ) if isSequence( s ) else []

def __getStepsDataFromAlgSequence(HLTAllSteps):
    """ Generates a list where the index corresponds to a Step number and the stored object is a list of Sequencers making up the Step 
    """
    stepsData = []
    if HLTAllSteps is not None:
        for HLTStep in getSequenceChildren( HLTAllSteps ):
            if "_reco" not in HLTStep.getName(): # Avoid the pre-step Filter execution
                # Look for newJO reco
                for Step in getChildrenIfSequence( HLTStep ):
                    for View in getChildrenIfSequence( Step ):
                        for Reco in getChildrenIfSequence( View ):
                            if "_reco" in Reco.getName() and HLTStep.getName() not in stepsData:
                                stepsData.append( getSequenceChildren( HLTStep ) )
                                break
                continue

            stepsData.append( getSequenceChildren( HLTStep ) )
    else:
        __log.warn( "No HLTAllSteps sequencer, will not export per-Step data for chains.")
    return stepsData

def __getChainSequencers(stepsData, chainName):
    """ Finds the Filter which is responsible for this Chain in each Step.
        Return a list of the per-Step name() of the Sequencer which is unlocked by the Chain's Filter in the Step.
    """
    sequencers = []
    counter = 0
    from DecisionHandling.TrigCompositeUtils import chainNameFromLegName
    for step in stepsData:
        counter += 1
        mySequencer = None
        endOfChain = False
        for sequencer in step:
            sequencerFilter = getSequenceChildren( sequencer )[0] # Always the first child in the step
            if hasattr(sequencerFilter, "Chains") and any(chainName in chainNameFromLegName(fChain) for fChain in sequencerFilter.Chains):
                if mySequencer is not None:
                    __log.error( "Multiple Filters found (corresponding Sequencers %s, %s) for %s in Step %i!",
                        mySequencer.getName(), sequencer.getName(), chainName, counter)
                mySequencer = sequencer
        if mySequencer is None:
            endOfChain = True
            if counter == 1 and  'noalg' not in chainName:
                __log.info("No Filter found for %s in Step 1", chainName)
        else:
            if endOfChain is True:
                __log.error( "Found another Step, (Step %i) for chain %s "
                    "which looked like it had already finished after %i Steps!", 
                    counter, chainName, len(sequencers))
            sequencers.append(mySequencer.getName())
    return sequencers

def __getSequencerAlgs(stepsData):
    """ For each Sequencer in each Step, return a flat list of the full name of all Algorithms under the Sequencer
    """
    from AthenaCommon.CFElements import findAllAlgorithms
    sequencerAlgs = odict()
    for step in stepsData:
        for sequencer in step:
            sequencerAlgs[ sequencer.getName() ] = list(map(lambda x: x.getFullJobOptName(), findAllAlgorithms(sequencer)))
    return sorted(sequencerAlgs.items(), key=lambda t: t[0])

def __generateJSON( chainDicts, chainConfigs, HLTAllSteps, menuName, fileName ):
    """ Generates JSON given the ChainProps and sequences
    """
    # Menu dictionary that is used to create the JSON content
    menuDict = odict([ ("filetype", "hltmenu"), ("name", menuName), ("chains", odict()), ("streams", odict()), ("sequencers", odict()) ])

    # List of steps data for sequencers
    stepsData = __getStepsDataFromAlgSequence(HLTAllSteps)

    from TriggerMenuMT.HLTMenuConfig.Menu import StreamInfo
    for chain in chainDicts:
        # Prepare information for stream list and fill separate dictionary
        chainStreamTags = []
        for streamName in chain["stream"]:
            streamTag = StreamInfo.getStreamTag(streamName)
            # Stream needs to have been defined in StreamInfo.py otherwise is not added to JSON
            if streamTag is None:
                __log.error('Stream %s does not have StreamTags defined excluding from JSON', streamName)
                continue
            # Add stream to the chain
            chainStreamTags.append(streamName)
            # If not already listed, add stream details to stream dictionary
            if streamName not in menuDict["streams"]:
                menuDict["streams"][streamName] = odict([
                    ("name", streamName),
                    ("type", streamTag.type()),
                    ("obeyLB", streamTag.obeysLumiBlock()),
                    ("forceFullEventBuilding", streamTag.forceFullEventBuilding())
                ])

        # Find L1 Threshold information for current chain
        l1Thresholds  = []
        [ l1Thresholds.append(p['L1threshold']) for p in chain['chainParts'] ]

        # Now have all information to write the chain to the menu dictionary
        chainName = chain["chainName"]
        menuDict["chains"][chainName] = odict([
            ("counter", chain["chainCounter"]),
            ("nameHash", chain["chainNameHash"]),
            ("l1item", chain["L1item"]),
            ("l1thresholds", l1Thresholds),
            ("groups", chain["groups"]),
            ("streams", chainStreamTags),
            ("sequencers", __getChainSequencers(stepsData, chainName) )
        ])

    # All algorithms executed by a given Sequencer
    menuDict["sequencers"].update( __getSequencerAlgs(stepsData) )

    # Menu dictionary now completed, write to JSON
    __log.info( "Writing HLT Menu JSON to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( menuDict, fp, indent=4, sort_keys=False )


def generateJSON():
    __log.info("Generating HLT Menu JSON in the rec-ex-common job")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.CFElements import findSubSequence

    return __generateJSON( TriggerConfigHLT.dictsList(), 
                           TriggerConfigHLT.configsList(), 
                           findSubSequence(AlgSequence(), "HLTAllSteps"),
                           ConfigFlags.Trigger.triggerMenuSetup,
                           getHLTMenuFileName(ConfigFlags) )
    
def generateJSON_newJO( chainDicts, chainConfigs, HLTAllSteps ):
    __log.info("Generating HLT Menu JSON in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    return __generateJSON( chainDicts, 
                           chainConfigs, 
                           HLTAllSteps,
                           ConfigFlags.Trigger.triggerMenuSetup, 
                           getHLTMenuFileName( ConfigFlags) )
