#Please note that to access LC PFlow container, one should run the below tool to combine the two neutral containers into one
#That then puts a new container in StoreGate, which PhysValPFO_neutral could be updated to access
#from PFlowUtils.PFlowUtilsConf import CombinePFO

from PFODQA.PFODQAConf import PhysValPFO # get plots from PFO
from PFODQA.PFODQAConf import PhysValFE  # get plots from FE (and PFO comparison)
# charged PFO
PhysValPFO_charged = PhysValPFO("PhysValPFO_charged")
PhysValPFO_charged.OutputLevel = WARNING
PhysValPFO_charged.DetailLevel = 10
PhysValPFO_charged.EnableLumi = False
PhysValPFO_charged.PFOContainerName="JetETMissChargedParticleFlowObjects"
monMan.AthenaMonTools += [ PhysValPFO_charged ]

# neutral PFO
PhysValPFO_neutral_EM = PhysValPFO("PhysValPFO_neutral")
PhysValPFO_neutral_EM.OutputLevel = WARNING
PhysValPFO_neutral_EM.DetailLevel = 10
PhysValPFO_neutral_EM.EnableLumi = False
PhysValPFO_neutral_EM.PFOContainerName="JetETMissNeutralParticleFlowObjects"
PhysValPFO_neutral_EM.useNeutralPFO = True
monMan.AthenaMonTools += [ PhysValPFO_neutral_EM ]

#charged Flow Element

PhysValFE_charged=PhysValFE("PhysValFE_charged")
PhysValFE_charged.OutputLevel= WARNING
PhysValFE_charged.DetailLevel= 10
PhysValFE_charged.EnableLumi=False
PhysValFE_charged.FlowElementContainerName="JetETMissChargedFlowElements"
PhysValFE_charged.PFOContainerName="JetETMissChargedParticleFlowObjects"
PhysValFE_charged.useNeutralFE= False
monMan.AthenaMonTools += [ PhysValFE_charged ]


PhysValFE_neutral=PhysValFE("PhysValFE_neutral")
PhysValFE_neutral.OutputLevel= WARNING
PhysValFE_neutral.DetailLevel= 10
PhysValFE_neutral.EnableLumi=False
PhysValFE_neutral.FlowElementContainerName="JetETMissNeutralFlowElements"
PhysValFE_neutral.PFOContainerName="JetETMissNeutralParticleFlowObjects"
PhysValFE_neutral.useNeutralFE=True
monMan.AthenaMonTools += [ PhysValFE_neutral ]
