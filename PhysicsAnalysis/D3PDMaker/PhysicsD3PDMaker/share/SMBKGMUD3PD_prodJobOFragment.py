# $Id: SMBKGMUD3PD_prodJobOFragment.py 466232 2011-11-03 16:47:45Z aad $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMBKGMU D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMBKGMUD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMBKGMUD3PDStream_msg = logging.getLogger( 'SMBKGMUD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMBKGMUD3PD.isVirtual:
    SMBKGMUD3PDStream_msg.error( "The SMBKGMU D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMBKGMU D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMBKGMU D3PD:
streamName = prodFlags.WriteSMBKGMUD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMBKGMUD3PD )
SMBKGMUD3PDStream_msg.info( "Configuring SMBKGMUD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")


#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMBKGMUD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
SMWZD3PD( SMBKGMUD3PDStream )

# Define skims
from WZSkims.WZSkimsConf import WZSkimsMain
BackgroundMuon = WZSkimsMain("BackgroundMuon")
BackgroundMuon.doBackgroundMuonSkim = True
BackgroundMuon.OutputLevel = INFO
SMBKGMUD3PDStream.filterSeq += BackgroundMuon

# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMBKGMUD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMBKGMUD3PDStream ) ]
