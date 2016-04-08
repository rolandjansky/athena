# $Id: BackgroundD3PD_prodJobOFragment.py 467303 2011-11-10 10:14:19Z mtibbett $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Background D3PD cart".

# This jobO should not be included more than once:
include.block( "BackgroundD3PDMaker/BackgroundD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
BackgroundD3PDStream_msg = logging.getLogger( 'BackgroundD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteBackgroundD3PD.isVirtual:
    BackgroundD3PDStream_msg.error( "The Background D3PD stream can't be virtual! " +
                                    "It's a configuration error!" )
    raise NameError( "Background D3PD set to be a virtual stream" )
    pass
    
# Construct the stream and file names for the Background D3PD:
streamName = prodFlags.WriteBackgroundD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteBackgroundD3PD )
BackgroundD3PDStream_msg.info( "Configuring BackgroundD3PD with streamName '%s' and fileName '%s'" % \
                               ( streamName, fileName ) )
        
# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
BackgroundD3PDStream = MSMgr.NewRootStream( streamName, fileName, "background" )

# ESD or AOD
if rec.readESD:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetESD
    BackgroundD3PDMakerFlagsSetESD()
    BackgroundD3PDStream_msg.info("Detected ESD input so setting BackgroundD3PDMakerFlags accordingly")
elif rec.readAOD:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetAOD
    BackgroundD3PDMakerFlagsSetAOD()
    BackgroundD3PDStream_msg.info("Detected AOD input so setting BackgroundD3PDMakerFlags accordingly")
else:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetDefaults
    BackgroundD3PDMakerSetDefaults()
    BackgroundD3PDStream_msg.warning("Neither ESD or AOD input so setting BackgroundD3PDMakerFlags to default")

# pp or HI
if rec.doHeavyIon:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetHI
    BackgroundD3PDMakerFlagsSetHI()
    BackgroundD3PDStream_msg.info("Background D3PD configured for HI reconstruction, masking known absent collections")
else:
    from BackgroundD3PDMaker.BackgroundD3PDMakerFlags import BackgroundD3PDMakerFlagsSetpp
    BackgroundD3PDMakerFlagsSetpp()
    BackgroundD3PDStream_msg.info("Background D3PD configured for pp reconstruction")

#Turn off trigger
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger.set_Value_and_Lock( False )

# Now add all the content to this stream:
from BackgroundD3PDMaker.backgroundD3PD import backgroundD3PD
backgroundD3PD( BackgroundD3PDStream )
