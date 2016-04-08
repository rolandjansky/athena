# $Id: SMBKGED3PD_prodJobOFragment.py 466232 2011-11-03 16:47:45Z aad $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMBKGE D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMBKGED3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMBKGED3PDStream_msg = logging.getLogger( 'SMBKGED3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMBKGED3PD.isVirtual:
    SMBKGED3PDStream_msg.error( "The SMBKGE D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMBKGE D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMBKGE D3PD:
streamName = prodFlags.WriteSMBKGED3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMBKGED3PD )
SMBKGED3PDStream_msg.info( "Configuring SMBKGED3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")


#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMBKGED3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
SMWZD3PD( SMBKGED3PDStream )

# Define skims
from WZSkims.WZSkimsConf import WZSkimsMain
BackgroundElec = WZSkimsMain("BackgroundElec")
BackgroundElec.doBackgroundElecSkim = True
BackgroundElec.OutputLevel = INFO
SMBKGED3PDStream.filterSeq += BackgroundElec

# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMBKGED3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMBKGED3PDStream ) ]
