
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Configure and schedule jet finding algorithms for the jet 
## containers defined in the user's JO fragment
##
## These algorithms must be scheduled *after* event generation and
## any corrections, but *before* the sequence of generator filters


# Convert HepMC event record to xAODTruth for use by jet tools
print 'bedzie konwersja !!'
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
if not hasattr(prefiltSeq, 'xAODCnv'):
    prefiltSeq += xAODMaker__xAODTruthCnvAlg('xAODCnv',WriteTruthMetaData=False)
prefiltSeq.xAODCnv.AODContainerName = 'GEN_EVENT'

# Turn off jet tools specific to detector-level jets
print 'bedzie wylaczenie !!'
from JetRec.JetRecFlags import jetFlags
jetFlags.useTracks = False
jetFlags.eventShapeTools = []
jetFlags.useTopo = False
jetFlags.useMuonSegments = False
if hasattr( jetFlags, 'usePFlow' ): jetFlags.usePFlow = False
elif hasattr( jetFlags, 'usePflow'): jetFlags.usePflow = False
else: raise NameError('jetFlags has no attribute usePflow or usePFlow') 
jetFlags.useCaloQualityTool = False

# Attach jet algorithms
print 'podlacz algorytmy jetowe !!'
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
if not hasattr(prefiltSeq, 'jetalg'):
    addJetRecoToAlgSequence(prefiltSeq)
