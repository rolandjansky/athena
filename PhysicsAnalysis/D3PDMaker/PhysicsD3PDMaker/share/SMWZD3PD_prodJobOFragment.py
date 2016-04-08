# $Id: SMWZD3PD_prodJobOFragment.py 542026 2013-03-22 15:02:53Z jlee $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMWZ D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMWZD3PD_prodJobOFragment.py" )

### Jason
include( "PhysicsD3PDMaker/SMWZ_RedoJets.py")

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMWZD3PDStream_msg = logging.getLogger( 'SMWZD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMWZD3PD.isVirtual:
    SMWZD3PDStream_msg.error( "The SMWZ D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "SMWZ D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMWZ D3PD:
streamName = prodFlags.WriteSMWZD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMWZD3PD )
SMWZD3PDStream_msg.info( "Configuring SMWZD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Fat/Groomed Jets
include("PhysicsD3PDMaker/FatJetsConfig.py")
###########Bertrand
include( "BTagging/BTagging_LoadTools.py" )
include( "BTagging/BTagging_jobOptions.py" )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMWZD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
SMWZD3PD( SMWZD3PDStream )


from PhysicsD3PDMaker.JSD3PD import JSD3PD
for xx in myJetKeys:
      if xx[0] != None and xx[1] != None:
	        JSD3PD(xx, SMWZD3PDStream )

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
