# JetRec standalone reconstruction jobOptions
import AthenaPoolCnvSvc.ReadAthenaPool

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

from AthenaCommon.AlgScheduler import AlgScheduler
AlgScheduler.OutputLevel( INFO )
AlgScheduler.ShowControlFlow( True )
AlgScheduler.ShowDataDependencies( True )

from JetRec.JetRecStandard import jtm, jetFlags
jetFlags.debug=3

#--------------------------------------------------------------
# Define the finders and groomers.
# Each line configures a finder or groomer and its associated jetrec tool.
# The first argument is the name of the output collection and the jetrec tool.
# The fifth argument is the list of modifiers.
# Non-zero ghostArea enables calculation of active area.
#--------------------------------------------------------------

calibopt = "arj"
if not jetFlags.useVertices():
  calibopt = "aj"
  jetlog.info(myname + "No vertices -- switch calibopt to " + calibopt)

jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo_reduced", "emtopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= 15000, calibOpt=calibopt)
jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo_reduced", "lctopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= 15000, calibOpt=calibopt)
jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,   "empflow_reduced", "pflow_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= 10000, calibOpt=calibopt+":pflow")
if jetFlags.useTruth():
    jtm.addJetFinder("AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ptmin= 5000)
    jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000)
if jetFlags.useTracks():
    jtm.addJetFinder("AntiKt2PV0TrackJets", "AntiKt", 0.2, "pv0track", ptmin= 2000)
jtm.addJetFinder("AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo_reduced", "lctopo_ungroomed", ghostArea=0.01, ptmin= 40000, ptminFilter=50000, calibOpt="none")
jtm.addJetTrimmer( "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                   rclus=0.2, ptfrac=0.05, input="AntiKt10LCTopoJets", modifiersin="lctopo_groomed" )


#--------------------------------------------------------------
# Build output container list.
#--------------------------------------------------------------

# Both standard and aux container must be listed explicitly.
# For release 19, the container version must be explicit.

for jetrec in jtm.jetrecs:
  jetFlags.jetAODList += [ "xAOD::JetContainer#" + jetrec.name() ]
  auxprefix = ""

if jetFlags.useTracks() and jetFlags.useTopo():
    jetFlags.jetAODList += [ "xAOD::CaloClusterContainer#EMOriginTopoClusters" ,
                             "xAOD::CaloClusterContainer#LCOriginTopoClusters" ,
                             "xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.",
                             "xAOD::ShallowAuxContainer#EMOriginTopoClustersAux."] 
    jetFlags.jetAODList += [ "xAOD::PFOContainer#CHSChargedParticleFlowObjects",
                             "xAOD::PFOContainer#CHSNeutralParticleFlowObjects",
                             "xAOD::ShallowAuxContainer#CHSChargedParticleFlowObjectsAux.",
                             "xAOD::ShallowAuxContainer#CHSNeutralParticleFlowObjectsAux."] 

#--------------------------------------------------------------
# Add jet reco to algorithm sequence.
#--------------------------------------------------------------
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence()

#--------------------------------------------------------------
# save event shapes set with the JetAlgorithm
#--------------------------------------------------------------
for esTool in jtm.allEDTools:
    t = getattr(ToolSvc, esTool.getName() )
    jetFlags.jetAODList += [ "xAOD::EventShape#%s" % t.OutputContainer,
                             "xAOD::EventShapeAuxInfo#%sAux." % t.OutputContainer ]

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
for item in jetFlags.jetAODList():
    xaodStream.AddItem(item)


