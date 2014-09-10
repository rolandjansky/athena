# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# 
## @file TrackD3PDMaker/python/TrackD3PDMakerFlags.py
## @purpose Python module to hold common flags to configure TrackD3PDMaker JobOptions
##



##-----------------------------------------------------------------------------
## Import
from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

# General flags

class doTruth(JobProperty):
    """ Turn on filling of truth branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeDiagonalCovarianceAsErrors(JobProperty):
    """ store diagonal covariance matrix elements as errors ( err[i] = sqrt(cov[i][i]) )
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

# TrackD3PDObject flags

class storeHitTruthMatching(JobProperty):
    """ Turn on filling of hit truth matching branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeDetailedTruth(JobProperty):
    """ Turn on filling of detailed truth branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class trackParametersAtGlobalPerigeeLevelOfDetails(JobProperty):
    """ Set level of details for track parameter at global perigee branches
        0: Don't store
        1: Store parameters only
        2: Store diagonal elements of the covariance matrix
        3: Store off diagonal elements of the covariance matrix
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[0,1,2,3]
    StoredValue=0

class trackParametersAtPrimaryVertexLevelOfDetails(JobProperty):
    """ Set level of details for track parameter at primary vertex branches
        0: Don't store
        1: Store parameters only
        2: Store diagonal elements of the covariance matrix
        3: Store off diagonal elements of the covariance matrix
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[0,1,2,3]
    StoredValue=2

class trackParametersAtBeamSpotLevelOfDetails(JobProperty):
    """ Set level of details for track parameter at beam spot branches
        0: Don't store
        1: Store parameters only
        2: Store diagonal elements of the covariance matrix
        3: Store off diagonal elements of the covariance matrix
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[0,1,2,3]
    StoredValue=3
    
class trackParametersAtBeamLineLevelOfDetails(JobProperty):
    """ Set level of details for track parameter at beam spot branches
        0: Don't store
        1: Store parameters only
        2: Store diagonal elements of the covariance matrix
        3: Store off diagonal elements of the covariance matrix
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[0,1,2,3]
    StoredValue=3

class storeTrackParametersAtCalo(JobProperty):
    """ Turn on filling of track parameters at Calo
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTrackParametersAtCalo2ndLayer(JobProperty):
    """ Turn on filling of track parameters at Calo 2ndLayer
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTrackUnbiasedIPAtPV(JobProperty):
    """ Turn on filling of track unbiased impact parameters at primary vertex branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTrackMomentum(JobProperty):
    """ Turn on filling of track momentum branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTrackInfo(JobProperty):
    """ Turn on filling of track info (fitter and pattern reco info) branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTrackFitQuality(JobProperty):
    """ Turn on filling of track fit quality (chi2 and ndof) branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTrackSummary(JobProperty):
    """ Turn on filling of track summary branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True
    FullInfo = False ## this flag stores the full TS info, overwriting the other flags
    IDHits = True
    IDHoles = True
    IDSharedHits = True
    IDOutliers = False
    PixelInfoPlus = False
    SCTInfoPlus = False
    TRTInfoPlus = False
    InfoPlus = False
    MuonHits = True
    MuonHoles = False
    ExpectBLayer = True
    HitSum = True
    HoleSum = True
    HitPattern = True
    SiHits = False
    TRTRatio = True
    PixeldEdx = False
    ElectronPID = False
    
class storePullsAndResiduals(JobProperty):
    """ Turn on filling of pulls and residuals of hits and outliers on track branches
    """ 
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeBLayerHitsOnTrack(JobProperty):
    """ Turn on filling of Pixel hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storePixelHitsOnTrack(JobProperty):
    """ Turn on filling of Pixel hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeSCTHitsOnTrack(JobProperty):
    """ Turn on filling of SCT hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTRTHitsOnTrack(JobProperty):
    """ Turn on filling of TRT hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeMDTHitsOnTrack(JobProperty):
    """ Turn on filling of MDT hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storeCSCHitsOnTrack(JobProperty):
    """ Turn on filling of CSC hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storeRPCHitsOnTrack(JobProperty):
    """ Turn on filling of TRT hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTGCHitsOnTrack(JobProperty):
    """ Turn on filling of TGC hits on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeBLayerOutliersOnTrack(JobProperty):
    """ Turn on filling of Pixel outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storePixelOutliersOnTrack(JobProperty):
    """ Turn on filling of Pixel outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeSCTOutliersOnTrack(JobProperty):
    """ Turn on filling of SCT outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeTRTOutliersOnTrack(JobProperty):
    """ Turn on filling of TRT outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeMDTOutliersOnTrack(JobProperty):
    """ Turn on filling of MDT outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeCSCOutliersOnTrack(JobProperty):
    """ Turn on filling of CSC outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeRPCOutliersOnTrack(JobProperty):
    """ Turn on filling of RPC outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
            
