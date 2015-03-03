# $Id: makeTrigMuonD3PD.py 307702 2010-06-29 12:06:37Z krasznaa $
#
# Example jobO for saving the full trigger muon information from an ESD/AOD file.
#

# Properties that the users should modify:
if not "InputFiles" in dir():
    InputFiles = [ "AOD.pool.root" ]
if not "level" in dir():
    level = 10
if not "OutputFile" in dir():
    OutputFile = "trig_muon_d3pd_level%d.root" % level
if not "EvtMax" in dir():
    EvtMax = 100

# Set up the needed RecEx flags:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput.set_Value_and_Lock( InputFiles )
athenaCommonFlags.EvtMax.set_Value_and_Lock( EvtMax )

# Set up what the RecExCommon job should and shouldn't do:
from RecExConfig.RecFlags import rec
rec.AutoConfiguration = [ "everything" ]
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteESD.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doESD.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doDPD.set_Value_and_Lock( False )
rec.doHist.set_Value_and_Lock( False )
rec.doPerfMon.set_Value_and_Lock( False )
rec.doForwardDet.set_Value_and_Lock( False )

# Let RecExCommon set everything up:
include( "RecExCommon/RecExCommon_topOptions.py" )

#########################################################################
#                                                                       #
#                     Now set up the D3PDMaker job                      #
#                                                                       #
#########################################################################

# Create a logger, mainly for fun:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "makeTrigMuonD3PD" )

# Set which HLT chains to care about:
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.MuonL1TrigPattern = "L1_MU0|L1_MU10|L1_MU20"
D3PDMakerFlags.MuonL2TrigPattern = "L2_mu4|L2_mu10|L2_mu20|L2_mu4_tile_SiTrk|L2_mu4_trod_SiTrk"
D3PDMakerFlags.MuonEFTrigPattern = "EF_mu4|EF_mu10|EF_mu20|EF_mu4_tile_SiTrk|EF_mu4_trod_SiTrk"

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Save the full trigger muon information:
from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PD
d3pdalg = TrigMuonD3PD( OutputFile, level = level )

# Add some basic event-level information:
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( level )

#Save all of bit information of trigger decision 
from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
d3pdalg += TrigDecisionD3PDObject( level )

# Add Primary Vertex Information
from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
d3pdalg += PrimaryVertexD3PDObject(level, sgkey = D3PDMakerFlags.VertexSGKey(), prefix = 'vxp_')

#Add Offline Muon Information
from MuonD3PDMaker.MuonD3PDObject             import MuonD3PDObject
d3pdalg += MuonD3PDObject( level, sgkey='MuidMuonCollection', prefix='mu_muid_' )
d3pdalg += MuonD3PDObject( level, sgkey='StacoMuonCollection', prefix='mu_staco_' )

#Add TGC information
from TrigMuonD3PDMaker.TgcD3PD import TgcD3PD
TgcD3PD( d3pdalg, level)

#Add RPC information
from TrigMuonD3PDMaker.RpcCoinDataD3PDObject import RpcCoinDataD3PDObject
d3pdalg += RpcCoinDataD3PDObject( level )
from TrigMuonD3PDMaker.RpcPadD3PDObject import RpcPadD3PDObject
d3pdalg += RpcPadD3PDObject( level )

#Add MuCTPi information
from TriggerD3PDMaker.MuCTPID3PDObject import MuCTPID3PDObject
d3pdalg += MuCTPID3PDObject(level)

logger.info( "Configured the D3PDMaker algorithm" )

# Do some of the additional setup:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
