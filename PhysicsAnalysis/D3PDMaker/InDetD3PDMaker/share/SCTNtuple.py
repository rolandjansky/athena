#import MinBiasD3PDMaker
import TrackD3PDMaker
import InDetD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TriggerD3PDMaker

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags       import TrackD3PDFlags
from InDetD3PDMaker.InDetD3PDMakerSCTFlags    import InDetD3PDSCTFlags

from InDetD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
from InDetD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject

### added to enure the TrackD3PD generic track variables (14.04.2011)
TrackD3PDFlags.vertexPositionLevelOfDetails=1
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics=True
TrackD3PDFlags.storeVertexPurity=False
TrackD3PDFlags.storeVertexTrackAssociation=False
TrackD3PDFlags.storeVertexTrackIndexAssociation=False
TrackD3PDFlags.doTruth = True

TrackD3PDFlags.storeTrackSummary = True
TrackD3PDFlags.storeTrackSummary.IDHits = True
TrackD3PDFlags.storeTrackSummary.IDHoles = True
TrackD3PDFlags.storeTrackSummary.IDSharedHits = False
TrackD3PDFlags.storeTrackSummary.IDOutliers = False
TrackD3PDFlags.storeTrackSummary.PixelInfoPlus = False
TrackD3PDFlags.storeTrackSummary.SCTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.TRTInfoPlus = False
TrackD3PDFlags.storeTrackSummary.InfoPlus = False
TrackD3PDFlags.storeTrackSummary.MuonHits = False #default True
TrackD3PDFlags.storeTrackSummary.MuonHoles = False
TrackD3PDFlags.storeTrackSummary.ExpectBLayer = False #default True
TrackD3PDFlags.storeTrackPredictionAtBLayer = False
TrackD3PDFlags.storeTrackSummary.HitSum = True
TrackD3PDFlags.storeTrackSummary.HoleSum = True
TrackD3PDFlags.storeTrackSummary.HitPattern = True
TrackD3PDFlags.storeTrackSummary.SiHits = False
TrackD3PDFlags.storeTrackSummary.TRTRatio = True
TrackD3PDFlags.storeTrackSummary.PixeldEdx = True #default False
TrackD3PDFlags.storeTrackSummary.ElectronPID = False
####
## Fix impact parameters
TrackD3PDFlags.trackParametersAtGlobalPerigeeLevelOfDetails=0
TrackD3PDFlags.trackParametersAtPrimaryVertexLevelOfDetails=2
TrackD3PDFlags.trackParametersAtBeamSpotLevelOfDetails=0
TrackD3PDFlags.storeTrackUnbiasedIPAtPV=False
## Fix track variables
TrackD3PDFlags.storeTrackMomentum=True
TrackD3PDFlags.storeTrackInfo=True
TrackD3PDFlags.storeTrackFitQuality=True
## Fix the rest
TrackD3PDFlags.storePullsAndResiduals=False
TrackD3PDFlags.storeBLayerHitsOnTrack=False
TrackD3PDFlags.storePixelHitsOnTrack=False
TrackD3PDFlags.storeSCTHitsOnTrack=False
TrackD3PDFlags.storeTRTHitsOnTrack=False
TrackD3PDFlags.storeBLayerOutliersOnTrack=False
TrackD3PDFlags.storePixelOutliersOnTrack=False
TrackD3PDFlags.storeTRTOutliersOnTrack=False
TrackD3PDFlags.storeBLayerHolesOnTrack=False
TrackD3PDFlags.storePixelHolesOnTrack=False   
TrackD3PDFlags.storeSCTHolesOnTrack=False
TrackD3PDFlags.storeTRTHolesOnTrack=False
TrackD3PDFlags.storeVertexAssociation=False

from EventCommonD3PDMaker.EventInfoD3PDObject import EventInfoD3PDObject

from InDetD3PDMaker.SCTClusterD3PDObject      import SCTClusterD3PDObject     #new
from InDetD3PDMaker.SCTRDOD3PDObject          import SCTRDOD3PDObject         #new
from InDetD3PDMaker.TRTPhaseD3PDObject        import TRTPhaseD3PDObject
from InDetD3PDMaker.SctHitOnTrackAssociation  import SctHitOnTrackAssociation #new

#primary vertex
from InDetD3PDMaker.VertexD3PDObject          import PrimaryVertexD3PDObject 


from InDetD3PDMaker.TrackParticleD3PDObjects  import TrackParticleD3PDObject
#from MinBiasD3PDMaker.BcmRdoD3PDObject        import BcmRdoD3PDObject
from TrigMbD3PDMaker.BcmRdoD3PDObject         import BcmRdoD3PDObject

