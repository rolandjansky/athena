# JetRec_jobOptions.py
#
# David Adams
# February 2014
#
# RecExCommon job options for running jet reconstruction.
#
# Run with
# > athena.py test_RunJetRec.py
#

# Import the jet tool manager.
from JetRec.JetRecStandard import jtm,jetlog
myname = "JetRec_jobOptions.py: "
jetlog.info( myname + "Begin." )
from JetRec.JetRecFlags import jetFlags, JetContentDetail
# jetFlags.separateJetAlgs.set_Value(True)
# jetFlags.timeJetToolRunner.set_Value(2)

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

ptminFilter_topo = 15000
ptminFilter_pFlow = 10000
if not jetFlags.useCalibJetThreshold:
  ptminFilter_topo = 1
  ptminFilter_pFlow = 1
  jetlog.info(myname + "Switching off the jet pT threshold applied at the calibrated scale")

# Finders.
if jetFlags.detailLevel()==JetContentDetail.Reduced:
  if jetFlags.useTopo():
    jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo_reduced", "emtopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=ptminFilter_topo, calibOpt=calibopt)
    jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo_reduced", "lctopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=ptminFilter_topo, calibOpt=calibopt)
    jtm.addJetFinder("AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo_reduced", "lctopo_ungroomed", ghostArea=0.01, ptmin= 40000, ptminFilter=50000, calibOpt="none")
  if jetFlags.usePFlow():
    jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,   "empflow_reduced", "pflow_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter= ptminFilter_pFlow, calibOpt=calibopt+":pflow")
  if jetFlags.useTruth():
    jtm.addJetFinder("AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ptmin= 5000)
elif jetFlags.detailLevel()>=JetContentDetail.Full:
  if jetFlags.useTruth():
    jtm.addJetFinder("AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ptmin= 5000)
    jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000)
    jtm.addJetFinder("AntiKt10TruthJets",   "AntiKt", 1.0,    "truth", ptmin=40000)
    jtm.addJetFinder("AntiKt10TruthWZJets", "AntiKt", 1.0,  "truthwz", ptmin=40000)
  if jetFlags.useTracks():
    jtm.addJetFinder("AntiKt2PV0TrackJets", "AntiKt", 0.2, "pv0track", ptmin= 2000)
    jtm.addJetFinder("AntiKt4PV0TrackJets", "AntiKt", 0.4, "pv0track", ptmin= 2000)
  if jetFlags.useTopo():
    jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "emtopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=ptminFilter_topo, calibOpt=calibopt)
    jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo", "lctopo_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=ptminFilter_topo, calibOpt=calibopt)
    jtm.addJetFinder("AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", "lctopo_ungroomed", ghostArea=0.01, ptmin= 40000, ptminFilter=50000, calibOpt="none")
  if jetFlags.usePFlow():
    jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,   "empflow", "pflow_ungroomed", ghostArea=0.01, ptmin= 5000, ptminFilter=ptminFilter_pFlow, calibOpt=calibopt+":pflow")
if jetFlags.detailLevel()==JetContentDetail.Validation:
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
  if jetrec.Trigger:
    auxprefix = "Trig"
  jetFlags.jetAODList += [ "xAOD::Jet" + auxprefix + "AuxContainer#" + jetrec.name() + "Aux." ]

if jetFlags.useTracks() and jetFlags.useTopo():
    jetFlags.jetAODList += [ "xAOD::CaloClusterContainer#EMOriginTopoClusters" ,
                             "xAOD::CaloClusterContainer#LCOriginTopoClusters" ,
                             "xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.",
                             "xAOD::ShallowAuxContainer#EMOriginTopoClustersAux."] 
    jetFlags.jetAODList += [ "xAOD::PFOContainer#CHSChargedParticleFlowObjects",
                             "xAOD::PFOContainer#CHSNeutralParticleFlowObjects",
                             "xAOD::ShallowAuxContainer#CHSChargedParticleFlowObjectsAux.",
                             "xAOD::ShallowAuxContainer#CHSNeutralParticleFlowObjectsAux."] 

# For testing. These blocks should not be enabled in production.
if jetFlags.debug > 0:
  jetlog.info( myname + "Requested output stream: ")
  jetlog.info( "%s", jetFlags.jetAODList )
if jetFlags.debug > 1:
  jetlog.info( myname + "Setting output level to DEBUG for all jetrecs")
  for jetrec in jtm.jetrecs:
    jtm.setOutputLevel(jetrec, DEBUG)

#--------------------------------------------------------------
# Add jet reco to algorithm sequence.
#--------------------------------------------------------------
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence()

#--------------------------------------------------------------
# save event shapes set with the JetAlgorithm
#--------------------------------------------------------------
for esTool in jtm.allEDTools:
    jetFlags.jetAODList += [ "xAOD::EventShape#"+esTool.OutputContainer,
                             "xAOD::EventShapeAuxInfo#"+esTool.OutputContainer+'Aux.' ]

jetlog.info( myname + "End." )
