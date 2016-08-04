# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool

def declareDefaultTools():

  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm
  from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
  from eflowRec.eflowRecFlags import jobproperties

  # Retriever for pflow objects.
  jtm += RetrievePFOTool("pflowretriever")

  if True == jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction:
    useChargedWeights = True
  else:
    useChargedWeights = False

  useVertices = True
  if False == jetFlags.useVertices:
    useVertices = False

  useTrackVertexTool = False
  if True == jetFlags.useTrackVertexTool:
    useTrackVertexTool = True

  # EM-scale pflow.
  jtm += PFlowPseudoJetGetter(
    "empflowget",
    Label = "EMPFlow",
    OutputContainer = "PseudoJetEMPFlow",
    RetrievePFOTool = jtm.pflowretriever,
    InputIsEM = True,
    CalibratePFO = False,
    SkipNegativeEnergy = True,
    UseChargedWeights = useChargedWeights,
    UseVertices = useVertices,
    UseTrackToVertexTool = useTrackVertexTool
  )

  # Calibrated EM-scale pflow.
  jtm += PFlowPseudoJetGetter(
    "emcpflowget",
    Label = "EMCPFlow",
    OutputContainer = "PseudoJetEMCPFlow",
    RetrievePFOTool = jtm.pflowretriever,
    InputIsEM = True,
    CalibratePFO = True,
    SkipNegativeEnergy = True,
    UseChargedWeights = useChargedWeights,
    UseVertices = useVertices,
    UseTrackToVertexTool = useTrackVertexTool
  )

  # LC-scale pflow.
  jtm += PFlowPseudoJetGetter(
    "lcpflowget",
    Label = "LCPFlow",
    OutputContainer = "PseudoJetLCPFlow",
    RetrievePFOTool = jtm.pflowretriever,
    InputIsEM = False,
    CalibratePFO = False,
    SkipNegativeEnergy = True,
    UseChargedWeights = useChargedWeights,
    UseVertices = useVertices,
    UseTrackToVertexTool = useTrackVertexTool
  )

