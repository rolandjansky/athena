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

myname = "JetRec_jobOptions:"

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags
from RecExConfig.ObjKeyStore import cfgKeyStore
from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# Skip truth if rec says it is absent.
if not rec.doTruth():
  jetFlags.useTruth = False 
jetlog.info( " Truth enabled ? jetFlags.useTruth == %s", jetFlags.useTruth() )

#skip track if not built or not present in the job
if not rec.doInDet()  \
   and ( not cfgKeyStore.isInTransient("xAOD::VertexContainer_v1","PrimaryVertices") \
         or not cfgKeyStore.isInTransient('xAOD::TrackParticleContainer_v1','InDetTrackParticles') ):
    jetFlags.useTracks = False
jetlog.info( " Tracks enabled ? jetFlags.useTracks == %s", jetFlags.useTracks() )

#skip muon segment if not built
if not rec.doMuon() or not rec.doMuonCombined() :
    jetFlags.useMuonSegments = False
    print rec.doMuon() , rec.doMuonCombined()
jetlog.info( " MuonSegments enabled ? jetFlags.useMuonSegments == %s", jetFlags.useMuonSegments() )

#skip cluster  if not built
if not rec.doCalo():
    jetFlags.useTopo = False
jetlog.info( " TopCluster enabled ? jetFlags.useTopo == %s", jetFlags.useTopo() )
    


# The following can be used to exclude tools from reconstruction.
if 0:
  jetFlags.skipTools = ["comshapes"]
jetlog.info( "Skipped tools: %s", jetFlags.skipTools())

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
  jtm.addJetFinder("CamKt12TruthJets",    "AntiKt", 1.2,    "truth", ptmin=40000)
  jtm.addJetFinder("CamKt12TruthWZJets",  "AntiKt", 1.2,  "truthwz", ptmin=40000)
if jetFlags.useTracks():
  jtm.addJetFinder("AntiKt3PV0TrackJets", "AntiKt", 0.3, "pv0track", ptmin= 2000)
  jtm.addJetFinder("AntiKt3ZTrackJets",   "AntiKt", 0.3,   "ztrack", ptmin= 2000)
  jtm.addJetFinder("AntiKt4PV0TrackJets", "AntiKt", 0.4, "pv0track", ptmin= 2000)
  jtm.addJetFinder("AntiKt4ZTrackJets",   "AntiKt", 0.4,   "ztrack", ptmin= 2000)
if jetFlags.useTopo():
  jtm.addJetFinder("AntiKt4EMTopoJets",   "AntiKt", 0.4,   "emtopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter= 5000)
  jtm.addJetFinder("AntiKt4LCTopoJets",   "AntiKt", 0.4,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter= 7000)
  jtm.addJetFinder("AntiKt10LCTopoJets",  "AntiKt", 1.0,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter=50000)
  jtm.addJetFinder("CamKt12LCTopoJets",    "CamKt", 1.2,   "lctopo", "calib", ghostArea=0.01, ptmin= 2000, ptminFilter=50000)

#--------------------------------------------------------------
# Build output container list.
#--------------------------------------------------------------

# Both standard and aux container must be listed explicitly.
# For release 19, the container version must be explicit.

for jetrec in jtm.jetrecs:
  jetFlags.jetAODList += [ "xAOD::JetContainer_v1#" + jetrec.name() ]
  auxprefix = ""
  if jetrec.Trigger:
    auxprefix = "Trig"
  jetFlags.jetAODList += [ "xAOD::Jet" + auxprefix + "AuxContainer_v1#" + jetrec.name() + "Aux." ]

# For testing. These blocks should not be enabled in production.
if jetFlags.debug > 0:
  jetlog.info( "JetRec_jobOptions.py: Requested output stream: ")
  jetlog.info( "%s", jetFlags.jetAODList )
if jetFlags.debug > 1:
  jetlog.info( "JetRec_jobOptions.py: Setting output level to DEBUG for all jetrecs")
  for jetrec in jtm.jetrecs:
    jtm.setOutputLevel(jetrec, DEBUG)

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------
import JetRec.JetAlgorithm 

