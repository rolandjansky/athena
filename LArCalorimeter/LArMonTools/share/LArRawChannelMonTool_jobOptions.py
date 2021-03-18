# --- for athena online running ---
# For Atlas Ready Filter. Not needed.
from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool

from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg ('totalNoise')


# ---- Local Variables ----
# ---specify trigger tool and trigger chain here
LArRawChannelMonTool_TrigToolName = "Trig::TrigDecisionTool/TrigDecisionTool"
LArRawChannelMonTool_TriggerChain = "L1_RD0_EMPTY" #"L1_RD0_FILLED"

# --- LArRawChannelMon configuration ---
from LArMonTools.LArMonToolsConf import LArRawChannelMonTool
LArRawChannelMon = LArRawChannelMonTool(
    # --- set up mon tool ---
    name                      = "LArRawChannelMon",
    LArRawChannelContainerKey = "LArRawChannels",
    histoPathBase             = "LAr/RawChannelOldTool",
    OutputLevel               = 3,
    # ATLAS Ready Filter Tool

    ATLASReadyFilterTool     = [GetAtlasReadyFilterTool()],

    # --- specify trigger options ---
    # These will be specified for different streams below
    TrigDecisionTool          = "",
    TriggerChain              = "",    
    #Added by Louis Stream Aware, add the stream this tool should monitor:
    noise_streams             = [], #["RNDM"]

    # --- delay between procHistograms() calls ---
    ProcessNEvents = LArMonFlags.EventBlockSize(),

    # --- histogram thresholds ---
    occupancy_thresholds = [ 100 * MeV,  # EMBA
                             100 * MeV,  # EMBC
                             100 * MeV,  # EMECA
                             100 * MeV,  # EMECC
                             500 * MeV,  # HECA
                             500 * MeV,  # HECC
                             700 * MeV,  # FCALA
                             700 * MeV ],# FCALC

    signal_thresholds    = [ -9999. * GeV,  # EMBA
                             -9999. * GeV,  # EMBC
                             -9999. * GeV,  # EMECA
                             -9999. * GeV,  # EMECC
                             -9999. * GeV,  # HECA
                             -9999. * GeV,  # HECC
                             -9999. * GeV,  # FCALA
                             -9999. * GeV ],# FCALC

    pos_noise_thresholds = [ 3,  # EMBA
                             3,  # EMBC
                             3,  # EMECA
                             3,  # EMECC
                             3,  # HECA
                             3,  # HECC
                             3,  # FCALA
                             3 ],# FCALC

    neg_noise_thresholds = [ 3,  # EMBA
                             3,  # EMBC
                             3,  # EMECA
                             3,  # EMECC
                             3,  # HECA
                             3,  # HECC
                             3,  # FCALA
                             3 ],# FCALC

    bcid_signal_threshold         = 500. * MeV,
    time_threshold                = 5,
    quality_threshold             = 65530,
    noise_burst_percent_threshold = [ 1,  # EMBA
                                      1,  # EMBC
                                      1,  # EMECA
                                      1,  # EMECC
                                      1,  # HECA
                                      1,  # HECC
                                      2,  # FCALA
                                      2 ],# FCALC

    noise_burst_nChannel_threshold = [ 10,  # EMBA
                                       10,  # EMBC
                                       10,  # EMECA
                                       10,  # EMECC
                                       10,  # HECA
                                       10,  # HECC
                                       10,  # FCALA
                                       10 ],# FCALC


    # --- Histogram Types ---
    monitor_occupancy         = False,
    monitor_signal            = LArMonFlags.doLArRawMonitorSignal(),

    monitor_positive_noise    = LArMonFlags.doLArRawMonitorSignal(),
    monitor_negative_noise    = LArMonFlags.doLArRawMonitorSignal(),
    monitor_time              = False,
    monitor_quality           = LArMonFlags.doLArRawMonitorSignal(),

    # --- Detector Views ---
    monitor_febs              = False,
    monitor_feedthroughs      = False,
    monitor_detectors         = True,

    # --- OFC as peak finder only ---
    db_and_ofc_only           = True,

    # --- histogram registration ---
    interval                  = "run",

    masking_tool              = theLArBadChannelsMasker,

    # Number of luminosity blocks to display in histograms
    n_lumi_blocks             = 3000,

)


## Monitor signal and noise if cosmic run is being taken. Feb 2012 - VB
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType.get_Value() == 'cosmics':
   CaloNoiseCondAlg ('electronicNoise')
   LArRawChannelMon.NoiseKey = 'electronicNoise'
   LArRawChannelMon.monitor_signal=True
   LArRawChannelMon.monitor_positive_noise=True
   LArRawChannelMon.monitor_negative_noise=True


#The next line is actually getting all the information. I'm sure this is available offline at the T0, but 
#this shouldn't be available online, so be careful when you use it....

#TK: Only do this when trigger is enabled.

from AthenaMonitoring.DQMonFlags import DQMonFlags

if DQMonFlags.useTrigger() and not athenaCommonFlags.isOnline():
	from PyUtils.MetaReaderPeeker import metadata
	# this retrieve the stream the file open belongs to the output is like: express_express or physics_CosmicCalo
	# so retrieve the chain, split it in the "_" char and get the second part (ie CosmicCalo or express...)
	if "stream" in metadata:
		if '_' in metadata['stream']:
			streamFromFile = metadata['stream'].split("_")[1]
			if streamFromFile == 'express':
				LArRawChannelMon.noise_streams = ["RNDM"]
				LArRawChannelMon.TrigDecisionTool = LArRawChannelMonTool_TrigToolName
				LArRawChannelMon.TriggerChain = LArRawChannelMonTool_TriggerChain
			if streamFromFile == 'RNDM':
				LArRawChannelMon.noise_streams = ["RNDM"]
				LArRawChannelMon.TrigDecisionTool = LArRawChannelMonTool_TrigToolName
				LArRawChannelMon.TriggerChain = LArRawChannelMonTool_TriggerChain

# --- register with toolservice ---
#ToolSvc += LArRawChannelMon
LArMon.AthenaMonTools+=[ LArRawChannelMon ]
