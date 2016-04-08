# $Id: SMLIGHTD3PD_prodJobOFragment.py 520434 2012-10-06 13:16:34Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMLIGHT D3PD cart".


# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMLIGHTD3PD_prodJobOFragment.py" )


# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


# Set up a logger:
from AthenaCommon.Logging import logging
SMLIGHTD3PDStream_msg = logging.getLogger( 'SMLIGHTD3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteSMLIGHTD3PD.isVirtual:
    SMLIGHTD3PDStream_msg.error( "The SMLIGHT D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMLIGHT D3PD set to be a virtual stream" )
    pass


# Construct the stream and file names for the SMLIGHT D3PD:
streamName = prodFlags.WriteSMLIGHTD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMLIGHTD3PD )
SMLIGHTD3PDStream_msg.info( "Configuring SMLIGHTD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )


#### add specific containers
include ("PhysicsD3PDMaker/ElectronSelector.py")
include ("PhysicsD3PDMaker/PhotonSelector.py")
include ("PhysicsD3PDMaker/MuonSelector.py")
include ("PhysicsD3PDMaker/JetSelector.py")
include ("PhysicsD3PDMaker/TauSelector.py")


# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "SMLIGHT_D3PDSvc" )
ServiceMgr.SMLIGHT_D3PDSvc.VetoedNames  = [ "L1_.*","L2_.*","EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMLIGHT_D3PDSvc.VetoedNames += [ ".*etx.*",".*ety.*" ]


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMLIGHTD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" ) 
SMLIGHTD3PDStream.D3PDSvc = ServiceMgr.SMLIGHT_D3PDSvc


# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
SMWZLightD3PD( SMLIGHTD3PDStream,
               stdElectronContainer = 'HighPtElectrons',
               gsfElectronContainer = 'HighPtGSFElectrons',
               stacoMuonContainer   = 'HighPtStacoMuons',
               muidMuonContainer    = 'HighPtMuidMuons',
               photonContainer      = 'HighPtPhotons',
               jetContainer         = 'HighPtJets' )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMLIGHTD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMLIGHTD3PDStream ) ]


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
