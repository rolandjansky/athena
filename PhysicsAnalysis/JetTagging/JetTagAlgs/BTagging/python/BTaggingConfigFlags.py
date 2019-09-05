# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createBTaggingConfigFlags():
    btagcf = AthConfigFlags()

    btagcf.addFlag("BTagging.run2TaggersList", ['IP2D','IP3D','SV1','SoftMu','JetFitterNN','MV2c10','MV2c10mu','MV2c10rnn','MV2c100','MV2cl100','RNNIP','DL1','DL1mu','DL1rnn','JetVertexCharge']) 
    #Do we really need this in AthConfigFlags?
    #Comments in BTaggingConfiguration.py
    btagcf.addFlag("BTagging.OutputFiles.Prefix", "BTagging_")
    btagcf.addFlag("BTagging.GeneralToolSuffix",'') #Not sure it will stay like that later on. Was '', 'Trig, or 'AODFix'

    return btagcf
