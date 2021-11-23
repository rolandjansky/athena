#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#


# They force us to setup this weird jettoolmanager first
# although we don't really (want to) use it afterwards...
from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth=False
jetFlags.useTracks=False
jetFlags.useTopo=True
jetFlags.usePFlow=False
jetFlags.useMuonSegments = False

# Import the jet tool manager.
from JetRec.JetRecStandard import jtm

# schedule N+1 jets - one of them is master and must run first to create 
#  the ParticleJetMaps to which all other jet algorithm append their thing
#  no multiple clusters as input yet

# schedule jetfinders by hand
from JetRec.JetRecConf import JetAlgorithm
import JetRec.JetRecStandardTools
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecStandardToolManager import jtm
from JetRec.JetRecConf import PseudoJetGetter
# from JetRec.JetRecConf import JetToolRunner
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import JetFinder
from JetRec.JetRecConf import JetFromPseudojet

from AthenaCommon.AppMgr import ToolSvc

#   ParticleJetMaps
# schedule one standard jetfinder on which all others depend
# the weirdo "ParticleJetMap thingy should then be existing for all. Still a race condition problem, but much less I hope...
#  Note: add dependencies in the CaloHiveDeps.py

# anti-kt 4 standardjets
jalg4 = JetAlgorithm("jetalg4")
j4 = JetRecTool("jetrectool4",OutputContainer="AntiKt4EMTopoJets")
g4 = PseudoJetGetter(
  "emget4",
  InputContainer = "CaloTopoClusters",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo4",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
  )
f4 = JetFinder("fastjet-4")
f4.JetAlgorithm = "AntiKt"
f4.JetRadius = 0.4
f4.GhostArea = 0.0
f4.PtMin = 10.0
o4=JetFromPseudojet("out4")

ToolSvc += JetRecTool("jetrectool4")
ToolSvc += PseudoJetGetter("emget4")
ToolSvc += JetFinder("fastjet-4")
ToolSvc += JetFromPseudojet("out4")
j4.PseudoJetGetters += [g4]
j4.JetFinder = f4
f4.JetBuilder = o4
jalg4.Tools += [j4]
topSequence += jalg4

jetFlags.jetAODList += [ "xAOD::JetContainer#AntiKt4EMTopoJets" ]
jetFlags.jetAODList += [ "xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux." ]

OutLevel=INFO

for x in range(0, nJets):
  app=str((0.5+x)/10)
  app=app.replace('.', 'p')
  out="AntiKt"+app+"EMTopoJets"
  jalg = JetAlgorithm("jetalg"+app,OutputLevel=OutLevel)
  j = JetRecTool("jetrectool"+app,OutputContainer=out,OutputLevel=OutLevel)
  g = PseudoJetGetter(
    "emget"+app,
    InputContainer = "CaloTopoClusters",
    Label = "EMTopo",
    OutputContainer = "PseudoJetEMTopo"+app,
    SkipNegativeEnergy = True,
    GhostScale = 0.0,
    OutputLevel=OutLevel
    )
  f = JetFinder("fastjet-"+app,OutputLevel=OutLevel)
  f.JetAlgorithm = "AntiKt"
  f.JetRadius = (0.5+x)/10
  f.GhostArea = 0.001
  f.PtMin = 0.01
  
  o=JetFromPseudojet("out"+app,OutputLevel=OutLevel)
  
  ToolSvc += JetRecTool("jetrectool"+app)
  ToolSvc += PseudoJetGetter("emget"+app)
  ToolSvc += JetFinder("fastjet-"+app)
  ToolSvc += JetFromPseudojet("out"+app)
  j.PseudoJetGetters += [g]
  j.JetFinder = f
  f.JetBuilder = o
  jalg.Tools += [j]
  topSequence += jalg

  jetFlags.jetAODList += [ "xAOD::JetContainer#" + out ]
  jetFlags.jetAODList += [ "xAOD::JetAuxContainer#" + out + "Aux." ]

