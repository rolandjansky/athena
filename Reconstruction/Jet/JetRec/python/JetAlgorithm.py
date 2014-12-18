# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetAlgorithm.py
#
# David Adams
# March 2014
# October 2014: Update to provide a fn that allow specification of alg sequence.
#
# Configure the jet algorithm after the tool manager has been configured.

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
  if separateJetAlgs == None:
    separateJetAlgs = jetFlags.separateJetAlgs()

  # Add the truth tools.
  if useTruth:
    from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
    scheduleCopyTruthParticles(job)

  # Event shape tools.
  evstools = []
  for name in eventShapeTools:
    from EventShapeTools.EventDensityConfig import configEventDensityTool
    if   name == "em":
      jtm += configEventDensityTool("EMTopoEventShape", jtm.emget, 0.4)
      evstools += [jtm.tools["EMTopoEventShape"]]
    elif name == "lc":
      jtm += configEventDensityTool("LCTopoEventShape", jtm.lcget, 0.4)
      evstools += [jtm.tools["LCTopoEventShape"]]
    else:
      print myname + "Invalid event shape key: " + name
      raise Exception

  # Add the tool runner. It runs the jetrec tools.
  rtools = []
  if jetFlags.useCells():
    rtools += [jtm.missingcells]
  if jetFlags.useTracks:
    rtools += [jtm.tracksel]
    rtools += [jtm.tvassoc]
  rtools += jtm.jetrecs
  from JetRec.JetRecConf import JetToolRunner
  jtm += JetToolRunner("jetrun",
           EventShapeTools=evstools,
           Tools=rtools,
           Timer=jetFlags.timeJetToolRunner()
         )
  jetrun = jtm.jetrun

  # Add the algorithm. It runs the jetrec tools.
  from JetRec.JetRecConf import JetAlgorithm

  if jetFlags.separateJetAlgs():
    job += JetAlgorithm("jetalg")
    jetalg = job.jetalg
    jetalg.Tools = [jtm.jetrun]
    for t in rtools:
      jalg = JetAlgorithm("jetalg"+t.name())
      jalg.Tools = [t]
      job+= jalg

  else:
    job += JetAlgorithm("jetalg")
    jetalg = job.jetalg
    jetalg.Tools = [jtm.jetrun]
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
