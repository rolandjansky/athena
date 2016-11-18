# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
from PFlowUtils.PFlowUtilsConf import CP__WeightPFOTool as WeightPFOTool

def declareDefaultTools():

  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm
  from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
  from eflowRec.eflowRecFlags import jobproperties

  # Retriever for pflow objects.
  jtm += RetrievePFOTool("pflowretriever")

  # Weight tool for charged pflow objects.
  jtm += WeightPFOTool("pflowweighter")
  jtm += WeightPFOTool("pflowweighter_LC",NeutralPFOScale="LC")

  useVertices = True
  if False == jetFlags.useVertices:
    useVertices = False

  if True == jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction:
    useChargedWeights = True
  else:
    useChargedWeights = False

  useTrackVertexTool = False
  if True == jetFlags.useTrackVertexTool:
    useTrackVertexTool = True

  # EM-scale pflow.
  jtm += PFlowPseudoJetGetter(
    "empflowget",
    Label = "EMPFlow",
    OutputContainer = "PseudoJetEMPFlow",
    RetrievePFOTool = jtm.pflowretriever,
    WeightPFOTool = jtm.pflowweighter,
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
    WeightPFOTool = jtm.pflowweighter_LC,
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
    WeightPFOTool = jtm.pflowweighter_LC,
    InputIsEM = False,
    CalibratePFO = False,
    SkipNegativeEnergy = True,
    UseChargedWeights = useChargedWeights,
    UseVertices = useVertices,
    UseTrackToVertexTool = useTrackVertexTool
  )
