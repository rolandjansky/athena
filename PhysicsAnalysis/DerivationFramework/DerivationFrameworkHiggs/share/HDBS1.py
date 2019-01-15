#********************************************************************
# HDBS1.py:  reductionConf flag HDBS1 in Reco_tf.py
# N. Tamir - mail@cern.ch
#
DAOD_StreamID = 'HDBS1'
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
    from DerivationFrameworkMCTruth.MCTruthCommon import *
    #Template Higgs cross section tool for DFHiggs
    from DerivationFrameworkHiggs.TruthCategories import *

print "Hello, my name is {} and I am running on {}".format(DAOD_StreamID, 'MC' if DFisMC else 'Data')

#==============
# SET UP STREAM
#==============
streamName = derivationFlags.WriteDAOD_HDBS1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HDBS1Stream )
HDBS1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HDBS1Stream.AcceptAlgs([DAOD_StreamID+"Kernel"])

#============
# Setup tools
#============
# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HDBS1ThinningHelper = ThinningHelper( DAOD_StreamID+"ThinningHelper" )

#trigger navigation thinning
import DerivationFrameworkHiggs.HIGG4DxThinning
HDBS1ThinningHelper.TriggerChains = DerivationFrameworkHiggs.HIGG4DxThinning.TriggerChains(DAOD_StreamID)
HDBS1ThinningHelper.AppendToStream( HDBS1Stream )

# thinning tools
thinningTools = DerivationFrameworkHiggs.HIGG4DxThinning.setup(DAOD_StreamID, HDBS1ThinningHelper.ThinningSvc(), ToolSvc)

# skimming tools
import DerivationFrameworkHiggs.HIGG4DxSkimming
skimmingTools = DerivationFrameworkHiggs.HIGG4DxSkimming.setup(DAOD_StreamID, ToolSvc)
fatJetSkimmingTools = DerivationFrameworkHiggs.HIGG4DxSkimming.setupFatJetSkim(DAOD_StreamID, ToolSvc)

#augmentation tools
from DerivationFrameworkHiggs.HIGG4DxAugmentation import *
augmentationTools = DerivationFrameworkHiggs.HIGG4DxAugmentation.setup(DAOD_StreamID, ToolSvc)

#slimming tools
import DerivationFrameworkHiggs.HIGG4DxSlimming

#slimming helper
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
HDBS1SlimmingHelper = SlimmingHelper(DAOD_StreamID+"SlimmingHelper")

# jets and calibration
import DerivationFrameworkHiggs.HIGG4DxJets

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
HDBS1Sequence = CfgMgr.AthSequencer(DAOD_StreamID+"Sequence")

# augmentation
HDBS1Sequence += CfgMgr.DerivationFramework__CommonAugmentation("HIGG4DxCommonAugmentationKernel", AugmentationTools = augmentationTools)

# skimming #1: based on di-taus and trigger
HDBS1Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"SkimmingKernel", SkimmingTools = skimmingTools)

# fat/trimmed jet building (after skimming)
DerivationFrameworkHiggs.HIGG4DxJets.setup(DAOD_StreamID, HDBS1Sequence, HDBS1SlimmingHelper)

# thinning + skimming #2 based on fat jets (now we have them created)
HDBS1Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"Kernel", ThinningTools = thinningTools, SkimmingTools = fatJetSkimmingTools)

# add the private sequence to the main job
DerivationFrameworkJob += HDBS1Sequence

# slimming - last position
DerivationFrameworkHiggs.HIGG4DxSlimming.setup(DAOD_StreamID, HDBS1Stream, HDBS1SlimmingHelper)
