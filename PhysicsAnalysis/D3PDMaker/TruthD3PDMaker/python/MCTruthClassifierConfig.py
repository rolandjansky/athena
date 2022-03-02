# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# @file TruthD3PDMaker/python/MCTruthClassifierConfig.py
# @author scott snyder <snyder@bnl.gov>
# @date Jan, 2010
# @brief Import this to configure the MCTruthClassifier tool for D3PD making.
#

# Create the classifier tool, and set the MC collection name to match
# what's in D3PDMakerFlags.

from D3PDMakerCoreComps.resolveSGKey import resolveSGKey
from D3PDMakerConfig.D3PDMakerFlags  import D3PDMakerFlags
from AthenaCommon.Constants          import ERROR

mckey = resolveSGKey ('DataVector<xAOD::TruthParticle_v1>', D3PDMakerFlags.TruthSGKey())


from TruthD3PDAnalysis import D3PDMCTruthClassifier
from MCTruthClassifier import MCTruthClassifierBase
D3PDMCTruthClassifier = D3PDMCTruthClassifier(
    name='D3PDMCTruthClassifier',
    CaloDetDescrManager = 'CaloDetDescrManager',
    xAODTruthParticleContainerName=mckey,
    ParticleCaloExtensionTool=MCTruthClassifierBase.getMCTruthClassifierCaloExtensionTool(),
    pTNeutralPartCut=1e-3,
    OutputLevel=ERROR,
    partExtrConePhi=0.6,  # 0.4
    partExtrConeEta=0.2,  # 0.2
    ROICone=True)
try:
    D3PDMCTruthClassifier.forceNotUseBremRefitTrk = True
except AttributeError:
    pass

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += D3PDMCTruthClassifier