#Trigger
#from MinBiasD3PDMaker.CtpRdoD3PDObject        import CtpRdoD3PDObject
#from MinBiasD3PDMaker.CtpDecisionD3PDObject   import CtpDecisionD3PDObject
from TrigMbD3PDMaker.CtpRdoD3PDObject        import CtpRdoD3PDObject
from TrigMbD3PDMaker.CtpDecisionD3PDObject    import CtpDecisionD3PDObject
from TriggerD3PDMaker.TrigDecisionD3PDObject  import TrigDecisionD3PDObject
from TriggerD3PDMaker.TrigConfMetadata import addTrigConfMetadata

## Associations
from D3PDMakerCoreComps.D3PDObject            import D3PDObject
from D3PDMakerCoreComps.IndexAssociation      import IndexAssociation
from D3PDMakerCoreComps.IndexMultiAssociation import IndexMultiAssociation
from D3PDMakerCoreComps.SimpleAssociation     import SimpleAssociation

## Athena imports
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
########################################################################################
# Configure SCT Ntuple tree

print "NICK here i am"

InDetD3PDSCTFlags.storeEventInfo = True
InDetD3PDSCTFlags.storeBSerror = True
InDetD3PDSCTFlags.storeTracks = False
InDetD3PDSCTFlags.storeSctClusters = True
InDetD3PDSCTFlags.storeSctRDOs = True
InDetD3PDSCTFlags.storeTriggerInfo = False
if globalflags.InputFormat() == 'bytestream' and hasattr(runArgs,"topOptions"): 
    InDetD3PDSCTFlags.storeTriggerInfo = False
elif globalflags.InputFormat() == 'pool' and  globalflags.DataSource() == 'geant4' and hasattr(runArgs,"topOptions"):
    InDetD3PDSCTFlags.storeTriggerInfo = False
    
InDetD3PDSCTFlags.doTrackToMuonAssociation = False
InDetD3PDSCTFlags.storePhysicsInfo = False
InDetD3PDSCTFlags.storeBCMrdo = False
InDetD3PDSCTFlags.includeTruthInfo = False
InDetD3PDSCTFlags.storeHitsOnTracks = False
InDetD3PDSCTFlags.doTrackToClusterAssociation = False
InDetD3PDSCTFlags.doClusterToTrackAssociation = False
InDetD3PDSCTFlags.doRdoToClusterAssociation = True
InDetD3PDSCTFlags.doClusterToRdoAssociation = True


if InDetD3PDSCTFlags.doSCTNtupleLight:
    InDetD3PDSCTFlags.doSCTNtuple = False
    InDetD3PDSCTFlags.doSCTNtupleExtended = False
elif InDetD3PDSCTFlags.doSCTNtupleExtended:
    InDetD3PDSCTFlags.doSCTNtupleLight = False
    InDetD3PDSCTFlags.doSCTNtuple = False



if InDetD3PDSCTFlags.doSCTNtupleLight:
    InDetD3PDSCTFlags.storeSctClusters = False
    InDetD3PDSCTFlags.storeSctRDOs = False
    if InDetD3PDSCTFlags.storeTracks:
        InDetD3PDSCTFlags.storeHitsOnTracks = True
        InDetD3PDSCTFlags.doTrackToMuonAssociation = False
        InDetD3PDSCTFlags.doTrackToClusterAssociation = False
        InDetD3PDSCTFlags.doClusterToTrackAssociation = False
        InDetD3PDSCTFlags.doRdoToClusterAssociation = False
        InDetD3PDSCTFlags.doClusterToRdoAssociation = False

elif InDetD3PDSCTFlags.doSCTNtupleExtended:
    InDetD3PDSCTFlags.storeBCMrdo = True

    
## D3PD Maker alg ########################################################################################
from InDetRecExample.InDetKeys import InDetKeys
name = InDetD3PDSCTFlags.outputFile() #'SctNtuple_'+InDetKeys.trkD3PDFileName()

print InDetD3PDSCTFlags

print TrackD3PDFlags

#print "1 ", name 
#
#if InDetD3PDSCTFlags.doSCTNtupleLight and True:
#    name = 'Light_'+InDetD3PDSCTFlags.outputFile()
#    print "2 ", name 
#elif InDetD3PDSCTFlags.doSCTNtupleExtended and True:
#    name = 'Extended_'+InDetD3PDSCTFlags.outputFile()
#    print "3 ", name
    
alg = D3PDMakerCoreComps.MakerAlg('InDetTrackTree',
                                  topSequence,
                                  name)

#print "4 ", name

##########################################################################################################
## Track to cluster association for full/extended version 
if InDetD3PDSCTFlags.doTrackToClusterAssociation:
    SCTClusterAssoc = IndexMultiAssociation(TrackParticleD3PDObject,
                                            InDetD3PDMaker.TrackParticleToSCTClusterAssociationTool,
                                            'SCTClus','clAssoc_',level = 1)

