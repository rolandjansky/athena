# $Id: makeTrigMuonD3PD.py 341143 2011-01-21 08:45:31Z krasznaa $
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

# Create the D3PD output stream:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( "trigger", OutputFile )

# Save the full trigger muon information:
from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PDObjects
TrigMuonD3PDObjects( d3pdalg, level = level )

# Add some basic event-level information:
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 1 )

logger.info( "Configured the D3PDMaker algorithm" )

# Do some of the additional setup:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = athenaCommonFlags.EvtMax()
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 10000000
