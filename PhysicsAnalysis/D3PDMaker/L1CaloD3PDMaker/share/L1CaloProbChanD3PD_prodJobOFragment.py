# $Id$
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the L1Calo D3PD

# This jobO should not be included more than once:
include.block( "L1CaloD3PDMaker/L1CaloProbChanD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
L1CALOPROBD3PDStream_msg = logging.getLogger( 'L1CaloProbChanD3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteL1CALOPROBD3PD.isVirtual:
    L1CALOPROBD3PDStream_msg.error( "The L1Calo Problem Channel D3PD stream can't be virtual! " +
                                    "It's a configuration error!" )
    raise NameError( "L1Calo Problem Channel D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the L1Calo D3PD:
streamName = prodFlags.WriteL1CALOPROBD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteL1CALOPROBD3PD )
L1CALOPROBD3PDStream_msg.info( "Configuring L1CALOPROBD3PD with streamName '%s' and fileName '%s'" % \
                               ( streamName, fileName ) )

# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
L1CALOPROBD3PDStream = MSMgr.NewRootStream( streamName, fileName, "L1CaloTree" )

include("TileRecAlgs/TileCellToTTL1_jobOptions.py")
from L1CaloD3PDMaker.L1CaloD3PDProblemChannel import L1CaloD3PDProblemChannel
L1CaloD3PDProblemChannel( L1CALOPROBD3PDStream )