##########################################################################################################
## Cluster on Tracks for light version 
if InDetD3PDSCTFlags.storeHitsOnTracks:
    LightAssociationTool = InDetD3PDMaker.TrackParticleToSCTClusterAssociationToolLight
    SCT_clAssocLight     = SctHitOnTrackAssociation\
                           (TrackParticleD3PDObject,
                            LightAssociationTool,
                            'clAssoc_', 'clAssoc_', 0) 
## SCT CLusters ##########################################################################################
## Set up associaations for full/extended version 
if InDetD3PDSCTFlags.doClusterToRdoAssociation:
    SCTRDOAssoc = IndexMultiAssociation(SCTClusterD3PDObject,
                                        InDetD3PDMaker.SCTClusterSCTRDOAssociationTool,
                                        'SCTRdo','rdoAssoc_',level = 1)

if InDetD3PDSCTFlags.doClusterToTrackAssociation:
    TrackAssoc = IndexMultiAssociation(SCTClusterD3PDObject,
                                       InDetD3PDMaker.SCTClusterToTrackParticleAssociationTool,
                                       'trk','trkAssoc_',level = 1,
                                       TrackParticleContainerSGKey = D3PDMakerFlags.TrackSGKey())

## Muons ##########################################################################################
## TrackParticle to muon association
if InDetD3PDSCTFlags.doTrackToMuonAssociation:
    if InDetD3PDSCTFlags.useStacoMuonCollectionForAssociation==True:
        MuonAssoc = SimpleAssociation(TrackParticleD3PDObject,
                                      TrackD3PDMaker.SCTTrackParticleToMuonAssociationTool,
                                      matched = 'hasMuon',
                                      MuonContainerSGKey = "StacoMuonCollection")
    else:
        MuonAssoc = SimpleAssociation(TrackParticleD3PDObject,
                                      TrackD3PDMaker.SCTTrackParticleToMuonAssociationTool,
                                      matched = 'hasMuon',
                                      MuonContainerSGKey = "Muons")

## SCT RDOs ##############################################################################################
## Set up associations for full/extended version    
if InDetD3PDSCTFlags.doRdoToClusterAssociation:
    clasterAssoc = IndexAssociation(SCTRDOD3PDObject,InDetD3PDMaker.SCTRDOClusterAssociationTool,
                                    'SCTClus','clAssoc_',level = 1, ClusterConteinerKey = 'SCT_Clusters')

#############################################################################################################
## SCT ByteStreaErrors
if InDetD3PDSCTFlags.storeBSerror:
    from InDetD3PDMaker import SCTByteStreamErrorFillerTool
    EventInfoD3PDObject.defineBlock (1, 'SCTErrors',SCTByteStreamErrorFillerTool)

##############################################################################################################
## Add blocks to the tree

if InDetD3PDSCTFlags.storeEventInfo:
    alg += EventInfoD3PDObject(10)
    if not InDetFlags.doHeavyIon():
        alg += TRTPhaseD3PDObject(10)
    #use Truth?    
    if (InDetD3PDSCTFlags.includeTruthInfo and globalflags.DataSource == 'geant4'):
        alg += TruthVertexD3PDObject(10)
        alg += PrimaryVertexD3PDObject(10)
    else:
        alg += PrimaryVertexD3PDObject(8)

if InDetD3PDSCTFlags.storeTracks:
    #use Truth?
    if (InDetD3PDSCTFlags.includeTruthInfo and globalflags.DataSource == 'geant4'):
        alg += TruthTrackD3PDObject(10)
        alg += TrackParticleD3PDObject(10)
    else:
        alg += TrackParticleD3PDObject(8)


if InDetD3PDSCTFlags.storeSctRDOs:
    alg += SCTRDOD3PDObject(8)

if InDetD3PDSCTFlags.storeSctClusters:    
    alg += SCTClusterD3PDObject(8)

if InDetD3PDSCTFlags.storeBCMrdo:
    alg += BcmRdoD3PDObject(8)

if InDetD3PDSCTFlags.storeTriggerInfo:
    ## Configure Trigger reading
    DoTrigger = True
    if DoTrigger and False:
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("ReadPool")

    #alg += CtpDecisionD3PDObject(10)
    #if InDetD3PDSCTFlags.doSCTNtupleExtended:
    #    alg += CtpRdoD3PDObject(10)
    alg += TrigDecisionD3PDObject(3)
    addTrigConfMetadata(alg)
    
###################################################################################################################

if globalflags.InputFormat() == 'bytestream':
    from AnalysisTools.AnalysisToolsConf import AANTupleStream
    AANTupleStream('D3PD').ExistDataHeader = False



#theApp.CreateSvc += ["FPEControlSvc"]
