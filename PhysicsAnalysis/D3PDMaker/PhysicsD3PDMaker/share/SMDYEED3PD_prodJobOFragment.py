# $Id: SMDYEED3PD_prodJobOFragment.py 520434 2012-10-06 13:16:34Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMDYEE D3PD cart".


# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMDYEED3PD_prodJobOFragment.py" )


# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


# Set up a logger:
from AthenaCommon.Logging import logging
SMDYEED3PDStream_msg = logging.getLogger( 'SMDYEED3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteSMDYEED3PD.isVirtual:
    SMDYEED3PDStream_msg.error( "The SMDYEE D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMDYEE D3PD set to be a virtual stream" )
    pass


# Construct the stream and file names for the SMDYEE D3PD:
streamName = prodFlags.WriteSMDYEED3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMDYEED3PD )
SMDYEED3PDStream_msg.info( "Configuring SMDYEED3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )


#### add specific containers
include ("PhysicsD3PDMaker/LowPtElectronPairSelector.py")
include ("PhysicsD3PDMaker/PhotonSelector.py")
include ("PhysicsD3PDMaker/MuonSelector.py")
include ("PhysicsD3PDMaker/JetSelector.py")
include ("PhysicsD3PDMaker/TauSelector.py")


# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "SMDYEE_D3PDSvc" )
ServiceMgr.SMDYEE_D3PDSvc.VetoedNames  = [ "L1_.*","L2_.*","EF_m.*","EF_2m.*","EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMDYEE_D3PDSvc.VetoedNames += [ "trig_EF_trigmu.*" ]
ServiceMgr.SMDYEE_D3PDSvc.VetoedNames += [ "MET.*",".*etx.*",".*ety.*" ]


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMDYEED3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
SMDYEED3PDStream.D3PDSvc = ServiceMgr.SMDYEE_D3PDSvc


# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
SMWZLightD3PD( SMDYEED3PDStream,
               stdElectronContainer = 'LowPtElectrons' )


# apply skim
SMDYEED3PDStream.AddAcceptAlgs( ["SMWZ_JPsieeFilter"] )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMDYEED3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMDYEED3PDStream ) ]


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
