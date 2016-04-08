# $Id: SMTRILEPD3PD_prodJobOFragment.py 531942 2013-01-06 20:06:49Z nedwards $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMTRILEP D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMTRILEPD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMTRILEPD3PDStream_msg = logging.getLogger( 'SMTRILEPD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMTRILEPD3PD.isVirtual:
    SMTRILEPD3PDStream_msg.error( "The SMTRILEP D3PD stream can't be virtual! " +
                                  "It's a configuration error!" )
    raise NameError( "SMTRILEP D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMTRILEP D3PD:
streamName = prodFlags.WriteSMTRILEPD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMTRILEPD3PD )
SMTRILEPD3PDStream_msg.info( "Configuring SMTRILEPD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
#include ("PhysicsD3PDMaker/MyJetMake.py")


#configure MuonScatteringAngleSignificanceTool
#include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMTRILEPD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
    #from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
    #SMWZD3PD( SMTRILEPD3PDStream )

#h ok
### add specific containers
# Will need to redefine these ourselves...
include ("PhysicsD3PDMaker/SMTRILEP_ElectronSelector.py")
include ("PhysicsD3PDMaker/SMTRILEP_PhotonSelector.py")
include ("PhysicsD3PDMaker/SMTRILEP_MuonSelector.py")
include ("PhysicsD3PDMaker/SMTRILEP_JetSelector.py")
#include ("PhysicsD3PDMaker/TauSelector.py")


# Configure branches
from AthenaCommon.AppMgr import ServiceMgr
from D3PDMakerRoot.D3PDMakerRootConf import D3PD__RootD3PDSvc
ServiceMgr += D3PD__RootD3PDSvc( "SMTRILEP_D3PDSvc" )
#ServiceMgr.SMTRILEP_D3PDSvc.VetoedNames  = [ "EF_j.*","EF_2j.*","EF_tau.*" ]
ServiceMgr.SMTRILEP_D3PDSvc.VetoedNames  = [ "^L1_.*","^L2_.*","EF_j.*","EF_2j.*","EF_tau.*", "EF_xe.*", "EF_eb.*", ]
ServiceMgr.SMTRILEP_D3PDSvc.VetoedNames += [ ".*etx.*",".*ety.*", "IPEstimate.*" ]

include("PhysicsD3PDMaker/SMTRILEP_VetoBranches.py")
ServiceMgr.SMTRILEP_D3PDSvc.VetoedNames += SMTRILEP_VetoBranches

SMTRILEPD3PDStream.D3PDSvc = ServiceMgr.SMTRILEP_D3PDSvc

# Now add all the content to this stream:
# Will need to redefine this - defines what goes into the skim
from PhysicsD3PDMaker.SMTrilepD3PD import SMTrilepD3PD
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
SMTrilepD3PD( SMTRILEPD3PDStream,
               stdElectronContainer = 'TrilepElectrons',
              #gsfElectronContainer = 'HighPtGSFElectrons',
               stacoMuonContainer   = 'TrilepStacoMuons',
               thirdMuonContainer   = 'TrilepThirdMuons',
               caloMuonContainer   = 'TrilepCaloMuons',
               #muidMuonContainer    = 'HighPtMuidMuons',
               photonContainer      = 'HighPtPhotons',
               jetLC4Container      = "HighPtJetsLC4",
               jetContainer         = 'HighPtJets' )


# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMTRILEPD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMTRILEPD3PDStream ) ]

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

# Define skims
#from WZSkims.WZSkimsConf import WZSkimsMain
#ThreeLeptons = WZSkimsMain("ThreeLeptons")
#ThreeLeptons.doThreeLeptonsSkim = True
#ThreeLeptons.OutputLevel = INFO
#SMTRILEPD3PDStream.filterSeq += ThreeLeptons
from PhysicsD3PDMaker.PhysicsD3PDMakerConf import TrileptonSkimmer
TrileptonSkim =  TrileptonSkimmer( )
SMTRILEPD3PDStream.filterSeq += TrileptonSkim   # 1 alg, named "TrilepSkim"

# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMTRILEPD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMTRILEPD3PDStream ) ]
