# $Id: SMZEED3PD_prodJobOFragment.py 520434 2012-10-06 13:16:34Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMZEE D3PD cart".


# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMZEED3PD_prodJobOFragment.py" )


# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


# Set up a logger:
from AthenaCommon.Logging import logging
SMZEED3PDStream_msg = logging.getLogger( 'SMZEED3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteSMZEED3PD.isVirtual:
    SMZEED3PDStream_msg.error( "The SMZEE D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMZEE D3PD set to be a virtual stream" )
    pass


# Construct the stream and file names for the SMZEE D3PD:
streamName = prodFlags.WriteSMZEED3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMZEED3PD )
SMZEED3PDStream_msg.info( "Configuring SMZEED3PD with streamName '%s' and fileName '%s'" % \
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
ServiceMgr += D3PD__RootD3PDSvc( "SMZEE_D3PDSvc" )
ServiceMgr.SMZEE_D3PDSvc.VetoedNames  = [ "L1_.*","L2_.*","EF_m.*","EF_2m.*","EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMZEE_D3PDSvc.VetoedNames += [ "trig_EF_trigmu.*" ]
ServiceMgr.SMZEE_D3PDSvc.VetoedNames += [ ".*etx.*",".*ety.*" ]


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMZEED3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
SMZEED3PDStream.D3PDSvc = ServiceMgr.SMZEE_D3PDSvc


# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
SMWZLightD3PD( SMZEED3PDStream,
               stdElectronContainer = 'MediumPtElectrons',
               stacoMuonContainer   = 'HighPtStacoMuons',
               photonContainer      = 'HighPtPhotons',
               jetContainer         = 'HighPtJets' )


# apply skim
SMZEED3PDStream.AddAcceptAlgs( ["SMWZ_ZeeFilter"] )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMZEED3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMZEED3PDStream ) ]


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
