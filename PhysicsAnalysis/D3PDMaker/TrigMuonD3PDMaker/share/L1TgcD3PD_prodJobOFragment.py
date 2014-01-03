# $Id: L1TgcD3PD_prodJobOFragment.py 448550 2011-07-13 11:52:10Z krasznaa $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "L1TgcD3PD"

# This jobO should not be included more than once:
include.block( "TrigMuonD3PDMaker/L1TgcD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
	
# Set up a logger:
from AthenaCommon.Logging import logging
L1TgcD3PDStream_msg = logging.getLogger( 'L1TgcD3PD_prodJobOFragment' )
	
# Check if the configuration makes sense:
if prodFlags.WriteL1TgcD3PD.isVirtual:
    L1TgcD3PDStream_msg.error( "The L1Tgc D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "L1Tgc D3PD set to be a virtual stream" )
    pass
	
# Construct the stream and file names for the L1Tgc D3PD:
streamName = prodFlags.WriteL1TgcD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteL1TgcD3PD )
L1TgcD3PDStream_msg.info( "Configuring L1TgcD3PD with streamName '%s' and fileName '%s'" % \
                             ( streamName, fileName ) )

# Create the D3PD stream(s):
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( streamName, fileName, "physics" )

# The D3PD streams are created by following function:
from TrigMuonD3PDMaker.L1TgcD3PD import L1TgcD3PD
alg = L1TgcD3PD( d3pdalg )
