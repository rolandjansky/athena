# $Id: SMZMUMUD3PD_prodJobOFragment.py 520434 2012-10-06 13:16:34Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMZMUMU D3PD cart".


# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMZMUMUD3PD_prodJobOFragment.py" )


# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName


# Set up a logger:
from AthenaCommon.Logging import logging
SMZMUMUD3PDStream_msg = logging.getLogger( 'SMZMUMUD3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteSMZMUMUD3PD.isVirtual:
    SMZMUMUD3PDStream_msg.error( "The SMZMUMU D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMZMUMU D3PD set to be a virtual stream" )
    pass


# Construct the stream and file names for the SMZMUMU D3PD:
streamName = prodFlags.WriteSMZMUMUD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMZMUMUD3PD )
SMZMUMUD3PDStream_msg.info( "Configuring SMZMUMUD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )


#### add specific containers
include ("PhysicsD3PDMaker/ElectronSelector.py")
include ("PhysicsD3PDMaker/PhotonSelector.py")
include ("PhysicsD3PDMaker/LowPtMuonPairSelector.py")
include ("PhysicsD3PDMaker/JetSelector.py")
include ("PhysicsD3PDMaker/TauSelector.py")


# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "SMZMUMU_D3PDSvc" )
ServiceMgr.SMZMUMU_D3PDSvc.VetoedNames  = [ "L1_.*","L2_.*","EF_e.*","EF_2e.*","EF_g.*","EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMZMUMU_D3PDSvc.VetoedNames += [ "trig_EF_el.*" ]
ServiceMgr.SMZMUMU_D3PDSvc.VetoedNames += [ ".*etx.*",".*ety.*" ]


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMZMUMUD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
SMZMUMUD3PDStream.D3PDSvc = ServiceMgr.SMZMUMU_D3PDSvc


# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
SMWZLightD3PD( SMZMUMUD3PDStream,
               stdElectronContainer = 'HighPtElectrons',
               stacoMuonContainer   = 'LowPtStacoMuons',
               muidMuonContainer    = 'LowPtMuidMuons',
               photonContainer      = 'HighPtPhotons',
               jetContainer         = 'HighPtJets' )


# apply skim
SMZMUMUD3PDStream.AddAcceptAlgs( ["SMWZ_ZmumuStacoFilter", "SMWZ_ZmumuMuidFilter"] )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMZMUMUD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMZMUMUD3PDStream ) ]


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
