#********************************************************************
# HIGG6D1.py:  reductionConf flag HIGG6D1 in Reco_tf.py
# B. Burghgrave - blake.burghgrave AT cern.ch
#
DAOD_StreamID = 'HIGG6D1'
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

# running on data or MC
from AthenaCommon.GlobalFlags import globalflags
DFisMC = (globalflags.DataSource()=='geant4')

if DFisMC:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()
    #Template Higgs cross section tool for DFHiggs
    from DerivationFrameworkHiggs.TruthCategories import *

print "Hello, my name is {} and I am running on {}".format(DAOD_StreamID, 'MC' if DFisMC else 'Data')

#==============
# SET UP STREAM
#==============
streamName = derivationFlags.WriteDAOD_HIGG6D1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG6D1Stream )
HIGG6D1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG6D1Stream.AcceptAlgs([DAOD_StreamID+"Kernel"])

#============
# Setup tools
#============
# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG6D1ThinningHelper = ThinningHelper( DAOD_StreamID+"ThinningHelper" )

#trigger navigation thinning
import DerivationFrameworkHiggs.HIGG4DxThinning
HIGG6D1ThinningHelper.TriggerChains = DerivationFrameworkHiggs.HIGG4DxThinning.TriggerChains(DAOD_StreamID)
HIGG6D1ThinningHelper.AppendToStream( HIGG6D1Stream )

# thinning tools
thinningTools = DerivationFrameworkHiggs.HIGG4DxThinning.setup(DAOD_StreamID, HIGG6D1ThinningHelper.ThinningSvc(), ToolSvc)

# skimming tools
import DerivationFrameworkHiggs.HIGG4DxSkimming
skimmingTools = DerivationFrameworkHiggs.HIGG4DxSkimming.setup(DAOD_StreamID, ToolSvc)

#augmentation tools
from DerivationFrameworkHiggs.HIGG4DxAugmentation import *
augmentationTools = DerivationFrameworkHiggs.HIGG4DxAugmentation.setup(DAOD_StreamID, ToolSvc)

#slimming tools
import DerivationFrameworkHiggs.HIGG4DxSlimming

#slimming helper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HIGG6D1SlimmingHelper = SlimmingHelper(DAOD_StreamID+"SlimmingHelper")

# jets and calibration
import DerivationFrameworkHiggs.HIGG4DxJets

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
HIGG6D1Sequence = CfgMgr.AthSequencer(DAOD_StreamID+"Sequence")

# augmentation
HIGG6D1Sequence += CfgMgr.DerivationFramework__CommonAugmentation("HIGG4DxCommonAugmentationKernel", AugmentationTools = augmentationTools)

# jet calibration (before skimming)
DerivationFrameworkHiggs.HIGG4DxJets.setup(DAOD_StreamID, HIGG6D1Sequence, HIGG6D1SlimmingHelper)

# skimming
HIGG6D1Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"SkimmingKernel", SkimmingTools = skimmingTools)

# NonPrompt Lepton Tagger
DerivationFrameworkHiggs.HIGG4DxAugmentation.addJetTagNonPromptLepton(DAOD_StreamID, HIGG6D1Sequence)

# thinning
HIGG6D1Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"Kernel", ThinningTools = thinningTools)

# add the private sequence to the main job
DerivationFrameworkJob += HIGG6D1Sequence

# slimming - last position
DerivationFrameworkHiggs.HIGG4DxSlimming.setup(DAOD_StreamID, HIGG6D1Stream, HIGG6D1SlimmingHelper)

