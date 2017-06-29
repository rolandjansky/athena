# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def declareDefaultTools():
  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm
  from JetRec.JetRecConf import PseudoJetGetter

  #--------------------------------------------------------------
  # Pseudojet builders.
  #--------------------------------------------------------------

  # Prepare a sequence of input constituent modifiers
  from JetRecToolsConfig import ctm
  jtm += ctm.buildConstitModifSequence( "JetConstitSeq_LCOrigin",
                                        OutputContainer='LCOriginTopoClusters',
                                        InputContainer= 'CaloCalTopoClusters',
                                        modList = [  'clus_origin' ] )
  
  jtm += ctm.buildConstitModifSequence( "JetConstitSeq_EMOrigin",
                                        OutputContainer='EMOriginTopoClusters',
                                        InputContainer= 'CaloCalTopoClusters',                                      
                                        modList = [ 'clus_emscale', 'clus_origin' ] )

  jtm += PseudoJetGetter(
    "lcoriginget",
    InputContainer = jtm.JetConstitSeq_LCOrigin.OutputContainer,
    Label = "LCTopoOrigin",
    OutputContainer = jtm.JetConstitSeq_LCOrigin.OutputContainer+"PseudoJet",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )

  jtm += PseudoJetGetter(
    "emoriginget",
    InputContainer = jtm.JetConstitSeq_EMOrigin.OutputContainer,
    Label = "EMTopoOrigin",
    OutputContainer = jtm.JetConstitSeq_EMOrigin.OutputContainer+"PseudoJet",
    SkipNegativeEnergy = True,
    GhostScale = 0.0
  )
