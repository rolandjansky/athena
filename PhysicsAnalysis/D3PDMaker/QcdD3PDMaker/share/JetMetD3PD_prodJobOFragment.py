# $Id$
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "JetMet D3PD cart".

# This jobO should not be included more than once:
include.block( "QcdD3PDMaker/JetMetD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
JetMetD3PDStream_msg = logging.getLogger( 'JetMetD3PD_prodJobOFragment' )

# additional settings
from RecExConfig.RecFlags import rec
#rec.UserAlgs+=['QcdD3PDMaker/QcdD3PD_preSetup.py']
include('QcdD3PDMaker/QcdD3PD_preSetup.py')
include('QcdD3PDMaker/QcdD3PDMakerPreSetup.common.py')

# Check if the configuration makes sense:
if prodFlags.WriteJetMetD3PD.isVirtual:
    JetMetD3PDStream_msg.error( "The Qcd D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "Qcd D3PD set to be a virtual stream" )
    pass

# Adding specific containers
include ('QcdD3PDMaker/ElectronSelector.py')
include ('QcdD3PDMaker/PhotonSelector.py')
#include ('QcdD3PDMaker/TrackParticleSelector.py')
#include ('QcdD3PDMaker/TruthParticleSelector.py')

# Construct the stream and file names for the Qcd D3PD:
streamName = prodFlags.WriteJetMetD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteJetMetD3PD )
JetMetD3PDStream_msg.info( "Configuring JetMetD3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )
# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
JetMetD3PDStream = MSMgr.NewRootStream( streamName, fileName, "qcd" )

# Now add all the content to this stream:
from QcdD3PDMaker.QcdD3PD import QcdD3PD
QcdD3PD( JetMetD3PDStream,
         stdElectronContainer = 'GoodElectrons',
         stdPhotonContainer = 'GoodPhotons')
#         stdTrackParticleContainer = 'GoodTracks')
#         stdTruthParticleContainer = 'GoodTruthParticles' )

### large-R jets
from RecExConfig.ObjKeyStore import objKeyStore
if objKeyStore.isInInput("CaloClusterContainer","CaloCalTopoCluster"):
    preseq = AlgSequence (D3PDMakerFlags.PreD3PDAlgSeqName())
    from QcdD3PDMaker.JSjets import createJSJets
    from QcdD3PDMaker.JSD3PD import JSD3PD

    from QcdD3PDMaker.GroomedJetsConfig import getGroomedJetsConfig
    dictsConfig = getGroomedJetsConfig()
    for dC in dictsConfig:
        xx = createJSJets(dC[0], dC[1], preseq)
        if xx[0] != None and xx[1] != None:
            JSD3PD(xx, JetMetD3PDStream)
