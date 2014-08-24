# $Id$
#
# @file egammaD3PDMaker/share/ClusterCorrectionD3PD_prodJobOFragment.py
# @author scott snyder <snyder@bnl.gov>
# @date Oct, 2012
# @brief JO fragment for making cluster correction D3PD.
#

# This jobO should not be included more than once:
include.block( "egammaD3PDMaker/ClusterCorrectionD3PD_prodJobOFragment.py" )

# Common import(s):
from D3PDMakerConfig import D3PDProdFlags
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
ClusterCorrectionD3PDStream_msg = \
                  logging.getLogger( 'ClusterCorrectionD3PD_prodJobOFragment' )

# Construct the stream and file names for the D3PD:
streamName = prodFlags.WriteClusterCorrectionD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteClusterCorrectionD3PD )
ClusterCorrectionD3PDStream_msg.info(
    "Configuring D3PD with streamName '%s' and fileName '%s'" % 
    ( streamName, fileName ) )

# Create the D3PD stream:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
ClusterCorrectionD3PDStream = MSMgr.NewRootStream( streamName,
                                                   fileName,
                                                   'clusters' )

# Now add all the content to this stream:
from egammaD3PDMaker.ClusterCorrectionD3PD import ClusterCorrectionD3PD
ClusterCorrectionD3PD_args = globals().get('ClusterCorrectionD3PD_args', {})
ClusterCorrectionD3PD( ClusterCorrectionD3PDStream,
                       **ClusterCorrectionD3PD_args )
