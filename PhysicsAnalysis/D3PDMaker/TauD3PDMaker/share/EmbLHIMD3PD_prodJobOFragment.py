# $Id: EmbLHIMD3PD_prodJobOFragment.py 543602 2013-04-04 14:26:57Z mtm $
#  based on SUSYD3PD_prodJobOFragment.py, egammaD3PD_prodJobOFragment.py
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Tau D3PD cart".

# This jobO should not be included more than once:
include.block( "TauD3PDMaker/EmbLHIMD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags

from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

#Dump the trueTaus in SG
from RecExConfig.RecFlags import rec
if rec.doTruth():
    from TauD3PDMaker.TauD3PDMakerConf import TruthTausToSG
    topSequence += TruthTausToSG('truthtaus')
    topSequence.truthtaus.truth_container = 'SpclMC'
    if rec.readESD:
         topSequence.truthtaus.truth_container = 'INav4MomTruthEvent'

    #get decay mode information: dump true taus to TruthTaus in SG in special format to get decay mode
    #from TauTrackTools.TauTrackToolsConf import TauID__TruthTauCreatorAlg
    #truthTauCreatorAlg = TauID__TruthTauCreatorAlg()
    #topSequence += truthTauCreatorAlg



#Setting a few D3PDMaker flags
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.FilterCollCand = False
D3PDMakerFlags.JetSGKey = 'AntiKt4LCTopoJets'
D3PDMakerFlags.DoTrigger = False
D3PDMakerFlags.RerunTauID = True
D3PDMakerFlags.DoTJVA = True
D3PDMakerFlags.RerunTauRec = True

# Set up a logger:
from AthenaCommon.Logging import logging
TauD3PDStream_msg = logging.getLogger( 'EmbLHIMD3PD_prodJobOFragment.py' )

# Check if the configuration makes sense:
if prodFlags.WriteEmbLHIMD3PD.isVirtual:
    TauD3PDStream_msg.error( "The Tau D3PD EMBLHIM stream can't be virtual! " +
                                "It's a configuration error!" )
    raise NameError( "Tau D3PD EMBLHIM set to be a virtual stream" )
    pass

# Construct the stream and file names for the Tau D3PD:
streamName = prodFlags.WriteEmbLHIMD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteEmbLHIMD3PD )
TauD3PDStream_msg.info( "Configuring EmbLHIMD3PD with streamName '%s' and fileName '%s'" % \
                           ( streamName, fileName ) )


## Rerun Jet moments for jet area variables
from JetRec.JetRecFlags import jetFlags
jetFlags.jetPerformanceJob = True

from JetMomentTools.JetMomentsConfigHelpers import recommendedAreaAndJVFMoments
jetmomAlg = recommendedAreaAndJVFMoments('AntiKt4LCTopoJets')
                           
## Rerun B-tagging for MV3
from BTagging.BTaggingFlags import BTaggingFlags
#BTaggingFlags.CalibrationTag="BTagCalibALL-07-02" # up to now it is not linked to the global flag
BTaggingFlags.Jets=['AntiKt4LCTopo']
BTaggingFlags.CalibrationChannelAliases+=[ 'AntiKt4LCTopo->AntiKt4TopoEM' ]
BTaggingFlags.Active=True
BTaggingFlags.JetFitterCharm=True
BTaggingFlags.MV3_bVSu=False
BTaggingFlags.MV3_bVSc=False
BTaggingFlags.MV3_cVSu=False

include( "BTagging/BTagging_LoadTools.py" )
include( "BTagging/BTagging_jobOptions.py" )

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.Taggers+=["JetFitterCharm"]#,"MV3_bVSu","MV3_bVSc","MV3_cVSu"] 
JetTagD3PDFlags.JetFitterCharmTagInfo=True

# Create the D3PD stream itself:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
TauD3PDStream = MSMgr.NewRootStream( streamName, fileName, "tau" )

#re-run MET
include('TauD3PDMaker/metAlgs.py')
from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(True)

# Now add all the content to this stream:
from TauD3PDMaker.tauPerf import tauPerf
tauPerf( TauD3PDStream, doCl = False, doOtherTrk = True )



