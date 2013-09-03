# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

Algorithm("TriggerDecisionMaker").OutputLevel=DEBUG
MessageSvc.OutputLevel = INFO

#theApp.Dlls  += ["AnalysisTools"]
#theApp.Dlls += ["EventViewBuilderUtils"]
#theApp.Dlls += ["EventViewBuilderAlgs"]
#theApp.Dlls += ["EventViewInserters"]

theApp.TopAlg += ["EVMultipleOutputToolLooper/triggerEV"]

triggerEVAlg = Algorithm( "triggerEV" )

triggerEVAlg.EventViewCollectionOutputName="triggerEV"

triggerEVAlg.EventViewTools=[
    "EVEventInfoUserData"
]

triggerEVAlg.OutputLevel=INFO

include( "TrigEventAthenaPool/TrigEventAthenaPool_joboptions.py" )
include( "TrigSteeringTestAthenaPool/TrigSteeringTestAthenaPool_joboptions.py" )

triggerEVAlg.EventViewTools += ["EVTriggerDecisionUserData","EVL1TriggerUserData"]

triggerEVAlg.EVTriggerDecisionUserData.TriggerDecisionKey="MyTriggerDecision+"

triggerEVAlg.EVTriggerDecisionUserData.TriggerNames=[
