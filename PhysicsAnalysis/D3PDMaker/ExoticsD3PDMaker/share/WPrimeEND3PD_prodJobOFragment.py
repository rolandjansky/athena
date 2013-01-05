# $Id: WPrimeEND3PD_prodJobOFragment.py 497547 2012-04-24 19:26:13Z ntsirint $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "WPrimeEN D3PD cart".

# This jobO should not be included more than once:
include.block( "PhysicsD3PDMaker/WPrimeEND3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Trigger information definition
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.egammaL1TrigPattern = ''
D3PDMakerFlags.ElectronL2TrigPattern = ''


# Set up a logger:
from AthenaCommon.Logging import logging
WPrimeEND3PDStream_msg = logging.getLogger( 'WPrimeEND3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteWPrimeEND3PD.isVirtual:
    WPrimeEND3PDStream_msg.error( "The WPrimeEN D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "WPrimeEN D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the WPrimeEN D3PD:
streamName = prodFlags.WriteWPrimeEND3PD.StreamName
fileName   = buildFileName( prodFlags.WriteWPrimeEND3PD )
WPrimeEND3PDStream_msg.info( "Configuring WPrimeEND3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#add specific containers
include ("ExoticsD3PDMaker/ElectronSelector.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
WPrimeEND3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )

# Set require algorithm
WPrimeEND3PDStream.RequireAlgs+=['WPrimeElectronSelector']

# Now add all the content to this stream:
from ExoticsD3PDMaker.WPrimeEND3PD import WPrimeEND3PD
WPrimeEND3PD( WPrimeEND3PDStream )



