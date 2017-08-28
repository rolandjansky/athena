include.block ("MuonTagTools/MuonTagTool_jobOptions.py")

########### Muon tag options ################

import EventKernel.ParticleDataType

CosmicsCommissioning = False
theMuonCollection    = [ "Muons" ]
MuonMETContainerName = "Muons_TAG_skim"
from AthenaCommon import CfgMgr
from AthenaCommon.BeamFlags import jobproperties

if jobproperties.Beam.beamType() == 'cosmics' or jobproperties.Beam.beamType() == 'singlebeam':
   CosmicsCommissioning = True
   theMuonCollection = [ "MuidMuonCollection" ]

from AthenaCommon.AppMgr import ToolSvc

########### Muon Selection tool ################

from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool
MuonCorrectionTool = CP__MuonCalibrationAndSmearingTool("MuonCorrectionTool")
ToolSvc += MuonCorrectionTool        

########### Muon Selection tool ################

from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
MuonSelectionTool = CP__MuonSelectionTool("MuonSelectionTool",
                                          MaxEta = 2.7)
ToolSvc += MuonSelectionTool        

########### Muon Isolation options ################

from IsolationSelection.IsolationSelectionConf import CP__IsolationSelectionTool
LooseTrackOnlyIsoTool            = CfgMgr.CP__IsolationSelectionTool( "MuonLooseTrackOnlyIsolationSelectionTool" )
LooseTrackOnlyIsoTool.MuonWP     = "LooseTrackOnly"
ToolSvc += LooseTrackOnlyIsoTool
LooseIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "MuonLooseIsolationSelectionTool" )
LooseIsoTool.MuonWP              = "Loose"
ToolSvc += LooseIsoTool
TightIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "MuonTightIsolationSelectionTool" )
TightIsoTool.MuonWP              = "Tight"
ToolSvc += TightIsoTool
GradientIsoTool                  = CfgMgr.CP__IsolationSelectionTool( "MuonGradientIsolationSelectionTool" )
GradientIsoTool.MuonWP           = "Gradient"
ToolSvc += GradientIsoTool
GradientLooseIsoTool             = CfgMgr.CP__IsolationSelectionTool( "MuonGradientLooseIsolationSelectionTool" )
GradientLooseIsoTool.MuonWP      = "GradientLoose"
ToolSvc += GradientLooseIsoTool
FixedCutTightTrackOnlyIsoTool            = CfgMgr.CP__IsolationSelectionTool( "MuonFixedCutTightTrackOnlyIsolationSelectionTool" )
FixedCutTightTrackOnlyIsoTool.MuonWP     = "FixedCutTightTrackOnly"
ToolSvc += FixedCutTightTrackOnlyIsoTool
FixedCutLooseIsoTool                     = CfgMgr.CP__IsolationSelectionTool( "MuonFixedCutLooseIsolationSelectionTool" )
FixedCutLooseIsoTool.MuonWP              = "FixedCutLoose"
ToolSvc += FixedCutLooseIsoTool

from RecExConfig.RecFlags import rec
from MuonTagTools.MuonTagToolsConf import MuonTagTool as ConfiguredMuonTagTool
from MuonRecExample.MuonRecFlags import muonRecFlags
MuonTagTool = ConfiguredMuonTagTool(Container              = theMuonCollection,
                                    MuonMETContainerName   = MuonMETContainerName,
                                    PtCut                  = 6.0*GeV,
                                    EtconeIsoCutValues     = [3.0*GeV, 5.0*GeV],
                                    PtconeIsoCutValues     = [3.0*GeV, 5.0*GeV],
                                    EtconeRelIsoCutValues  = [0.15, 0.3],
                                    PtconeRelIsoCutValues  = [0.15, 0.3],
                                    isCosmics              = CosmicsCommissioning,
                                    doInDet                = rec.doInDet(),
                                    MuonCalibrationTool    = MuonCorrectionTool,
                                    MuonSelectionTool      = MuonSelectionTool,
                                    LooseTrackOnlyIsolation= LooseTrackOnlyIsoTool,
                                    LooseIsolation         = LooseIsoTool      ,
                                    TightIsolation         = TightIsoTool      ,
                                    GradientIsolation      = GradientIsoTool   ,
                                    GradientLooseIsolation = GradientLooseIsoTool,
                                    FixedCutTightTrackOnlyIsolation= FixedCutTightTrackOnlyIsoTool,
                                    FixedCutLooseIsolation         = FixedCutLooseIsoTool,
                                    DoIsolation = muonRecFlags.doMuonIso()
                                    )
ToolSvc += MuonTagTool
