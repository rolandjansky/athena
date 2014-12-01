#import MinBiasD3PDMaker
import TrackD3PDMaker
import D3PDMakerCoreComps
import EventCommonD3PDMaker
import TriggerD3PDMaker

from D3PDMakerConfig.D3PDMakerFlags           import D3PDMakerFlags
from TrackD3PDMaker.TrackD3PDMakerFlags       import TrackD3PDFlags
from TrackD3PDMaker.TrackD3PDMakerSCTFlags    import TrackD3PDSCTFlags

from TrackD3PDMaker.TruthTrackD3PDObject import TruthTrackD3PDObject
from TrackD3PDMaker.TruthVertexD3PDObject import TruthVertexD3PDObject

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

from TrackD3PDMaker.SCTClusterD3PDObject      import SCTClusterD3PDObject     #new
from TrackD3PDMaker.SCTRDOD3PDObject          import SCTRDOD3PDObject         #new
from TrackD3PDMaker.TRTPhaseD3PDObject        import TRTPhaseD3PDObject
from TrackD3PDMaker.SctHitOnTrackAssociation  import SctHitOnTrackAssociation #new

#primary vertex
from TrackD3PDMaker.VertexD3PDObject          import PrimaryVertexD3PDObject 


from TrackD3PDMaker.TrackD3PDObject           import TrackParticleD3PDObject
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

TrackD3PDSCTFlags.storeEventInfo = True
TrackD3PDSCTFlags.storeBSerror = True
TrackD3PDSCTFlags.storeTracks = False
TrackD3PDSCTFlags.storeSctClusters = True
TrackD3PDSCTFlags.storeSctRDOs = True
TrackD3PDSCTFlags.storeTriggerInfo = False
if globalflags.InputFormat() == 'bytestream' and hasattr(runArgs,"topOptions"): 
    TrackD3PDSCTFlags.storeTriggerInfo = False
elif globalflags.InputFormat() == 'pool' and  globalflags.DataSource() == 'geant4' and hasattr(runArgs,"topOptions"):
    TrackD3PDSCTFlags.storeTriggerInfo = False
    
TrackD3PDSCTFlags.doTrackToMuonAssociation = False
TrackD3PDSCTFlags.storePhysicsInfo = False
TrackD3PDSCTFlags.storeBCMrdo = False
TrackD3PDSCTFlags.includeTruthInfo = False
TrackD3PDSCTFlags.storeHitsOnTracks = False
TrackD3PDSCTFlags.doTrackToClusterAssociation = False
TrackD3PDSCTFlags.doClusterToTrackAssociation = False
TrackD3PDSCTFlags.doRdoToClusterAssociation = True
TrackD3PDSCTFlags.doClusterToRdoAssociation = True


if TrackD3PDSCTFlags.doSCTNtupleLight:
    TrackD3PDSCTFlags.doSCTNtuple = False
    TrackD3PDSCTFlags.doSCTNtupleExtended = False
elif TrackD3PDSCTFlags.doSCTNtupleExtended:
    TrackD3PDSCTFlags.doSCTNtupleLight = False
    TrackD3PDSCTFlags.doSCTNtuple = False



if TrackD3PDSCTFlags.doSCTNtupleLight:
    TrackD3PDSCTFlags.storeSctClusters = False
    TrackD3PDSCTFlags.storeSctRDOs = False
    if TrackD3PDSCTFlags.storeTracks:
        TrackD3PDSCTFlags.storeHitsOnTracks = True
        TrackD3PDSCTFlags.doTrackToMuonAssociation = False
        TrackD3PDSCTFlags.doTrackToClusterAssociation = False
        TrackD3PDSCTFlags.doClusterToTrackAssociation = False
        TrackD3PDSCTFlags.doRdoToClusterAssociation = False
        TrackD3PDSCTFlags.doClusterToRdoAssociation = False

elif TrackD3PDSCTFlags.doSCTNtupleExtended:
    TrackD3PDSCTFlags.storeBCMrdo = True

    
## D3PD Maker alg ########################################################################################
from InDetRecExample.InDetKeys import InDetKeys
name = TrackD3PDSCTFlags.outputFile() #'SctNtuple_'+InDetKeys.trkD3PDFileName()

print TrackD3PDSCTFlags

print TrackD3PDFlags

#print "1 ", name 
#
#if TrackD3PDSCTFlags.doSCTNtupleLight and True:
#    name = 'Light_'+TrackD3PDSCTFlags.outputFile()
#    print "2 ", name 
#elif TrackD3PDSCTFlags.doSCTNtupleExtended and True:
#    name = 'Extended_'+TrackD3PDSCTFlags.outputFile()
#    print "3 ", name
    
