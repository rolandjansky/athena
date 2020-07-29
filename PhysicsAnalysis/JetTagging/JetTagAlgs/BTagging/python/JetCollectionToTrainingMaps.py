# The JetBTaggerTool will be configured to run these as modifiers
# before and after the BTagTool. Since it is configured
# per-jet-collection it should be relatively easy to run different
# tunings on different collections.

preTagDL2JetToTrainingMap={
    'AntiKt4EMPFlow': [
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/smt/antikt4empflow/network.json',
    ],
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903': [
        'BTagging/201903/rnnip/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/smt/antikt4empflow/network.json',
    ],
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903GhostTag': [
        'BTagging/201903/rnnip/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/smt/antikt4empflow/network.json',
    ]
}

postTagDL2JetToTrainingMap={
    'AntiKt4EMPFlow': [
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        'BTagging/201903/dl1rmu/antikt4empflow/network.json',
    ],
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903': [
        'BTagging/201903/dl1r/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/dl1/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/dl1rmu/antiktvr30rmax4rmin02track/network.json',
    ],
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903GhostTag': [
        'BTagging/201903/dl1r/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/dl1/antiktvr30rmax4rmin02track/network.json',
        'BTagging/201903/dl1rmu/antiktvr30rmax4rmin02track/network.json',
    ]
}

# in some cases we just want to make sure the b-jets are augmented
# with all the required inputs for b-tagging
btagAugmentedJetCollectionList = [
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810',
    'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810GhostTag',
]

# By default the things called by BTagTool will ignore anything that
# is listed in the two dictionaries above. Unfortunately we'll miss
# collections since we duplicate and rename a few jet collections
# within the JetBTaggerTool.
blacklistedJetCollections=["AntiKt4EMPFlow_BTagging201903"]
