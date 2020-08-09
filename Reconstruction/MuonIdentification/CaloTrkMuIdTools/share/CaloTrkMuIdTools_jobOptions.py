###############################################################################
## JobOptions for setting up CaloTrkMuIdTools 
###############################################################################
include.block("CaloTrkMuIdTools/CaloTrkMuIdTools_jobOptions.py")


###logfile
from AthenaCommon.Logging import log

###############################################################################
## Helper function for defining THistSvc inputs
###############################################################################
# Finds the root file \@rootFileName and creates a one-item list for THistSvc.Input
###############################################################################

###############################################################################
## jobOptions
###############################################################################
if not 'doMEOTP' in dir():
   doMEOTP=False

### Setup extrapolator ###
if(doMEOTP):
   ### Configure Material Effects On Track Provider (usually this is not done) ###
   include('CaloMaterialEffectsOnTrackProvider/CaloMaterialEffectsOnTrackProvider.py')
   from TrkExTools.TrkExToolsConf import Trk__MaterialEffectsUpdator
   CaloTrkMuId_MaterialUpdator            = Trk__MaterialEffectsUpdator()
   ToolSvc += CaloTrkMuId_MaterialUpdator
   
   from TrkExRungeKuttaPropagator.TrkExRungeKuttaPropagatorConf import Trk__RungeKuttaPropagator
   CaloTrkMuId_Propagator = Trk__RungeKuttaPropagator()
   ToolSvc += CaloTrkMuId_Propagator
   
   from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
   CaloTrkMuIdTools_Extrapolator = AtlasExtrapolator(name = 'CaloTrkMuIdTools_Extrapolator')
   CaloTrkMuIdTools_Extrapolator.DoCaloDynamic          = True
   CaloTrkMuIdTools_Extrapolator.Propagators            = [CaloTrkMuIdTools_Propagator]
   CaloTrkMuIdTools_Extrapolator.MaterialEffectsUpdator = CaloTrkMuIdTools_MaterialUpdator
   
else:
    ### Use the public AtlasExtrapolator ###
    from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
    CaloTrkMuIdTools_Extrapolator = AtlasExtrapolator()
    
ToolSvc += CaloTrkMuIdTools_Extrapolator
#print      CaloTrkMuIdTools_Extrapolator

### Configure TrackEnergyInCaloTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackEnergyInCaloTool as ConfiguredTrackEnergyInCaloTool
TrackEnergyInCaloTool = ConfiguredTrackEnergyInCaloTool(name = "TrackEnergyInCaloTool", ExtrapolatorHandle = CaloTrkMuIdTools_Extrapolator)
ToolSvc += TrackEnergyInCaloTool

### Configure TrackDepositInCaloTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import TrackDepositInCaloTool as ConfiguredTrackDepositInCaloTool
TrackDepositInCaloTool = ConfiguredTrackDepositInCaloTool(name = "TrackDepositInCaloTool")
ToolSvc += TrackDepositInCaloTool

### Configure CaloMuonLikelihoodTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import CaloMuonLikelihoodTool as ConfiguredCaloMuonLikelihoodTool
CaloMuonLikelihoodTool = ConfiguredCaloMuonLikelihoodTool(TrackEnergyInCaloTool = TrackEnergyInCaloTool)
ToolSvc += CaloMuonLikelihoodTool

### Configure CaloMuonScoreTool ###
from CaloTrkMuIdTools.CaloTrkMuIdToolsConf import CaloMuonScoreTool as ConfiguredCaloMuonScoreTool
CaloMuonScoreTool = ConfiguredCaloMuonScoreTool(TrackEnergyInCaloTool = TrackEnergyInCaloTool)
ToolSvc += CaloMuonScoreTool

