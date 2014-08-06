# $Id: TriggerD3PD_prodJobOFragment.py 593763 2014-04-22 15:03:09Z fpastore $
#
# Top jobO fragment setting up the common Trigger D3PD (NTUP_TRIG).
#
# @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

# This jobO should not be included more than once:
include.block( "TriggerD3PDMaker/TriggerD3PD_prodJobOFragment.py" )

# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from RecExConfig.ObjKeyStore import cfgKeyStore
from RecExConfig.RecFlags import rec
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags

# Set up a logger:
from AthenaCommon.Logging import logging
TriggerD3PDStream_msg = logging.getLogger( 'TriggerD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WriteTriggerD3PD.isVirtual:
    TriggerD3PDStream_msg.error( "The Trigger D3PD stream can't be virtual! " +
                                 "It's a configuration error!" )
    raise NameError( "Trigger D3PD set to be a virtual stream" )

# Construct the stream and file names for the Trigger D3PD:
streamName = prodFlags.WriteTriggerD3PD.StreamName
fileName   = buildFileName( prodFlags.WriteTriggerD3PD )
TriggerD3PDStream_msg.info( "Configuring Trigger D3PD with streamName '%s' and " \
                            "fileName '%s'" % ( streamName, fileName ) )

# Check if we will get trigger information:
triggerAvailable = ( rec.doTrigger() and ( not rec.noESDTrigger() ) )

# Print a warning if the trigger is not used:
if not triggerAvailable:
    TriggerD3PDStream_msg.warning( "Running without trigger information on the input. "
                                   "Most information will not be available/saved." )
    pass

# Set global D3PDMaker flags:
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoPAU     = False
D3PDMakerFlags.DoTrigger = True
#FPP: patch to run D3PD with the merged L2EF (must add new flags in D3PDMakerConfig/python/D3PDMakerFlags.py)
D3PDMakerFlags.ElectronEFTrigPattern = "EF_[0-9]?e[0-9]+.*|HLT_[0-9]?e[0-9]+.*"
D3PDMakerFlags.ElectronL2TrigPattern = "L2_[0-9]?e[0-9]+.*"
D3PDMakerFlags.PhotonEFTrigPattern   = "EF_[0-9]?g[0-9]+.*|HLT_[0-9]?e[0-9]+.*"
D3PDMakerFlags.PhotonL2TrigPattern   = "L2_[0-9]?g[0-9]+.*"
D3PDMakerFlags.egammaL1TrigPattern   = ""
D3PDMakerFlags.MuonEFTrigPattern   = 'EF_2?mu.*|EF_L1ItemStreamer_L1_2?MU.*|HLT_2?mu.*|HLT_L1ItemStreamer_L1_2?MU.*'

from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
PAUflags.DoReRunJetFinding = False

# Create the D3PD stream(s):
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
d3pdalg = MSMgr.NewRootStream( streamName, fileName, "trigger" )
#trigalg = MSMgr.NewRootStream( streamName + ":" + streamName + "TrigDec",
#                               fileName, "triggerDec" )
#d3pdalg.Stream.trigDecisionTree = trigalg

##
## Basic event information:
##
from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject
d3pdalg += EventInfoD3PDObject( 2 )
#trigalg += EventInfoD3PDObject( 0, exclude = [ "eventMu", "eventType" ] )
if triggerAvailable:
    from TriggerD3PDMaker.BGCodeD3PDObject import BGCodeD3PDObject
    d3pdalg += BGCodeD3PDObject( 0 )
    pass

##
## Add the encoded trigger decision. It has to be done at the beginning,
## because some of the following functions would add it with a lower
## detail level.
##
if triggerAvailable:
    from TriggerD3PDMaker.TrigDecisionD3PDObject import TrigDecisionD3PDObject
    d3pdalg += TrigDecisionD3PDObject( 10 )
    pass

##
## Offline e/gamma information:
##
oflEgammaIncludes = [ 'ClusterKin' ]
from egammaD3PDMaker.ElectronD3PDObject import ElectronD3PDObject
added = False
for sgkey in D3PDMakerFlags.ElectronSGKey().split( "," ):
    if cfgKeyStore.isInInput( "ElectronContainer", sgkey ):
        d3pdalg += ElectronD3PDObject( 1, include=oflEgammaIncludes )
        added = True
        break
    pass
if not added:
    TriggerD3PDStream_msg.warning( "Offline electrons not found in input" )
    pass
from egammaD3PDMaker.PhotonD3PDObject import PhotonD3PDObject
added = False
for sgkey in D3PDMakerFlags.PhotonSGKey().split( "," ):
    if cfgKeyStore.isInInput( "PhotonContainer", sgkey ):
        d3pdalg += PhotonD3PDObject( 1, include=oflEgammaIncludes+['Samplings'],
                                     Samplings_Samplings=[2],
                                     Samplings_WriteEnergy=False,
                                     Samplings_WritePhi=False)
        added = True
        break
    pass
if not added:
    TriggerD3PDStream_msg.warning( "Offline photons not found in input" )
    pass

##
## E/gamma trigger information:
##
if triggerAvailable:
    from TrigEgammaD3PDMaker.TrigEgammaD3PD import TrigEgammaD3PDObjects
    TrigEgammaD3PDObjects( d3pdalg, level = 1, EmTauROI_include = [ "RoIWord" ],
                           TrigEMCluster_include = [ "Details", "Details1" ] )
    pass

##
## Offline muon information:
##
from MuonD3PDMaker.MuonD3PDObject import MuonD3PDObject
oflMuonIncludes = [ 'EFCBInfoIndex',
                    'EFMGInfoIndex', 'EFMEInfoIndex', 'L2CBInfoIndex', 'L1InfoIndex' ]
oflMuonExcludes = [ 'EFCBInfo', 'EFMGInfo', 'EFMEInfo', 'L2CBInfo', 'L1Info',
                    'TruthInfoIndex' ]
if not cfgKeyStore.isInInput( "VxContainer",
                              "VxPrimaryCandidate" ):
    oflMuonExcludes += [ "ParametersAtPV", "PrimaryTrackDiagonalCovarianceAtPV",
                         "PrimaryTrackOffDiagonalCovarianceAtPV",
                         "TrkInfoInDet" ]
    pass
if cfgKeyStore.isInInput( "Analysis::MuonContainer",
                          "StacoMuonCollection" ):
    d3pdalg += MuonD3PDObject( 1, sgkey='StacoMuonCollection', prefix='mu_staco_',
                               include=oflMuonIncludes, exclude=oflMuonExcludes )
else:
    TriggerD3PDStream_msg.warning( "Offline muons not found in input" )
    pass

##
## Muon trigger information:
##
if triggerAvailable:
    from TrigMuonD3PDMaker.TrigMuonD3PD import TrigMuonD3PDObjects
    TrigMuonD3PDObjects( d3pdalg, level = 1, addNaviInfo = True,
                         addConfigInfo = False )
    pass

from TriggerD3PDMaker.MuCTPID3PDObject import MuCTPID3PDObject
d3pdalg += MuCTPID3PDObject( 1 )

##
## Offline tau information:
##
from TauD3PDMaker.TauD3PDObject import TauD3PDObject
if cfgKeyStore.isInInput( "Analysis::TauJetContainer",
                          D3PDMakerFlags.TauSGKey() ):
    d3pdalg += TauD3PDObject( 1 )
else:
    TriggerD3PDStream_msg.warning( "Offline taus not found in input" )
    pass

##
## Tau trigger information:
##
if triggerAvailable:
    from TrigTauD3PDMaker.TrigTauD3PD import TrigTauD3PDObjects
    TrigTauD3PDObjects( d3pdalg, level = 1, addNavInfo = True,
                        addConfigInfo = False )
    pass

##
## Offline jet information with b-tag info:
##
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDKeys
JetTagD3PDFlags.Taggers = [ "IP1D","IP2D","IP3D",
                            "SV0","SV1", "SV2" ]
jet_includes = [ JetTagD3PDKeys.BTagWeightsBlockName(),
                 JetTagD3PDKeys.SVInfoPlusBlockName(),
                 JetTagD3PDKeys.SV0InfoPlusBlockName(),
                 'JetVertexFraction',
                 'JVtx'
                 ]

from JetD3PDMaker.JetD3PDObject import getJetD3PDObject
if cfgKeyStore.isInInput( "JetCollection",
                          "AntiKt4TopoEMJets" ):
    JetD3PDObject = getJetD3PDObject()
    d3pdalg += JetD3PDObject( 1, sgkey = "AntiKt4TopoEMJets",
                              prefix = "jet_AntiKt4TopoEMJets_",
                              include = jet_includes,
                              JetTag_SoftMuonChi2InfoMuonAssoc_target = "mu_staco_" )
else:
    TriggerD3PDStream_msg.warning( "Offline jets not found in input" )
    pass

##
## Jet trigger information:
##
if triggerAvailable:
    from TrigJetD3PDMaker.TrigJetD3PD import TrigJetD3PDObjects
    TrigJetD3PDObjects( d3pdalg, level = 1, addNaviInfo = True,
                        addConfigInfo = False )
    pass

##
## B-Jet trigger information:
##
if triggerAvailable:
    from TrigBJetD3PDMaker.TrigBJetD3PD import TrigBJetD3PDObjects
    TrigBJetD3PDObjects( d3pdalg, level = 1, addNaviInfo = True,
                         addConfigInfo = False )
    pass

##
## Offline MET information:
##
from MissingETD3PDMaker.MissingETD3PDObject import MissingETD3PDObject
d3pdalg += MissingETD3PDObject( 3, allowMissing = True )

##
## MET trigger information:
##
if triggerAvailable:
    from TrigMissingETD3PDMaker.TrigMETD3PD import TrigMETD3PDObjects
    TrigMETD3PDObjects( d3pdalg )
    pass

##
## Additional offline information:
##
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.vertexPositionLevelOfDetails = 1
TrackD3PDFlags.storeVertexTrackIndexAssociation = False
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics = True

from TrackD3PDMaker.VertexD3PDObject import PrimaryVertexD3PDObject
if cfgKeyStore.isInInput( "VxContainer",
                          "VxPrimaryCandidate" ):
    d3pdalg += PrimaryVertexD3PDObject( 0, prefix = "vxp_" )
else:
    TriggerD3PDStream_msg.warning( "Offline primary vertices not found in input" )
    pass

##
## Add the different sorts of metadata:
##
from EventCommonD3PDMaker.LBMetadataConfig import LBMetadataConfig
d3pdalg.MetadataTools += [ LBMetadataConfig() ]
if triggerAvailable:
    from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata
    addTrigConfMetadata( d3pdalg )
    pass
from TriggerD3PDMaker.BunchStructureMetadata import addBunchStructureMetadata
addBunchStructureMetadata( d3pdalg )

##
## Turn on all the simple trigger decision branches:
##
bitFiller = getattr( d3pdalg, "TriggerBitsFiller", None )
if bitFiller:
    bitFiller.TriggerBitsFiller_TriggerBits.Triggers = [ "L1_.*", "L2_.*", "EF_.*", "HLT_.*" ]
    pass

##
## Silence the trigger navigation warnings about missing containers:
##
from AthenaCommon.AppMgr import ToolSvc
if not hasattr( ToolSvc, "TrigDecisionTool" ):
    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
    ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
    pass
ToolSvc.TrigDecisionTool.Navigation.OutputLevel = ERROR
