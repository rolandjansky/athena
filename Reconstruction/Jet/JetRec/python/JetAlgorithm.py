# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
  evsDict = {
    "emtopo"   : ("EMTopoEventShape",   jtm.emget),
    "lctopo"   : ("LCTopoEventShape",   jtm.lcget),
    "empflow"  : ("EMPFlowEventShape",  jtm.empflowget),
  }

  if jetFlags.useTracks():
    if jetFlags.useVertices():
      evsDict["emtopo"] = ("EMTopoOriginEventShape",   jtm.emoriginget)
      evsDict["lctopo"] = ("LCTopoOriginEventShape",   jtm.lcoriginget)
    else:
      evsDict["emtopo"] = ("EMTopoOriginEventShape",   jtm.emget)
      evsDict["lctopo"] = ("LCTopoOriginEventShape",   jtm.lcget)
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
        if not IsInInputFile("xAOD::EventShape",toolname):
          jtm += configEventDensityTool(toolname, getter.Label, 0.4)
          jtm.allEDTools += [jtm.tools[toolname]]
    else:
      jetlog.info( myname + "Invalid event shape key: " + evskey )
      raise Exception

  # Add the tool runner. It runs the jetrec tools.
  ctools = []
  # Add the truth tools.
  if useTruth:    
    from JetRec.JetFlavorAlgs import scheduleCopyTruthParticles
    ctools += scheduleCopyTruthParticles()
    
    # build truth jet input :
    ctools += [ jtm.truthpartcopy, jtm.truthpartcopywz ]

  ## if jetFlags.useCells():
  ##   ctools += [jtm.missingcells] commented out : incompatible with trigger : ATR-9696
  if jetFlags.useTracks:
    ctools += [jtm.tracksel, jtm.trackselloose_trackjets]
    if jetFlags.useVertices:
      ctools += [jtm.tvassoc]
   
  # LCOriginTopoClusters and EMOriginTopoClusters are shallow copies
  # of CaloCalTopoClusters.  This means that if CaloCalTopoClusters gets
  # thinned on output, the the two derived containers need to be thinned
  # in the same way, else they'll be corrupted in the output.
  # FIXME: this should be automatic somehow.
  postalgs = []
  thinneg = False
  from RecExConfig.RecFlags import rec
  if rec.doWriteAOD() and not rec.readAOD():
    from ParticleBuilderOptions.AODFlags import AODFlags
    if AODFlags.ThinNegativeEnergyCaloClusters:
      thinneg = True
  
  if jetFlags.useTracks and jetFlags.useVertices:
    if not IsInInputFile("xAOD::CaloClusterContainer","LCOriginTopoClusters"):
      ctools += [jtm.JetConstitSeq_LCOrigin]
      if thinneg:
        from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyCaloClustersAlg
        postalgs.append (ThinNegativeEnergyCaloClustersAlg ('ThinNegLCOriginTopoClusters',
                                                            ThinNegativeEnergyCaloClusters = True,
                                                            CaloClustersKey = 'LCOriginTopoClusters',
                                                            StreamName = 'StreamAOD'))
    if not IsInInputFile("xAOD::CaloClusterContainer","EMOriginTopoClusters"):
      ctools += [jtm.JetConstitSeq_EMOrigin]
      if thinneg:
        from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyCaloClustersAlg
        postalgs.append (ThinNegativeEnergyCaloClustersAlg ('ThinNegEMOriginTopoClusters',
                                                            ThinNegativeEnergyCaloClusters = True,
                                                            CaloClustersKey = 'EMOriginTopoClusters',
                                                            StreamName = 'StreamAOD'))
    if not IsInInputFile("xAOD::PFOContainer","CHSParticleFlowObjects"):
      if not hasattr(job,"jetalgCHSPFlow"):
        ctools += [jtm.JetConstitSeq_PFlowCHS]
        if thinneg:
          from ThinningUtils.ThinningUtilsConf import ThinNegativeEnergyNeutralPFOsAlg
          CHSnPFOsThinAlg = ThinNegativeEnergyNeutralPFOsAlg(
            "ThinNegativeEnergyCHSNeutralPFOsAlg",
            NeutralPFOsKey="CHSNeutralParticleFlowObjects",
            ThinNegativeEnergyNeutralPFOs = True,
            StreamName = 'StreamAOD'
            )
          postalgs.append(CHSnPFOsThinAlg)

  from JetRec.JetRecConf import JetToolRunner
  from JetRec.JetRecConf import JetAlgorithm
  runners = []
  if len(ctools)>0:
    jtm += JetToolRunner("jetconstit",
                         EventShapeTools=[],
                         Tools=ctools,
                         Timer=jetFlags.timeJetToolRunner()
                         )
    job += JetAlgorithm("jetalgConstituents",
                        Tools=[jtm.jetconstit])

  # Add all the PseudoJetAlgorithms now
  # To avoid massive refactoring and to preserve familiarity,
  # kept calling things "getters", but these are already
  # PseudoJetAlgorithms as we eliminated the wrappers
  for getter in jtm.allGetters:
    job += getter

  # Then, add all event shape tools in separate algs
  for evstool in jtm.allEDTools:
    from EventShapeTools.EventShapeToolsConf import EventDensityAthAlg
    job += EventDensityAthAlg("edalg_"+evstool.OutputContainer,EventDensityTool=evstool)

  if separateJetAlgs:

    for t in jtm.jetrecs:
      jalg = JetAlgorithm("jetalg"+t.name(),
                          Tools = [t])
      job += jalg

  else:
    from JetRec.JetRecConf import JetToolRunner
    jtm += JetToolRunner("jetrun",
                         EventShapeTools=[],
                         Tools=rtools+jtm.jetrecs,
                         Timer=jetFlags.timeJetToolRunner()
                         )
    runners += [jtm.jetrun]

  job += JetAlgorithm("jetalg")
  jetalg = job.jetalg
  jetalg.Tools = runners
  if jetFlags.debug > 0:
    # jtm.setOutputLevel(jtm.jetrun, DEBUG)
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

  for postalg in postalgs:
    job += postalg
    
