#Please note that to access LC PFlow container, one should run the below tool to combine the two neutral containers into one
#That then puts a new container in StoreGate, which PhysValPFO_neutral could be updated to access
#from PFlowUtils.PFlowUtilsConf import CombinePFO

from PFODQA.PFODQAConf import PhysValFE  # get plots from FE

#charged Flow Element

PhysValFE_charged=PhysValFE("PhysValFE_charged")
PhysValFE_charged.OutputLevel= WARNING
PhysValFE_charged.DetailLevel= 10
PhysValFE_charged.EnableLumi=False
PhysValFE_charged.FlowElementContainerName="JetETMissChargedParticleFlowObjects"
PhysValFE_charged.useNeutralFE= False
monMan.AthenaMonTools += [ PhysValFE_charged ]


PhysValFE_neutral=PhysValFE("PhysValFE_neutral")
PhysValFE_neutral.OutputLevel= WARNING
PhysValFE_neutral.DetailLevel= 10
PhysValFE_neutral.EnableLumi=False
PhysValFE_neutral.FlowElementContainerName="JetETMissNeutralParticleFlowObjects"
PhysValFE_neutral.useNeutralFE=True
monMan.AthenaMonTools += [ PhysValFE_neutral ]
