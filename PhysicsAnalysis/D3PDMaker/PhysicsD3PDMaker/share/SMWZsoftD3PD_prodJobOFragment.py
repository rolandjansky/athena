# $Id: SMWZsoftD3PD_prodJobOFragment.py 566706 2013-10-23 08:41:31Z ssnyder $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMWZ D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMWZsoftD3PD_prodJobOFragment.py" )

### Jason
include( "PhysicsD3PDMaker/SMWZ_RedoJets.py")

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMWZSOFTD3PDStream_msg = logging.getLogger( 'SMWZsoftD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMWZSOFTD3PD.isVirtual:
    SMWZD3PDStream_msg.error( "The SMWZ soft D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMWZ soft D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMWZ soft D3PD:
streamName = prodFlags.WriteSMWZSOFTD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMWZSOFTD3PD )
SMWZSOFTD3PDStream_msg.info( "Configuring SMWZsoftD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# disable parton filter
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.TruthWriteHadrons = True

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

### Jason
include( "BTagging/BTagging_LoadTools.py" )
include( "BTagging/BTagging_jobOptions.py" )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMWZSOFTD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
if not 'SMWZsoftD3PD_args' in globals():
    SMWZsoftD3PD_args = {}
SMWZD3PD( SMWZSOFTD3PDStream, doSoftQCD = False, **SMWZsoftD3PD_args )

# Silence tool warnings:
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
    pass
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR

if hasattr( ToolSvc, "SecondSoftMuonTagChi2" ):
    ToolSvc.SecondSoftMuonTagChi2.OutputLevel = ERROR

if hasattr( ToolSvc, "myTrkUpdator" ):
    ToolSvc.myTrkUpdator.OutputLevel = ERROR
