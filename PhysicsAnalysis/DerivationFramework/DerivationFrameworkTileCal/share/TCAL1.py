#====================================================================
# TCAL1.py
# Test set-up
# It requires the reductionConf flag TCAL1 in Reco_tf.py  
#====================================================================


if not 'SaveTileCellPositionAndDimention' in dir():
    SaveTileCellPositionAndDimention = True

if not 'SaveTileCellPmtInfo' in dir():
    SaveTileCellPmtInfo = True

if not 'SaveTileCellMuonInfo' in dir():
    SaveTileCellMuonInfo = True

if not 'TileDerivationPrefix' in dir():
    TCAL1Prefix = 'TCAL1_'

# TRACK TOOLS
from DerivationFrameworkTileCal.DerivationFrameworkTileCalConf import TrackTools
TCAL1TrackTools = TrackTools()
ToolSvc += TCAL1TrackTools


# Set up common services and job object.
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# CP GROUP TOOLS
#====================================================================

from DerivationFrameworkMuons.MuonsCommon import *

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
TCAL1AugmentationTools = []

TCAL1TileCellsMuonDecorator = CfgMgr.DerivationFramework__TileCellsMuonDecorator(name = 'TCAL1TileCellsMuonDecorator'
                                                                                 , CellContainer = 'AODCellContainer'
                                                                                 , Prefix = TCAL1Prefix
                                                                                 , SelectMuons = False
                                                                                 , MinMuonPt = 10 * GeV
                                                                                 , MaxAbsMuonEta = 1.7
                                                                                 , IsoCone = 0.4)

from AthenaCommon.JobProperties import jobproperties
if jobproperties.Beam.beamType() == 'collisions':
    TCAL1TileCellsMuonDecorator.SelectMuons = True

ToolSvc += TCAL1TileCellsMuonDecorator
TCAL1AugmentationTools.append(TCAL1TileCellsMuonDecorator)
print TCAL1TileCellsMuonDecorator


#====================================================================
# SKIMMING TOOLS
#====================================================================



if jobproperties.Beam.beamType() == 'collisions':
    TCAL1SelectionExpression = 'Muons.'+ TCAL1Prefix + 'SelectedMuon' 
else:
    TCAL1SelectionExpression = 'abs(Muons.eta) < 1.7'



TCAL1SkimmingExpression = 'count(' + TCAL1SelectionExpression + ') > 0'

TCAL1StringSkimmingTool = CfgMgr.DerivationFramework__xAODStringSkimmingTool(name = 'TCAL1StringSkimmingTool'
                                                                             , expression = TCAL1SkimmingExpression)
ToolSvc += TCAL1StringSkimmingTool
print TCAL1StringSkimmingTool




#====================================================================
# SET UP STREAM  
#====================================================================
streamName = derivationFlags.WriteDAOD_TCAL1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TCAL1Stream )
TCAL1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TCAL1Stream.AcceptAlgs(["TCAL1Kernel"])




#====================================================================
# THINNING SERVICE AND TOOLS
#====================================================================

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TCAL1ThinningHelper = ThinningHelper( "TCAL1ThinningHelper" )
TCAL1ThinningHelper.AppendToStream( TCAL1Stream )


TCAL1ThinningTools = []

if jobproperties.Beam.beamType() == 'collisions':

    TCAL1MuonTPThinningTool = CfgMgr.DerivationFramework__MuonTrackParticleThinning(name = "TCAL1MuonTPThinningTool"
                                                                                    , ThinningService = TCAL1ThinningHelper.ThinningSvc()
                                                                                    , MuonKey = "Muons"
                                                                                    , InDetTrackParticlesKey  = "InDetTrackParticles"
                                                                                    , SelectionString = TCAL1SelectionExpression
                                                                                    #, ConeSize = 0.4
                                                                                    , ApplyAnd = False)
    ToolSvc += TCAL1MuonTPThinningTool
    TCAL1ThinningTools.append(TCAL1MuonTPThinningTool)
    print TCAL1MuonTPThinningTool

    TCAL1MuonThinningTool = CfgMgr.DerivationFramework__GenericObjectThinning(name = "TCAL1MuonThinningTool"
                                                                              , ThinningService = TCAL1ThinningHelper.ThinningSvc()
                                                                              , ContainerName = "Muons"
                                                                              , SelectionString = TCAL1SelectionExpression )
    

    ToolSvc += TCAL1MuonThinningTool
    TCAL1ThinningTools.append(TCAL1MuonThinningTool)
    print TCAL1MuonThinningTool




#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TCAL1Kernel"
                                                                       , AugmentationTools = TCAL1AugmentationTools
                                                                       , SkimmingTools = [TCAL1StringSkimmingTool]
                                                                       , ThinningTools = TCAL1ThinningTools)



#====================================================================
# CONTENT DEFINITION
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TCAL1SlimmingHelper = SlimmingHelper('TCAL1SlimmingHelper')

TCAL1SlimmingHelper.SmartCollections = ['Muons', 'MET_Reference_AntiKt4LCTopo']
#TCAL1SlimmingHelper.AllVariables = ['Muons']

TCAL1ExtarVariables = 'Muons.' + TCAL1Prefix + 'etrkcone40'

for variable in ['energy', 'et', 'eta', 'phi', 'gain', 'bad', 'time', 'quality', 'sampling', 'sinTh', 'cosTh', 'cotTh', 'x', 'y', 'z']:
    TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + variable)


if SaveTileCellPositionAndDimention:
    for variable in ['side', 'section', 'module', 'tower', 'sample']:
        TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + variable)
    for variable in ['r', 'dx', 'dy', 'dz', 'dr', 'dphi', 'deta', 'volume']:
        TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + variable)


if SaveTileCellPmtInfo:
    for variable in ['ros', 'drawer', 'channel', 'energy', 'time', 'quality', 'qbit', 'bad', 'gain']:
        for pmt in ['pmt1', 'pmt2']:
            TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + pmt + '_' + variable)


if SaveTileCellMuonInfo:
    for variable in ['dx', 'dedx', 'x', 'y', 'z', 'eta', 'phi']:
        TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + 'muon_' + variable)
    for variable in ['dx', 'dy', 'dz', 'deta', 'dphi']:
        TCAL1ExtarVariables += ('.' + TCAL1Prefix + 'cells_' + 'to_muon_' + variable)


TCAL1SlimmingHelper.ExtraVariables = [TCAL1ExtarVariables]
TCAL1SlimmingHelper.AppendContentToStream(TCAL1Stream)
