# $Id$
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Qcd D3PD cart".

# This jobO should not be included more than once:
include.block( "QcdD3PDMaker/QcdD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
QcdD3PDStream_msg = logging.getLogger( 'QcdD3PD_prodJobOFragment' )

# additional settings
from RecExConfig.RecFlags import rec
from QcdD3PDMaker.QcdD3PDMakerFlags import *
QcdD3PDMakerFlagsSetPhysics()
include('QcdD3PDMaker/QcdD3PD_preSetup.py')
include('QcdD3PDMaker/QcdD3PDMakerPreSetup.common.py')

# Check if the configuration makes sense:
if prodFlags.WriteQcdD3PD.isVirtual:
    QcdD3PDStream_msg.error( "The Qcd D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "Qcd D3PD set to be a virtual stream" )
    pass

# Adding specific containers
include ('QcdD3PDMaker/ElectronSelector.py')
include ('QcdD3PDMaker/PhotonSelector.py') 
include ('QcdD3PDMaker/TrackParticleSelector.py') 
include ('QcdD3PDMaker/TruthParticleSelector.py') 

# Construct the stream and file names for the Qcd D3PD:
streamName = prodFlags.WriteQcdD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteQcdD3PD )
QcdD3PDStream_msg.info( "Configuring QcdD3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )
# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
QcdD3PDStream = MSMgr.NewRootStream( streamName, fileName, "qcd" )

# Now add all the content to this stream:
from QcdD3PDMaker.QcdD3PD import QcdD3PD
QcdD3PD( QcdD3PDStream,
         stdElectronContainer = 'GoodElectrons',
         stdPhotonContainer = 'GoodPhotons',
         stdTrackParticleContainer = 'GoodTracks',
         stdTruthParticleContainer = 'GoodTruthParticles' )


