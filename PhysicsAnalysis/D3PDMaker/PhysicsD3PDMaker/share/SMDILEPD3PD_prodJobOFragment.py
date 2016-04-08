# $Id: SMDILEPD3PD_prodJobOFragment.py 466232 2011-11-03 16:47:45Z aad $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "SMDILEP D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/SMDILEPD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
SMDILEPD3PDStream_msg = logging.getLogger( 'SMDILEPD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteSMDILEPD3PD.isVirtual:
    SMDILEPD3PDStream_msg.error( "The SMDILEP D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "SMDILEP D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SMDILEP D3PD:
streamName = prodFlags.WriteSMDILEPD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteSMDILEPD3PD )
SMDILEPD3PDStream_msg.info( "Configuring SMDILEPD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")


# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
SMDILEPD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Now add all the content to this stream:
from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
SMWZD3PD( SMDILEPD3PDStream )

# Define skims
from WZSkims.WZSkimsConf import WZSkimsMain
TwoLeptons = WZSkimsMain("TwoLeptons")
TwoLeptons.doTwoLeptonsSkim = True
TwoLeptons.OutputLevel = INFO
SMDILEPD3PDStream.filterSeq += TwoLeptons

# add cut flow information
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig
SMDILEPD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SMDILEPD3PDStream ) ]
