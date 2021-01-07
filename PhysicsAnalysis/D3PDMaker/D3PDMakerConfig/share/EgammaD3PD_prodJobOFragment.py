# $Id: EgammaD3PD_prodJobOFragment.py 505516 2012-06-14 14:15:12Z ssnyder $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Egamma D3PD cart".

# This jobO should not be included more than once:
include.block( "D3PDMakerConfig/EgammaD3PD_prodJobOFragment.py" )

# Common import(s):
from D3PDMakerConfig import D3PDProdFlags
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
EgammaD3PDStream_msg = logging.getLogger( 'EgammaD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteEgammaD3PD.isVirtual:
    EgammaD3PDStream_msg.error( "The Egamma D3PD stream can't be virtual! " +
                                "It's a configuration error!" )
    raise NameError( "Egamma D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SUSY D3PD:
streamName = prodFlags.WriteEgammaD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteEgammaD3PD )
EgammaD3PDStream_msg.info( "Configuring EgammaD3PD with streamName '%s' and fileName '%s'" % \
                           ( streamName, fileName ) )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
EgammaD3PDStream = MSMgr.NewRootStream( streamName, fileName, "egamma", asAlg=True )
EgammaTriggerD3PDStream = MSMgr.NewRootStream( streamName + ":" + streamName +
                                               "D3PDTrigDec", fileName,
                                               "egammaTrigDec",
                                               asAlg=True)

# Now add all the content to this stream:
from D3PDMakerConfig.egammaD3PD import egammaD3PD
egammaD3PD_args = globals().get('egammaD3PD_args', {})
egammaD3PD( EgammaD3PDStream, EgammaTriggerD3PDStream, **egammaD3PD_args )
