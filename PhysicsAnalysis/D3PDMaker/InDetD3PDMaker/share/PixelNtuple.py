#import MinBiasD3PDMaker
import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TriggerD3PDMaker

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags       import TrackD3PDFlags
from InDetD3PDMaker.InDetD3PDMakerPixelFlags  import InDetD3PDPixelFlags

### added to enure the TrackD3PD generic track variables (14.04.2011)
TrackD3PDFlags.vertexPositionLevelOfDetails=1
TrackD3PDFlags.storeVertexFitQuality = True
TrackD3PDFlags.storeVertexKinematics=True
TrackD3PDFlags.storeVertexPurity=False
TrackD3PDFlags.storeVertexTrackAssociation=False
TrackD3PDFlags.storeVertexTrackIndexAssociation=False

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

from InDetD3PDMaker.PixelClusterD3PDObject      import PixelClusterD3PDObject     #new
from InDetD3PDMaker.PixelRDOD3PDObject          import PixelRDOD3PDObject         #new
from TrackD3PDMaker.TRTPhaseD3PDObject        import TRTPhaseD3PDObject
from InDetD3PDMaker.PixelHitOnTrackAssociation  import PixelHitOnTrackAssociation #new

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

## Athena imports
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
########################################################################################
# Configure Pixel Ntuple tree


InDetD3PDPixelFlags.storeEventInfo = True
InDetD3PDPixelFlags.storeBSerror = True
InDetD3PDPixelFlags.storeTracks = True
InDetD3PDPixelFlags.storePixelClusters = True
InDetD3PDPixelFlags.storePixelRDOs = True
InDetD3PDPixelFlags.storeTriggerInfo = True

# Xiaoxiao:
InDetD3PDPixelFlags.storeTriggerInfo = False
#if globalflags.InputFormat() == 'bytestream' and hasattr(runArgs,"topOptions"): 
    #InDetD3PDPixelFlags.storeTriggerInfo = False
#elif globalflags.InputFormat() == 'pool' and  globalflags.DataSource() == 'geant4' and hasattr(runArgs,"topOptions"):
    #InDetD3PDPixelFlags.storeTriggerInfo = False
    

InDetD3PDPixelFlags.storePhysicsInfo = False
InDetD3PDPixelFlags.storeBCMrdo = False

InDetD3PDPixelFlags.storeHitsOnTracks = False
InDetD3PDPixelFlags.doTrackToClusterAssociation = True  #temporarily false 
InDetD3PDPixelFlags.doClusterToTrackAssociation = True  #temporarily false
InDetD3PDPixelFlags.doRdoToClusterAssociation = True
InDetD3PDPixelFlags.doClusterToRdoAssociation = True


if InDetD3PDPixelFlags.doPixelNtupleLight:
    InDetD3PDPixelFlags.doPixelNtuple = False
    InDetD3PDPixelFlags.doPixelNtupleExtended = False
elif InDetD3PDPixelFlags.doPixelNtupleExtended:
    InDetD3PDPixelFlags.doPixelNtupleLight = False
    InDetD3PDPixelFlags.doPixelNtuple = False



if InDetD3PDPixelFlags.doPixelNtupleLight:
    InDetD3PDPixelFlags.storePixelClusters = False
    InDetD3PDPixelFlags.storePixelRDOs = False
    if InDetD3PDPixelFlags.storeTracks:
        InDetD3PDPixelFlags.storeHitsOnTracks = True
        InDetD3PDPixelFlags.doTrackToClusterAssociation = False
        InDetD3PDPixelFlags.doClusterToTrackAssociation = False
        InDetD3PDPixelFlags.doRdoToClusterAssociation = False
        InDetD3PDPixelFlags.doClusterToRdoAssociation = False

elif InDetD3PDPixelFlags.doPixelNtupleExtended:
    InDetD3PDPixelFlags.storeBCMrdo = True

    
## D3PD Maker alg ########################################################################################
from InDetRecExample.InDetKeys import InDetKeys
name = InDetD3PDPixelFlags.outputFile() #'PixelNtuple_'+InDetKeys.trkD3PDFileName()

print InDetD3PDPixelFlags

print TrackD3PDFlags

#print "1 ", name 
#
#if InDetD3PDPixelFlags.doPixelNtupleLight and True:
#    name = 'Light_'+InDetD3PDPixelFlags.outputFile()
#    print "2 ", name 
#elif InDetD3PDPixelFlags.doPixelNtupleExtended and True:
#    name = 'Extended_'+InDetD3PDPixelFlags.outputFile()
#    print "3 ", name
    
