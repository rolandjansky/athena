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

myname = "JetRec_jobOptions: "
print myname + "Begin."

# from JetRec.JetRecFlags import jetFlags
# jetFlags.separateJetAlgs.set_Value(True)
# jetFlags.timeJetToolRunner.set_Value(2)

# Import the jet tool manager.
from JetRec.JetRecStandard import jtm

#--------------------------------------------------------------
# Define the finders and groomers.
# Each line configures a finder or groomer and its associated jetrec tool.
# The first argument is the name of the output collection and the jetrec tool.
# The fifth argument is the list of modifiers.
# Non-zero ghostArea enables calculation of active area.
#--------------------------------------------------------------

# Finders.
if jetFlags.useTruth():
  jtm.addJetFinder("AntiKt4TruthJets",    "AntiKt", 0.4,    "truth", ptmin= 5000)
  jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000)
  jtm.addJetFinder("AntiKt10TruthJets",   "AntiKt", 1.0,    "truth", ptmin=40000)
  jtm.addJetFinder("AntiKt10TruthWZJets", "AntiKt", 1.0,  "truthwz", ptmin=40000)
#  jtm.addJetFinder("CamKt12TruthJets",     "CamKt", 1.2,    "truth", ptmin=40000)
#  jtm.addJetFinder("CamKt12TruthWZJets",   "CamKt", 1.2,  "truthwz", ptmin=40000)
if jetFlags.useTracks():
  jtm.addJetFinder("AntiKt2PV0TrackJets", "AntiKt", 0.2, "pv0track", ptmin= 2000)
#  jtm.addJetFinder("AntiKt3PV0TrackJets", "AntiKt", 0.3, "pv0track", ptmin= 2000)
  jtm.addJetFinder("AntiKt4PV0TrackJets", "AntiKt", 0.4, "pv0track", ptmin= 2000)
if jetFlags.useTopo():
  jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter= 5000, calibOpt="ar")
  jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter= 7000, calibOpt="ar")
  jtm.addJetFinder("AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter=50000, calibOpt="none")
#  jtm.addJetFinder("CamKt12LCTopoJets",    "CamKt", 1.2,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter=50000, calibOpt="none")
if jetFlags.usePFlow():
  jtm.addJetFinder("AntiKt4EMPFlowJets",  "AntiKt", 0.4,  "empflow", "pflow", ghostArea=0.01, ptmin= 2000, ptminFilter= 5000, calibOpt="a:pflow")

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
    jetFlags.jetAODList += [ "xAOD::CaloClusterContainer#EMOriginTopoClusters" , "xAOD::CaloClusterContainer#LCOriginTopoClusters" ,
                             "xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.", "xAOD::ShallowAuxContainer#EMOriginTopoClustersAux."] 



# For testing. These blocks should not be enabled in production.
if jetFlags.debug > 0:
  jetlog.info( "JetRec_jobOptions.py: Requested output stream: ")
  jetlog.info( "%s", jetFlags.jetAODList )
if jetFlags.debug > 1:
  jetlog.info( "JetRec_jobOptions.py: Setting output level to DEBUG for all jetrecs")
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
for esTool in jtm.jetrun.EventShapeTools :
    t = getattr(ToolSvc, esTool.getName() )
    jetFlags.jetAODList += [ "xAOD::EventShape#"+t.OutputContainer,
                             "xAOD::EventShapeAuxInfo#"+t.OutputContainer+'Aux.' ]

print myname + "Begin."
