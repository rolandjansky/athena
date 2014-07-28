include.block ("MuonTagTools/MuonTagTool_jobOptions.py")

########### Muon tag options ################

import EventKernel.ParticleDataType

CosmicsCommissioning = False
theMuonCollection = [ "Muons" ]
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties

if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':
   CosmicsCommissioning = True
   theMuonCollection = [ "MuidMuonCollection" ]
#ToolSvc += CfgMgr.Rec__MuonOverlapTool( "MuonCombinedOverlapTool" )

from RecExConfig.RecFlags import rec
from MuonTagTools.MuonTagToolsConf import MuonTagTool as ConfiguredMuonTagTool
MuonTagTool = ConfiguredMuonTagTool(Container         = theMuonCollection,
                                    EtCut             = 2.0*GeV,
                                    CaloIsoCutValues  = [3.0*GeV, 5.0*GeV],
                                    TrackIsoCutValues = [3.0*GeV, 5.0*GeV],
                                    CaloRelIsoCutValues  = [0.15, 0.3],
                                    TrackRelIsoCutValues = [0.15, 0.3],
                                    isCosmics         = CosmicsCommissioning,
                                    doInDet           = rec.doInDet())

ToolSvc += MuonTagTool
