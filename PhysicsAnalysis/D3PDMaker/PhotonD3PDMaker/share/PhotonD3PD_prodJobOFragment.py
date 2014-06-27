# $Id: $
#
# This jobO fragment is meant to be called by Reco_trf.py in transformation jobs to
# create the "Photon D3PD cart".

# This jobO should not be included more than once:
include.block( "PhotonD3PDMaker/PhotonD3PD_prodJobOFragment.py" )

#-------------------------------------------------------------------------------------
# Some configuration options:
#
# Some options to increase the output
#
from PhotonD3PDMaker.PhotonD3PDMakerFlags import PhotonD3PDMakerFlags

PhotonD3PDMakerFlags.JetCollections = ['AntiKt4TopoEMJets','AntiKt4LCTopoJets']
if PhotonD3PDMakerFlags.FillR0p6Jets():
    PhotonD3PDMakerFlags.JetCollections += ['AntiKt6TopoEMJets','AntiKt6LCTopoJets']

PhotonD3PDMakerFlags.EnableBTagging        = True

PhotonD3PDMakerFlags.DumpNearbyTracks      = True
PhotonD3PDMakerFlags.DumpLotsOfTriggerInfo = True

PhotonD3PDMakerFlags.Fill_Etcone_ED_corrected = [15,20,25,30,35,40]
PhotonD3PDMakerFlags.Fill_Etcone_corrected = [15,20,25,30,35,40]
PhotonD3PDMakerFlags.Fill_ED_info = True

PhotonD3PDMakerFlags.FillMV3 = False
PhotonD3PDMakerFlags.FillJetFitterCharm = True

from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.MakeEgammaUserData.set_Value_and_Lock(True)

# Some options to reduce the output
#
from TrackD3PDMaker.TrackD3PDMakerFlags import TrackD3PDFlags
TrackD3PDFlags.storeTrackInfo                               .set_Value_and_Lock(False)
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails .set_Value_and_Lock(1)
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails      .set_Value_and_Lock(1)
TrackD3PDFlags.storeTrackPredictionAtBLayer                 .set_Value_and_Lock(True)
TrackD3PDFlags.vertexPositionLevelOfDetails                 .set_Value_and_Lock(1)
TrackD3PDFlags.storeVertexKinematics                        .set_Value_and_Lock(True)
TrackD3PDFlags.storeVertexTrackIndexAssociation             .set_Value_and_Lock(True)
TrackD3PDFlags.storeVertexPurity                            .set_Value_and_Lock(False)
TrackD3PDFlags.storeVertexFitQuality                        .set_Value_and_Lock(False)
TrackD3PDFlags.storeVertexAssociation                       .set_Value_and_Lock(True)
TrackD3PDFlags.storeTrackUnbiasedIPAtPV                     .set_Value_and_Lock(True)
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails .set_Value_and_Lock(1)

# This will fill some very large branches of the track parameters for all tracks associated with each PV
TrackD3PDFlags.storeVertexTrackAssociation                  .set_Value_and_Lock(False)

# Not sure why the D3PDMaker can't figure this out.
if rec.readESD():
    CellsSGKey = D3PDMakerFlags.CellsSGKey()
else:
    CellsSGKey = "AODCellContainer"

# Can almost certainly be removed later, but needed now for b-tagging warnings
#from BTagging.BTaggingFlags import BTaggingFlags
#BTaggingFlags.CalibrationTag = "BTagCalibALL-07-02"
#-------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------
# Some algorithms that should run before the main algsequence
#
# configure MuonScatteringAngleSignificanceTool...
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

# next, let's sort some of the containers by E_T
preseq = AlgSequence(D3PDMakerFlags.PreD3PDAlgSeqName())

from PhotonD3PDMaker.makeSorterAlg import makeElectronSorterAlg,makePhotonSorterAlg

photonsorter = makePhotonSorterAlg  (D3PDMakerFlags.PhotonSGKey()  , 'PhotonContainer'  , 'SortedPhotons')
photonsorter.RemoveTopoClusterCandidates = PhotonD3PDMakerFlags.RemoveTopoClusterCandidates()
preseq += photonsorter
D3PDMakerFlags.PhotonSGKey = 'SortedPhotons,'+D3PDMakerFlags.PhotonSGKey()

preseq += makeElectronSorterAlg(D3PDMakerFlags.ElectronSGKey(), 'ElectronContainer', 'SortedElectrons')
D3PDMakerFlags.ElectronSGKey = 'SortedElectrons,'+D3PDMakerFlags.ElectronSGKey()

# Diphoton algorithm
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
PAUflags.PhotonContainer = D3PDMakerFlags.PhotonSGKey()
from AthenaCommon.AlgSequence import AlgSequence
PAUflags.AlgSequence = AlgSequence().getName()
from PhotonAnalysisUtils.DiphotonAnalysisGetter import DiphotonAnalysisGetter
dip = DiphotonAnalysisGetter()

# Add some custom MET collections
from PhotonD3PDMaker.PhotonMET import addPhotonMET
customMETCollections = addPhotonMET()