alg = D3PDMakerCoreComps.MakerAlg('InDetTrackTree',
                                  topSequence,
                                  name)

#alg_2 = D3PDMakerCoreComps.MakerAlg('InDetAllTree',
#                                     topSequence,
#                                     name)

#print "4 ", name

##########################################################################################################
## Track to cluster association for full/extended version 
if InDetD3PDPixelFlags.doTrackToClusterAssociation:
    PixelClusterAssoc = IndexMultiAssociation(TrackParticleD3PDObject,
                                            InDetD3PDMaker.TrackParticleToPixelClusterAssociationTool,
                                            'PixelClus','clAssoc_',level = 1)

##########################################################################################################
## Cluster on Tracks for light version 
if InDetD3PDPixelFlags.storeHitsOnTracks:
    LightAssociationTool = InDetD3PDMaker.TrackParticleToPixelClusterAssociationToolLight
    Pixel_clAssocLight     = PixelHitOnTrackAssociation\
                           (TrackParticleD3PDObject,
                            LightAssociationTool,
                            'clAssoc_', 'clAssoc_', 0) 
## Pixel CLusters ##########################################################################################
## Set up associaations for full/extended version 
if InDetD3PDPixelFlags.doClusterToRdoAssociation:
    PixelRDOAssoc = IndexMultiAssociation(PixelClusterD3PDObject,
                                        InDetD3PDMaker.PixelClusterPixelRDOAssociationTool,
                                        'PixelRdo','rdoAssoc_',level = 1)

if InDetD3PDPixelFlags.doClusterToTrackAssociation:
    TrackAssoc = IndexMultiAssociation(PixelClusterD3PDObject,
                                       InDetD3PDMaker.PixelClusterToTrackParticleAssociationTool,
                                       'trk','trkAssoc_',level = 1,
                                       TrackParticleContainerSGKey = D3PDMakerFlags.TrackSGKey())

## Pixel RDOs ##############################################################################################
## Set up associations for full/extended version    
if InDetD3PDPixelFlags.doRdoToClusterAssociation:
    clasterAssoc = IndexAssociation(PixelRDOD3PDObject,InDetD3PDMaker.PixelRDOClusterAssociationTool,
                                    'PixelClus','clAssoc_',level = 1, ClusterConteinerKey = 'PixelClusters')

#############################################################################################################
## Pixel ByteStreamErrors
#if InDetD3PDPixelFlags.storeBSerror:
#    from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__PixelByteStreamErrorFillerTool as PixelByteStreamFiller
#    EventInfoD3PDObject.defineBlock (1, 'PixelErrors',PixelByteStreamFiller)

##############################################################################################################
## Add blocks to the tree

if InDetD3PDPixelFlags.storeEventInfo:
    alg += EventInfoD3PDObject(10)
    if not InDetFlags.doHeavyIon():
        alg += TRTPhaseD3PDObject(10)
    
    alg += PrimaryVertexD3PDObject(8)
    
if InDetD3PDPixelFlags.storeTracks:
    alg += TrackParticleD3PDObject(8)

if InDetD3PDPixelFlags.storePixelRDOs:
    alg += PixelRDOD3PDObject(8)
    #alg_2 += PixelRDOD3PDObject(8)

if InDetD3PDPixelFlags.storePixelClusters:    
    alg += PixelClusterD3PDObject(8)
    #alg_2 += PixelClusterD3PDObject(8)

if InDetD3PDPixelFlags.storeBCMrdo:
    alg += BcmRdoD3PDObject(8)

if InDetD3PDPixelFlags.storeTriggerInfo:
    ## Configure Trigger reading
    DoTrigger = True
    if DoTrigger and False:
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("ReadPool")

    #alg += CtpDecisionD3PDObject(10)
    #if InDetD3PDPixelFlags.doPixelNtupleExtended:
    #    alg += CtpRdoD3PDObject(10)
    alg += TrigDecisionD3PDObject(3)
    addTrigConfMetadata(alg)
    
###################################################################################################################

if globalflags.InputFormat() == 'bytestream':
    from AnalysisTools.AnalysisToolsConf import AANTupleStream
    AANTupleStream('D3PD').ExistDataHeader = False



#theApp.CreateSvc += ["FPEControlSvc"]