class storeTGCOutliersOnTrack(JobProperty):
    """ Turn on filling of TGC outliers on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeBLayerHolesOnTrack(JobProperty):
    """ Turn on filling of Pixel holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storePixelHolesOnTrack(JobProperty):
    """ Turn on filling of Pixel holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storeSCTHolesOnTrack(JobProperty):
    """ Turn on filling of SCT holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storeTRTHolesOnTrack(JobProperty):
    """ Turn on filling of TRT holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeMDTHolesOnTrack(JobProperty):
    """ Turn on filling of MDT holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeCSCHolesOnTrack(JobProperty):
    """ Turn on filling of CSC holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
        
class storeRPCHolesOnTrack(JobProperty):
    """ Turn on filling of RPC holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
            
class storeTGCHolesOnTrack(JobProperty):
    """ Turn on filling of TGC holes on track branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeVertexAssociation(JobProperty):
    """ Turn on filling of track to vertex association
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False
    
class storeTrackPredictionAtBLayer(JobProperty):
    """ Turn on filling of track prediction at the B-Layer branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeTruthInfo(JobProperty):
    """ Turn on filling truth perigee and classification information.
    """
    statusOn = True
    allowedTyps = ['bool']
    StoredValue = False

# VertexD3PDObject flags

class vertexPositionLevelOfDetails(JobProperty):
    """ Set level of details for vertex vertex position branches
        0: Don't store
        1: Store position only
        2: Store diagonal elements of the covariance matrix
        3: Store off diagonal elements of the covariance matrix
    """
    statusOn=True
    allowedTypes=['int']
    allowedValues=[0,1,2,3]
    StoredValue=3

class storeVertexType(JobProperty):
    """ Turn on filling of vertex type as defined here
    https://svnweb.cern.ch/trac/atlasoff/browser/Tracking/TrkEvent/TrkEventPrimitives/trunk/TrkEventPrimitives/VertexType.h
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeVertexFitQuality(JobProperty):
    """ Turn on filling of vertex fit quality (chi2 and ndof) branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeVertexKinematics(JobProperty):
    """ Turn on filling of vertex kinematics (sumPT, ...) branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True

class storeVertexPurity(JobProperty):
    """ Turn on filling of vertex purity (truth matching) branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeVertexTrackAssociation(JobProperty):
    """ Turn on filling of vertex track association branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=False

class storeVertexTrackIndexAssociation(JobProperty):
    """ Turn on filling of vertex track index association branches
    """
    statusOn=True
    allowedTypes=['bool']
    StoredValue=True


# V0D3PDObject flags


##-----------------------------------------------------------------------------
## Definition of the TrackD3PDMaker flag container

class TrackD3PDMakerFlags(JobPropertyContainer):
    """ The Track D3PD flag container
    """

        
# add the reconstruction flags container to the top container
jobproperties.add_Container(TrackD3PDMakerFlags)

jobproperties.TrackD3PDMakerFlags.add_JobProperty(doTruth)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeDiagonalCovarianceAsErrors)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeHitTruthMatching)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeDetailedTruth)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storePullsAndResiduals)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeBLayerHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storePixelHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeSCTHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTRTHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeMDTHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeCSCHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeRPCHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTGCHitsOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeBLayerOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storePixelOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeSCTOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTRTOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeMDTOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeCSCOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeRPCOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTGCOutliersOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeBLayerHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storePixelHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeSCTHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTRTHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeMDTHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeCSCHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeRPCHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTGCHolesOnTrack)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexAssociation)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackPredictionAtBLayer)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTruthInfo)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackInfo)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackFitQuality)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackSummary)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(trackParametersAtGlobalPerigeeLevelOfDetails)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(trackParametersAtPrimaryVertexLevelOfDetails)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(trackParametersAtBeamSpotLevelOfDetails)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(trackParametersAtBeamLineLevelOfDetails)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackParametersAtCalo)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackParametersAtCalo2ndLayer)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackUnbiasedIPAtPV)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeTrackMomentum)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(vertexPositionLevelOfDetails)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexType)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexFitQuality)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexKinematics)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexPurity)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexTrackAssociation)
jobproperties.TrackD3PDMakerFlags.add_JobProperty(storeVertexTrackIndexAssociation)

TrackD3PDFlags=jobproperties.TrackD3PDMakerFlags
