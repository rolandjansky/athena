# The JetBTaggerTool will be configured to run these as modifiers
# before and after the BTagTool. Since it is configured
# per-jet-collection it should be relatively easy to run different
# tunings on different collections.

preTagDL2JetToTrainingMap={
    'AntiKt4EMPFlow': [
        'flavtag/april2019/rnnip/antikt4empflow/network.json',
        'flavtag/april2019/smt/antikt4empflow/network.json',
    ]
}
postTagDL2JetToTrainingMap={
    'AntiKt4EMPFlow': [
        'flavtag/april2019/dl1r/antikt4empflow/network.json',
        'flavtag/april2019/dl1/antikt4empflow/network.json',
        'flavtag/april2019/dl1rmu/antikt4empflow/network.json'
    ]
}

# By default the things called by BTagTool will ignore anything that
# is listed in the two dictionaries above. Unfortunately we'll miss
# collections since we duplicate and rename a few jet collections
# within the JetBTaggerTool.
blacklistedJetCollections=["AntiKt4EMPFlow_BTagging201903"]
