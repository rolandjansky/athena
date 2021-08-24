# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

class _BTaggingFlags:

# default is analysis


    DoNotSetupBTagging = False # WOUTER: This is a temporary measure. It allows the JetRec people to test setting up b-tagging from their end without our job options setting it also up (and therefore ending up with a double setup). Do not switch to True unless you know what you are doing.

    # WOUTER: These govern the adding of jet collections to lists that decide which containers get persistified.
    OutputFilesPrefix = "BTagging_"
    OutputFilesSVname = "SecVtx"
    OutputFilesJFVxname = "JFVtx"
    OutputFilesBaseName = "xAOD::BTaggingContainer#"
    OutputFilesBaseAuxName = "xAOD::BTaggingAuxContainer#"
    OutputFilesBaseNameSecVtx = "xAOD::VertexContainer#"
    OutputFilesBaseAuxNameSecVtx = "xAOD::VertexAuxContainer#"
    OutputFilesBaseNameJFSecVtx = "xAOD::BTagVertexContainer#"
    OutputFilesBaseAuxNameJFSecVtx = "xAOD::BTagVertexAuxContainer#"

    btaggingAODList = [ ]

    btaggingESDList = [ ]

    DoJetHitAssociation = False

    def __init__ (self):

      self.btaggingAODList = list()
      self.btaggingESDList = list()

      # Jet collections not in this list (sans -Jets), and which do not have a calibration
      # alias to one of these, will print a warning during the configuration. This is to help
      # people trying to setup jet collections which we do not support (these would sometimes
      # simply crash during execution without a helpful error remark).
      # The function which uses this list is JetCollectionIsSupported() in
      # ./python/BTaggingConfiguration.py
      self.SupportedJetCollections = ["AntiKt4LCTopo",
                                      "AntiKt4EMTopo",
                                      "AntiKt4Track",
                                      "AntiKt4EMPFlow",
                                      "AntiKt10TruthWZ",
                                      "AntiKt4TruthWZ",
                                      ]


BTaggingFlags = _BTaggingFlags()
