# $Id: SMDYMUMUD3PD_prodJobOFragment.py 520434 2012-10-06 13:16:34Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMDYMUMU D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMDYMUMUD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMDYMUMUD3PDStream_msg = logging.getLogger( 'SMDYMUMUD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMDYMUMUD3PD.isVirtual:
    SMDYMUMUD3PDStream_msg.error( "The SMDYMUMU D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMDYMUMU D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMDYMUMU D3PD:
streamName = prodFlags.WriteSMDYMUMUD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMDYMUMUD3PD )
SMDYMUMUD3PDStream_msg.info( "Configuring SMDYMUMUD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )


# add specific containers
include ("PhysicsD3PDMaker/ElectronSelector.py")
include ("PhysicsD3PDMaker/PhotonSelector.py")
include ("PhysicsD3PDMaker/LowPtMuonPairSelector.py")
include ("PhysicsD3PDMaker/JetSelector.py")
include ("PhysicsD3PDMaker/TauSelector.py")


# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "SMDYMUMU_D3PDSvc" )
ServiceMgr.SMDYMUMU_D3PDSvc.VetoedNames  = [ "L1_.*","L2_.*","EF_e.*","EF_2e.*","EF_g.*","EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMDYMUMU_D3PDSvc.VetoedNames += [ "trig_EF_el.*" ]
ServiceMgr.SMDYMUMU_D3PDSvc.VetoedNames += [ "MET.*",".*etx.*",".*ety.*" ]


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMDYMUMUD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
SMDYMUMUD3PDStream.D3PDSvc = ServiceMgr.SMDYMUMU_D3PDSvc


# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
SMWZLightD3PD( SMDYMUMUD3PDStream,
               stacoMuonContainer   = 'LowPtStacoMuons',
               muidMuonContainer    = 'LowPtMuidMuons' )


# apply skim
SMDYMUMUD3PDStream.AddAcceptAlgs( ["SMWZ_JPsimumuStacoFilter", "SMWZ_JPsimumuMuidFilter"] )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMDYMUMUD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMDYMUMUD3PDStream ) ]


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
