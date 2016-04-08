# $Id: SMWMUNUJJD3PD_prodJobOFragment.py 354388 2011-03-25 15:03:06Z boonekam $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMWMUNUJJ D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMWMUNUJJD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMWMUNUJJD3PDStream_msg = logging.getLogger( 'SMWMUNUJJD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMWMUNUJJD3PD.isVirtual:
    SMWMUNUJJD3PDStream_msg.error( "The SMWMUNUJJ D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMWMUNUJJ D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMWMUNUJJ D3PD:
streamName = prodFlags.WriteSMWMUNUJJD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMWMUNUJJD3PD )
SMWMUNUJJD3PDStream_msg.info( "Configuring SMWMUNUJJD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#### add specific containers
include ("PhysicsD3PDMaker/ElectronSelector.py")
#include ("PhysicsD3PDMaker/PhotonSelector.py")
include ("PhysicsD3PDMaker/MuonSelector.py")
include ("PhysicsD3PDMaker/JetSelector.py")
#include ("PhysicsD3PDMaker/TauSelector.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMWMUNUJJD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" ) 

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
SMWZD3PD( SMWMUNUJJD3PDStream,
          stdElectronContainer = 'HighPtElectrons',
          gsfElectronContainer = 'None',
          muonContainer        = 'None',
          stacoMuonContainer   = 'HighPtStacoMuons',
          muidMuonContainer    = 'None',
          caloMuonContainer    = 'None',
          photonContainer      = 'None',
          tauContainer         = 'None',
          jetEM4Container      = 'HighPtJets',
          jetEM6Container      = 'HighPtJetsEM6',
          jetLC4Container      = 'HighPtJetsLC4',
          jetLC6Container      = 'HighPtJetsLC6',
          jetDetails           = 2,
          METDetails           = 2)

# Define skims
from WZSkims.WZSkimsConf import WZSkimsMain
Wmunujj = WZSkimsMain("Wmunujj")
Wmunujj.doWmunujjSkim = True
Wmunujj.OutputLevel = INFO
SMWMUNUJJD3PDStream.filterSeq += Wmunujj

include("WZSkims/Wlnujj_branchSelection.py")

# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMWMUNUJJD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMWMUNUJJD3PDStream ) ]
