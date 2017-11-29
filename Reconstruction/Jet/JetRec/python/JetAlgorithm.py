# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetAlgorithm.py
#
# David Adams
# March 2014
# October 2014: Update to provide a fn that allow specification of alg sequence.
#
# Configure the jet algorithm after the tool manager has been configured.

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetRec_jobOptions')

# Record the jet algorithm here.
# Retrieve this with "from JetRec.JetAlgorithm import jetalg" *after*
# calling addJetRecoToAlgSequence().
jetalg = None

# Function to add jet reconstruction to an algorithm sequence
#               job: algorithm sequence
#          useTruth: Flag to schedule building of selected-truth containers
#   eventShapeTools: Keys for the event shape tools to be run
#   separateJetAlgs: Run JetRecTools in separate algs (experts only)
#             debug: Debug level (0 for quiet). See below.
def addJetRecoToAlgSequence(job =None, useTruth =None, eventShapeTools =None,
                            separateJetAlgs= None, debug =None):

  myname = "JetAlgorithm: "

  # We need this to modify the global variable.
  global jetalg

  # Import message level flags.
  from GaudiKernel.Constants import DEBUG

  # Import the jet reconstruction control flags.
  from JetRec.JetRecFlags import jetFlags

  # Import the standard jet tool manager.
  from JetRec.JetRecStandard import jtm

  # Set sequence and flags as needed.
  if job == None:
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
  if useTruth == None:
    useTruth = jetFlags.useTruth()
  if eventShapeTools == None:
    eventShapeTools = jetFlags.eventShapeTools()
    if eventShapeTools == None:
      eventShapeTools = []
  if separateJetAlgs == None:
    separateJetAlgs = jetFlags.separateJetAlgs()


  # Event shape tools.
  evstools = []
  evsDict = {
    "emtopo"   : ("EMTopoEventShape",   jtm.emget),
    "lctopo"   : ("LCTopoEventShape",   jtm.lcget),
    "empflow"  : ("EMPFlowEventShape",  jtm.empflowget),
  }

  if jetFlags.useTracks():
    evsDict["emtopo"] = ("EMTopoOriginEventShape",   jtm.emoriginget)
    evsDict["lctopo"] = ("LCTopoOriginEventShape",   jtm.lcoriginget)
  jetlog.info( myname + "Event shape tools: " + str(eventShapeTools) )

  from RecExConfig.AutoConfiguration import IsInInputFile
  for evskey in eventShapeTools:
    from EventShapeTools.EventDensityConfig import configEventDensityTool
    if evskey in evsDict:
      (toolname, getter) = evsDict[evskey]
      if toolname in jtm.tools:
        jetlog.info( myname + "Skipping duplicate event shape: " + toolname )
      else:
        jetlog.info( myname + "Adding event shape " + evskey )
        if not IsInInputFile("xAOD::EventShape","Kt4"+toolname):
          jtm += configEventDensityTool(toolname, getter, 0.4)
          evstools += [jtm.tools[toolname]]
    else:
      jetlog.info( myname + "Invalid event shape key: " + evskey )
      raise Exception

  # Add the tool runner. It runs the jetrec tools.
  rtools = []
  # Add the truth tools.
  if useTruth:    
    from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
    rtools += scheduleCopyTruthParticles()
    
    # build truth jet input :
    rtools += [ jtm.truthpartcopy, jtm.truthpartcopywz ]

  ## if jetFlags.useCells():
  ##   rtools += [jtm.missingcells] commented out : incompatible with trigger : ATR-9696
  if jetFlags.useTracks:
    rtools += [jtm.tracksel,
               jtm.tvassoc,
               jtm.trackselloose_trackjets,
               ]
   
  # Add the algorithm. It runs the jetrec tools.
  from JetRec.JetRecConf import JetAlgorithm
  ctools = []
  if jetFlags.useTracks:
    if not IsInInputFile("xAOD::CaloClusterContainer","LCOriginTopoClusters"):
      ctools += [jtm.JetConstitSeq_LCOrigin]
    if not IsInInputFile("xAOD::CaloClusterContainer","EMOriginTopoClusters"):
      ctools += [jtm.JetConstitSeq_EMOrigin]
    if not IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects"):
      if not hasattr(job,"jetalgCHSPFlow"):
        ctools += [jtm.JetConstitSeq_PFlowCHS]
  #ctools += constitModTools
  from JetRec.JetRecConf import JetToolRunner
  runners = []
  if len(ctools)>0:
    jtm += JetToolRunner("jetconstit",
                         EventShapeTools=[],
                         Tools=ctools,
                         Timer=jetFlags.timeJetToolRunner()
                         )
    jtm.jetconstit
    runners = [jtm.jetconstit]

  if separateJetAlgs:

    jtm += JetToolRunner("jetrun",
                         EventShapeTools=evstools,
                         Tools=rtools,
                         Timer=jetFlags.timeJetToolRunner()
                         )
    runners += [jtm.jetrun]

    job += JetAlgorithm("jetalg")
    jetalg = job.jetalg
    jetalg.Tools = runners

    for t in jtm.jetrecs:
      jalg = JetAlgorithm("jetalg"+t.name())
      jalg.Tools = [t]
      job+= jalg

  else:
    from JetRec.JetRecConf import JetToolRunner
    jtm += JetToolRunner("jetrun",
                         EventShapeTools=evstools,
                         Tools=rtools+jtm.jetrecs,
                         Timer=jetFlags.timeJetToolRunner()
                         )
    runners += [jtm.jetrun]

    job += JetAlgorithm("jetalg")
    jetalg = job.jetalg
    jetalg.Tools = runners
    if jetFlags.debug > 0:
      jtm.setOutputLevel(jtm.jetrun, DEBUG)
      jetalg.OutputLevel = DEBUG
    if jetFlags.debug > 1:
      for tool in jtm.jetrecs:
        jtm.setOutputLevel(tool, DEBUG)
    if jetFlags.debug > 2:
      for tool in jtm.finders:
        jtm.setOutputLevel(tool, DEBUG)
    if jetFlags.debug > 3:
      jtm.setOutputLevel(jtm.jetBuilderWithArea, DEBUG)
      jtm.setOutputLevel(jtm.jetBuilderWithoutArea, DEBUG)