# Cell filter
from PhotonD3PDMaker.AddCellFilter import AddCellFilter
AddCellFilter(PhotonD3PDMakerFlags.SelectedCellsContainerName(), preseq)
#-------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------
# A special block just for jet configuration....
if rec.doTruth():
    PhotonD3PDMakerFlags.alltruthjets = ['AntiKt4TruthJets', 'AntiKt4TruthWithMuNoIntJets']
    if PhotonD3PDMakerFlags.FillR0p6Jets():
        PhotonD3PDMakerFlags.alltruthjets += ['AntiKt6TruthJets', 'AntiKt6TruthWithMuNoIntJets']

# this needs to come here to get the truth-jet flavor-tagging correct
from PhotonD3PDMaker.ConfigureTaggingOptions import ConfigureTaggingOptions
if PhotonD3PDMakerFlags.EnableBTagging():
    ConfigureTaggingOptions(PhotonD3PDMakerFlags.alltruthjets(),
                            PhotonD3PDMakerFlags.FillMV3(),
                            PhotonD3PDMakerFlags.FillJetFitterCharm())

# Create some new truth-jet collections
#
from JetRec.JetRecFlags import jetFlags
jetFlags.doBTagging = False
if rec.doTruth():
    
    from JetRec.JetGetters import make_StandardJetGetter
    antikt4truthAlg = make_StandardJetGetter('AntiKt',
                                             0.4,
                                             'Truth',
                                             globalSuff='WithMuNoInt',
                                             disable=False,
                                             includeMuons=True,
                                             useInteractingOnly=False,
                                             seq = preseq).jetAlgorithmHandle()
    
    if PhotonD3PDMakerFlags.FillR0p6Jets():
        antikt6truthAlg = make_StandardJetGetter('AntiKt',
                                                 0.6,
                                                 'Truth',
                                                 disable=False,
                                                 seq = preseq).jetAlgorithmHandle() 
        
        antikt6truthAlg = make_StandardJetGetter('AntiKt',
                                                 0.6,
                                                 'Truth',
                                                 globalSuff='WithMuNoInt',
                                                 disable=False,
                                                 includeMuons=True,
                                                 useInteractingOnly=False,
                                                 seq = preseq).jetAlgorithmHandle()

    if PhotonD3PDMakerFlags.RetagTruthBJets() and PhotonD3PDMakerFlags.EnableBTagging():
        if not 'JetTagD3PD_CollectionPostfix' in dir():
            PhotonD3PDMakerFlags.JetTagD3PD_CollectionPostfix=PhotonD3PDMakerFlags.BTaggingSuffix()
    elif not 'JetTagD3PD_CollectionPostfix' in dir():
        PhotonD3PDMakerFlags.JetTagD3PD_CollectionPostfix=""

if PhotonD3PDMakerFlags.EnableBTagging():
    # now we can load the b-tagging algorithms
    jetFlags.doBTagging = True
    
    include( "BTagging/BTagging_jobOptions.py" )

    # however, the algorithms are added to the topSequence, which is useless for us,
    # since we need to have the information available for an already-defined algorithm.
    # so, take them out of the topSequence, and add them to the presequence.
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    for algitr in topSequence:
        if algitr.getName().find("BJetBuilder") >= 0:
            preseq += algitr
            topSequence.remove(algitr)

if PhotonD3PDMakerFlags.ReCalculateJetMoments() or PhotonD3PDMakerFlags.FillFullJVF():
    # this just gets the JVF tool so we can reconfigure it
    from JetMomentTools.SetupJetMomentTools  import getJetVertexAssociationTool
    jvatool = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo') # apparently the parameters here don't matter
    
    # this will change _jvtxf, _jvtxfFull
    jvatool.AssociateToHighestJVF = False
    jvatool.JetVtxTrackHelper.UniqueTrkVtxAssocBySumPt = False
#-------------------------------------------------------------------------------------


#-------------------------------------------------------------------------------------
# Construct the stream and file names for the Photon D3PD:
# Common import(s):
from AthenaCommon.JobProperties import jobproperties
prodFlags = jobproperties.D3PDProdFlags
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName

# Set up a logger:
from AthenaCommon.Logging import logging
PhotonD3PDStream_msg = logging.getLogger( 'PhotonD3PD_prodJobOFragment' )

# Check if the configuration makes sense:
if prodFlags.WritePhotonD3PD.isVirtual:
    PhotonD3PDStream_msg.error( "The Photon D3PD stream can't be virtual! " +
                                "It's a configuration error!" )
    raise NameError( "Photon D3PD set to be a virtual stream" )
    pass

streamName = prodFlags.WritePhotonD3PD.StreamName
fileName   = buildFileName( prodFlags.WritePhotonD3PD )
PhotonD3PDStream_msg.info( "Configuring PhotonD3PD with streamName '%s' and fileName '%s'" % \
                           ( streamName, fileName ) )
# Create the D3PD streams:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
PhotonD3PDStream = MSMgr.NewRootStream( streamName, fileName, "photon" )

# Now add all the content to this stream:
from PhotonD3PDMaker.PhotonD3PD import PhotonD3PD
PhotonD3PD( PhotonD3PDStream,
            CustomMETCollections = customMETCollections)
#-------------------------------------------------------------------------------------
