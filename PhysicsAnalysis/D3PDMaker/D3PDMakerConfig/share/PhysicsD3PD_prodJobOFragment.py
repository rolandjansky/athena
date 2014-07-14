# $Id: PhysicsD3PD_prodJobOFragment.py 505516 2012-06-14 14:15:12Z ssnyder $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Physics D3PD cart".

# This jobO should not be included more than once:
include.block( "D3PDMakerConfig/PhysicsD3PD_prodJobOFragment.py" )

# Common import(s):
from D3PDMakerConfig import D3PDProdFlags
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
PhysicsD3PDStream_msg = logging.getLogger( 'PhysicsD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WritePhysicsD3PD.isVirtual:
    PhysicsD3PDStream_msg.error( "The Physics D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "Physics D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the SUSY D3PD:
streamName = prodFlags.WritePhysicsD3PD.StreamName
fileName   = buildFileName( prodFlags.WritePhysicsD3PD )
PhysicsD3PDStream_msg.info( "Configuring PhysicsD3PD with streamName '%s' and fileName '%s'" % \
                            ( streamName, fileName ) )

# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
PhysicsD3PDStream = MSMgr.NewRootStream( streamName, fileName, "physics" )
PhysicsTriggerD3PDStream = MSMgr.NewRootStream( streamName + ":" + streamName +
                                                "TrigDec", fileName,
                                                "physicsTrigDec" )

# Now add all the content to this stream:
from D3PDMakerConfig.physicsD3PD import physicsD3PD
physicsD3PD_args = globals().get('physicsD3PD_args', {})
physicsD3PD( PhysicsD3PDStream, PhysicsTriggerD3PDStream, **physicsD3PD_args )
