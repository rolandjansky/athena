# $Id$
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "JetMetEMCL D3PD cart".

# This jobO should not be included more than once:
include.block( "QcdD3PDMaker/JetMetEMCLD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
JetMetEMCLD3PDStream_msg = logging.getLogger( 'JetMetEMCLD3PD_prodJobOFragment' )

# additional settings
from RecExConfig.RecFlags import rec
from QcdD3PDMaker.QcdD3PDMakerFlags import *
QcdD3PDMakerFlagsSetEMCL()
include('QcdD3PDMaker/QcdD3PD_preSetup.py')
include('QcdD3PDMaker/QcdD3PDMakerPreSetup.common.py')

# Check if the configuration makes sense:
if prodFlags.WriteJetMetEMCLD3PD.isVirtual:
    JetMetEMCLD3PDStream_msg.error( "The Qcd D3PD stream can't be virtual! " +
                              "It's a configuration error!" )
    raise NameError( "Qcd D3PD set to be a virtual stream" )
    pass

# Construct the stream and file names for the Qcd D3PD:
streamName = prodFlags.WriteJetMetEMCLD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteJetMetEMCLD3PD )
JetMetEMCLD3PDStream_msg.info( "Configuring JetMetEMCLD3PD with streamName '%s' and fileName '%s'" % \
                         ( streamName, fileName ) )
# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
JetMetEMCLD3PDStream = MSMgr.NewRootStream( streamName, fileName, "qcd" )

# Now add all the content to this stream:
from QcdD3PDMaker.QcdD3PD import QcdD3PD
QcdD3PD( JetMetEMCLD3PDStream )

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
            JSD3PD(xx, JetMetEMCLD3PDStream)
