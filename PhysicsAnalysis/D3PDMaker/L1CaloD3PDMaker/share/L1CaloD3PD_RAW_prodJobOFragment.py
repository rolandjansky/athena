# $Id$
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the L1Calo D3PD

# This jobO should not be included more than once:
include.block( "L1CaloD3PDMaker/L1CaloD3PD_RAW_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
L1CALOD3PDStream_msg = logging.getLogger( 'L1CaloD3PD_prodJobOFragment' )


# Check if the configuration makes sense:
if prodFlags.WriteL1CALOD3PD.isVirtual:
    L1CALOD3PDStream_msg.error( "The L1Calo D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "L1Calo D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the L1Calo D3PD:
streamName = prodFlags.WriteL1CALOD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteL1CALOD3PD )
L1CALOD3PDStream_msg.info( "Configuring L1CALOD3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )


# setup l1calo database
include('TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py')
svcMgr.IOVDbSvc.overrideTags +=  ["<prefix>/CALO/Identifier/CaloTTOnOffIdMapAtlas</prefix> <tag>CALOIdentifierCaloTTOnOffIdMapAtlas-0002</tag>"]

# turn off masking of bad channels
ToolSvc.TileCellBuilder.maskBadChannels = False

# The D3PD streams are created by the following function:
include("TileRecAlgs/TileCellToTTL1_jobOptions.py")
from L1CaloD3PDMaker.L1CaloD3PD import L1CaloD3PD
L1CaloD3PD( fileName, level = 3, Calibration = True, Database = True )

