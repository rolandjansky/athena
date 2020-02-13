# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

import json
from collections import OrderedDict as odict
from TrigConfigSvc.TrigConfigSvcCfg import getHLTMenuFileName

from AthenaCommon.Logging import logging
__log = logging.getLogger( __name__ )

def __getStepsDataFromAlgSequence(HLTAllSteps):
    """ Generates a list where the index corresponds to a Step number and the stored object is a list of Sequencers making up the Step 
    """
    stepsData = []
    for HLTStep in HLTAllSteps.getChildren():
        if "HLTAllSteps_Step" not in HLTStep.name(): # Avoid the pre-step Filter execution
            continue
        stepsData.append( HLTStep.getChildren() )
    return stepsData

def __getChainSequencers(stepsData, chainName):
    """ Finds the Filter which is responsible for this Chain in each Step.
        Return a list of the per-Step name() of the Sequencer which is unlocked by the Chain's Filter in the Step.
    """
    sequencers = []
    counter = 0
    for step in stepsData:
        counter += 1
        mySequencer = None
        endOfChain = False
        for sequencer in step:
            sequencerFilter = sequencer.getChildren()[0] # Always the first child in the step
            if chainName in sequencerFilter.Chains:
                if mySequencer is not None:
                    __log.error( "Multiple Filters found (corresponding Sequencers %s, %s) for %s in Step %i!",
                        mySequencer.name(), sequencer.name(), chainName, counter)
                mySequencer = sequencer
        if mySequencer is None:
            endOfChain = True
        else:
            if endOfChain is True:
                __log.error( "Found another Step, (Step %i) for chain %s "
                    "which looked like it had already finished after %i Steps!", 
                    counter, chainName, sequencers.len())
            sequencers.append(mySequencer.name())
    return sequencers

def __getSequencerAlgs(stepsData):
    """ For each Sequencer in each Step, return a flat list of the full name of all Algorithms under the Sequencer
    """
    from AthenaCommon.CFElements import findAllAlgorithms
    sequencerAlgs = {}
    for step in stepsData:
        for sequencer in step:
            sequencerAlgs[ sequencer.name() ] = list(map(lambda x: x.getFullName(), findAllAlgorithms(sequencer)))
    return sequencerAlgs

def __generateJSON( chainDicts, chainConfigs, HLTAllSteps, menuName, fileName ):
    """ Generates JSON given the ChainProps and sequences
    """
    menuDict = odict([ ("filetype", "hltmenu"), ("name", menuName), ("chains", []), ("sequencers", []) ])

    # Dictionary of { Step Number: { Filter: Sequencer } }
    stepsData = __getStepsDataFromAlgSequence(HLTAllSteps)

    from TriggerMenuMT.HLTMenuConfig.Menu import StreamInfo
    for chain in chainDicts:
        streamDicts = []
        streamTags = StreamInfo.getStreamTags(chain["stream"])
        for streamTag in streamTags:
            streamDicts.append({"name": streamTag.name(),
                                "type": streamTag.type(),
                                "obeyLB": streamTag.obeysLumiBlock(),
                                "forceFullEventBuilding": streamTag.forceFullEventBuilding()})

        l1Thresholds  = []
        [ l1Thresholds.append(p['L1threshold']) for p in chain['chainParts'] ]

        chainDict = odict([ 
            ("counter", chain["chainCounter"]),
            ("name", chain["chainName"]),
            ("nameHash", chain["chainNameHash"]),
            ("l1item", chain["L1item"]),
            ("l1thresholds", l1Thresholds),
            ("groups", chain["groups"]),
            ("streams", streamDicts),
            ("sequencers", __getChainSequencers(stepsData, chain["chainName"]) )
        ])

        menuDict["chains"].append( chainDict )

    # All algorithms executed by a given Sequencer
    menuDict["sequencers"].append( __getSequencerAlgs(stepsData) )

    __log.info( "Writing trigger menu to %s", fileName )
    with open( fileName, 'w' ) as fp:
        json.dump( menuDict, fp, indent=4, sort_keys=False )

def generateJSON():
    __log.info("Generating HLT JSON config in the rec-ex-common job")
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from TriggerMenuMT.HLTMenuConfig.Menu.TriggerConfigHLT import TriggerConfigHLT
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.CFElements import findSubSequence

    return __generateJSON( TriggerConfigHLT.dictsList(), 
                           TriggerConfigHLT.configsList(), 
                           findSubSequence(AlgSequence(), "HLTAllSteps"),
                           TriggerFlags.triggerMenuSetup(),
                           getHLTMenuFileName() )
    
def generateJSON_newJO( chainDicts, chainConfigs, HLTAllSteps ):
    __log.info("Generating HLT JSON config in the new JO")
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    return __generateJSON( chainDicts, 
                           chainConfigs, 
                           HLTAllSteps,
                           ConfigFlags.Trigger.triggerMenuSetup, 
                           getHLTMenuFileName( ConfigFlags) )