alg = D3PDMakerCoreComps.MakerAlg('InDetTrackTree',
                                  topSequence,
                                  name)

#print "4 ", name

##########################################################################################################
## Track to cluster association for full/extended version 
if TrackD3PDSCTFlags.doTrackToClusterAssociation:
    SCTClusterAssoc = IndexMultiAssociation(TrackParticleD3PDObject,
                                            TrackD3PDMaker.TrackParticleToSCTClusterAssociationTool,
                                            'SCTClus','clAssoc_',level = 1)

##########################################################################################################
## Cluster on Tracks for light version 
if TrackD3PDSCTFlags.storeHitsOnTracks:
    LightAssociationTool = TrackD3PDMaker.TrackParticleToSCTClusterAssociationToolLight
    SCT_clAssocLight     = SctHitOnTrackAssociation\
                           (TrackParticleD3PDObject,
                            LightAssociationTool,
                            'clAssoc_', 'clAssoc_', 0) 
## SCT CLusters ##########################################################################################
## Set up associaations for full/extended version 
if TrackD3PDSCTFlags.doClusterToRdoAssociation:
    SCTRDOAssoc = IndexMultiAssociation(SCTClusterD3PDObject,
                                        TrackD3PDMaker.SCTClusterSCTRDOAssociationTool,
                                        'SCTRdo','rdoAssoc_',level = 1)

if TrackD3PDSCTFlags.doClusterToTrackAssociation:
    TrackAssoc = IndexMultiAssociation(SCTClusterD3PDObject,
                                       TrackD3PDMaker.SCTClusterToTrackParticleAssociationTool,
                                       'trk','trkAssoc_',level = 1,
                                       TrackParticleContainerSGKey = D3PDMakerFlags.TrackSGKey())

## Muons ##########################################################################################
## TrackParticle to muon association
if TrackD3PDSCTFlags.doTrackToMuonAssociation:
    if TrackD3PDSCTFlags.useStacoMuonCollectionForAssociation==True:
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
if TrackD3PDSCTFlags.doRdoToClusterAssociation:
    clasterAssoc = IndexAssociation(SCTRDOD3PDObject,TrackD3PDMaker.SCTRDOClusterAssociationTool,
                                    'SCTClus','clAssoc_',level = 1, ClusterConteinerKey = 'SCT_Clusters')

#############################################################################################################
## SCT ByteStreaErrors
if TrackD3PDSCTFlags.storeBSerror:
    from TrackD3PDMaker.TrackD3PDMakerConf import D3PD__SCTByteStreamErrorFillerTool as SCTByteStreamFiller
    EventInfoD3PDObject.defineBlock (1, 'SCTErrors',SCTByteStreamFiller)

##############################################################################################################
## Add blocks to the tree

if TrackD3PDSCTFlags.storeEventInfo:
    alg += EventInfoD3PDObject(10)
    if not InDetFlags.doHeavyIon():
        alg += TRTPhaseD3PDObject(10)
    #use Truth?    
    if (TrackD3PDSCTFlags.includeTruthInfo and globalflags.DataSource == 'geant4'):
        alg += TruthVertexD3PDObject(10)
        alg += PrimaryVertexD3PDObject(10)
    else:
        alg += PrimaryVertexD3PDObject(8)

if TrackD3PDSCTFlags.storeTracks:
    #use Truth?
    if (TrackD3PDSCTFlags.includeTruthInfo and globalflags.DataSource == 'geant4'):
        alg += TruthTrackD3PDObject(10)
        alg += TrackParticleD3PDObject(10)
    else:
        alg += TrackParticleD3PDObject(8)


if TrackD3PDSCTFlags.storeSctRDOs:
    alg += SCTRDOD3PDObject(8)

if TrackD3PDSCTFlags.storeSctClusters:    
    alg += SCTClusterD3PDObject(8)

if TrackD3PDSCTFlags.storeBCMrdo:
    alg += BcmRdoD3PDObject(8)

if TrackD3PDSCTFlags.storeTriggerInfo:
    ## Configure Trigger reading
    DoTrigger = True
    if DoTrigger and False:
        from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
        cfg = TriggerConfigGetter("ReadPool")

    #alg += CtpDecisionD3PDObject(10)
    #if TrackD3PDSCTFlags.doSCTNtupleExtended:
    #    alg += CtpRdoD3PDObject(10)
    alg += TrigDecisionD3PDObject(3)
    addTrigConfMetadata(alg)
    
###################################################################################################################

if globalflags.InputFormat() == 'bytestream':
    from AnalysisTools.AnalysisToolsConf import AANTupleStream
    AANTupleStream('D3PD').ExistDataHeader = False



#theApp.CreateSvc += ["FPEControlSvc"]
