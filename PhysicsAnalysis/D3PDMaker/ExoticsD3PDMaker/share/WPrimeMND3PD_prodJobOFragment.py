# $Id: WPrimeMND3PD_prodJobOFragment.py $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "WPrimeMN D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/WPrimeMND3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

#Trigger information definition
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.MuonEFTrigPattern = 'EF_mu36_tight|EF_mu50_MSonly_barrel_tight|EF_mu40_tight'


# Set up a logger:
from AthenaCommon.Logging import logging
WPrimeMND3PDStream_msg = logging.getLogger( 'WPrimeMND3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteWPrimeMND3PD.isVirtual:
    WPrimeMND3PDStream_msg.error( "The WPrimeMN D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "WPrimeMN D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the WPrimeMN D3PD:
streamName = prodFlags.WriteWPrimeMND3PD.StreamName
fileName   = buildFileName( prodFlags.WriteWPrimeMND3PD )
WPrimeMND3PDStream_msg.info( "Configuring WPrimeMND3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#add specific containers
include ("ExoticsD3PDMaker/MuonSelector.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
WPrimeMND3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Set require algorithm
WPrimeMND3PDStream.AcceptAlgs+=['WPrimeStacoMuonSelector']
WPrimeMND3PDStream.AcceptAlgs+=['WPrimeMuonsSelector']

# Now add all the content to this stream:
from ExoticsD3PDMaker.WPrimeMND3PD import WPrimeMND3PD
WPrimeMND3PD( WPrimeMND3PDStream )

