#********************************************************************
# HIGG4D4.py:  reductionConf flag HIGG4D4 in Reco_tf.py
# Z. Zinonos - zenon@cern.ch
#
DAOD_StreamID = 'HIGG4D4'
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

print "Hello, my name is {} and I am running on {}".format(DAOD_StreamID, 'MC' if DFisMC else 'Data')

#==============
# SET UP STREAM
#==============
streamName = derivationFlags.WriteDAOD_HIGG4D4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HIGG4D4Stream )
HIGG4D4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
HIGG4D4Stream.AcceptAlgs([DAOD_StreamID+"Kernel"])

#============
# Setup tools
#============
# Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
HIGG4D4ThinningHelper = ThinningHelper( DAOD_StreamID+"ThinningHelper" )

#trigger navigation thinning
import DerivationFrameworkHiggs.HIGG4DxThinning
HIGG4D4ThinningHelper.TriggerChains = DerivationFrameworkHiggs.HIGG4DxThinning.TriggerChains(DAOD_StreamID)
HIGG4D4ThinningHelper.AppendToStream( HIGG4D4Stream )

# thinning tools
thinningTools = DerivationFrameworkHiggs.HIGG4DxThinning.setup(DAOD_StreamID, HIGG4D4ThinningHelper.ThinningSvc(), ToolSvc)

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
HIGG4D4SlimmingHelper = SlimmingHelper(DAOD_StreamID+"SlimmingHelper")

# jets and calibration
import DerivationFrameworkHiggs.HIGG4DxJets

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
HIGG4D4Sequence = CfgMgr.AthSequencer(DAOD_StreamID+"Sequence")

# augmentation
HIGG4D4Sequence += CfgMgr.DerivationFramework__CommonAugmentation("HIGG4DxCommonAugmentationKernel", AugmentationTools = augmentationTools)

# skimming
# Note - unique to 21.2.10.1
higg4d4_expression = "(((count( (( abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3) ) || (TauJets.pt > 100.0*GeV && TauJets.nTracks == 2 )) && TauJets.pt > 45.0*GeV) >= 2)) || ((count((TauJets.pt > 100.0*GeV)) >= 1) && (count((TauJets.pt > 45.0*GeV)) >= 2) && (count((TauJets.nTracks == 1 || TauJets.nTracks == 2 || TauJets.nTracks == 3) && TauJets.pt > 45.0*GeV) >= 1)))&&(count( Electrons.pt > 15.0*GeV && (abs(Electrons.eta) < 1.30 || abs(Electrons.eta) > 1.60) && abs(Electrons.eta) < 2.35  && (Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight) ) == 0)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HIGG4D4SpecialSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name 		= "HIGG4D4SpecialSkimmingTool",
                                                                         expression 	= higg4d4_expression)

ToolSvc += HIGG4D4SpecialSkimmingTool
 
HIGG4D4Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"SkimmingKernel", SkimmingTools = [HIGG4D4SpecialSkimmingTool])


# fat/trimmed jet building (after skimming)
DerivationFrameworkHiggs.HIGG4DxJets.setup(DAOD_StreamID, HIGG4D4Sequence, HIGG4D4SlimmingHelper)

# thinning
HIGG4D4Sequence += CfgMgr.DerivationFramework__DerivationKernel(DAOD_StreamID+"Kernel", ThinningTools = thinningTools)

# add the private sequence to the main job
DerivationFrameworkJob += HIGG4D4Sequence

# slimming - last position
DerivationFrameworkHiggs.HIGG4DxSlimming.setup(DAOD_StreamID, HIGG4D4Stream, HIGG4D4SlimmingHelper)
