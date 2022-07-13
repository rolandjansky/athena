# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
@file HTTAlgorithmTags.py
@author Riley Xu - riley.xu@cern.ch
@date Mar 16th 2020

This file declares tags for configuration parameters of algorithm jobs in this package.
All options can be further overridden from the command line or job options.

In general, tags should not be edited after creation for consistent results.

See TrigHTT README for more info on tag heirarchy and usage.
'''



empty_tag = { # template to add a new tag, and some description of the fields
    # Common
    'name':     '',     # Name of this tag
    'package':  'algo', # Package identifier

    # Input
    'FirstInputToolN': 1,
    'realHitsOverlay': None,
    'HitFiltering': False,
    'SpacePoints': False,
    'SpacePointFiltering': False, 
    'writeOutputData': True,

    # Road Finding Selection
    'hough': None,
    'hough_d0phi0': None,
    'hough_1d': None,
    'hough_rootoutput': None,

    # NN Track Filtering
    'TrackNNAnalysis': None,

    # Overlap removal - default or fast
    'doFastOR' : False,    

    # Hough
    'xVar': '', # Variable to use on x axis
    'yVar': '', # Variable to use on y axis
    'phi_min': 0,
    'phi_max': 0,
    'qpt_min': 0,
    'qpt_max': 0,
    'd0_slices': [],
    'xBins': 0,
    'yBins': 0,
    'xBufferBins': 0, # Number of extra buffer bins on each side
    'yBufferBins': 0,
    'slicing': True, # Number of slices will be pulled from map tag
    'threshold': [],
    'convolution': [],
    'convSize_x': 0,
    'convSize_y': 0,
    'hitExtend_x': 0, # Hit lines will fill extra bins in x by this amount on each side
    'roadSliceOR': False,
    'fieldCorrection': True,

    # Performance monitoring
    'barcodeFracMatch' : 0.5,    

    # Track Fitter
    'chi2DofRecoveryMin' : None,
    'chi2DofRecoveryMax' : None,

    'doMajority': None,

    'nHits_noRecovery': None,

    'GuessHits': None,
    'DoMissingHitsChecks': None,
    'IdealCoordFitType': None,
    'EtaPatternRoadFilter': None,
    'PhiRoadFilter' : None,
    'PhiRoad_ptscaling' :0.0,
    'DoDeltaGPhis' : False, # True if doing the fit with delta global phis method

    # Second stage fitting
    'secondStage' : False,

    # Fast Monitoring
    'fastMon': False,
    'canExtendHistRanges': False,

    'DataClass': None,

    'doTracking' : None,
    'outputHitTxt': None,

    # lrt settings
    'lrt': False,
    'lrt_use_basicHitFilter': False,
    'lrt_use_mlHitFilter': False,
    'lrt_use_straightTrackHT': False,
    'lrt_use_doubletHT': False,
    'lrt_doublet_d0_range': 120.0,
    'lrt_doublet_d0_bins': 216,
    'lrt_doublet_qpt_range': 0.02,
    'lrt_doublet_qpt_bins': 216,
    'lrt_mon_phiRange': (0.2, 0.5),
    'lrt_mon_d0Range': (-100,100),
    'lrt_mon_z0Range': (-300,300),

}

def update( otherdict ):
    empty_tag_clone = empty_tag.copy()
    empty_tag_clone.update(otherdict)
    return empty_tag_clone

defaultTag = 'dev_21-02-15'


HTTAlgorithmTags = {
        'hough_1d': { # Runs the Hough 1D transform
            # Common
            'name':    'hough_1d',
            'package': 'algo',

            # Input
            'FirstInputToolN': 1,
            'realHitsOverlay': False,
            'HitFiltering': False,
            'SpacePoints': False,
            'SpacePointFiltering': False, 
            'writeOutputData': True,

            # Road Finding Selection
            'hough': False,
            'hough_1d': True,
            'hough_rootoutput': False,

            # NN Track Filtering
            'TrackNNAnalysis': False,

            # Overlap removal - default or fast
            'doFastOR' : False,            

            # Hough
            'xVar': 'phi',
            'yVar': 'q/pt',
            'phi_min': 0.0,
            'phi_max': 0.8,
            'qpt_min': -1.0,
            'qpt_max': 1.0,
            'd0_slices': [],
            'xBins': 200,
            'yBins': 0,
            'xBufferBins': 0,
            'yBufferBins': 0,
            'slicing': True,
            'threshold': [7],
            'convolution': [],
            'convSize_x': 0,
            'convSize_y': 0,
            'hitExtend_x': [1] * 8,
            'localMaxWindowSize': 0,
            'roadSliceOR': False,
            'fieldCorrection': True,

            # Performance monitoring
            'barcodeFracMatch' : 0.5,            

            # Hough 1D
            'phiRangeCut' : True,
            'splitpt' : 1,
            'radiiFile' : None,

            # Track Fitter
            'chi2DofRecoveryMin': 40,
            'chi2DofRecoveryMax': 100000000000,
            'doMajority': 1,
            'nHits_noRecovery': -1,
            'GuessHits': True,
            'DoMissingHitsChecks': False,
            'IdealCoordFitType': 0,
            'EtaPatternRoadFilter': None,
            'PhiRoadFilter' : None,
            'DoDeltaGPhis' : False,

            # Second stage fitting
            'secondStage' : False,

            # Fast Monitoring
            'fastMon': False,
            'canExtendHistRanges': False,

            'DataClass': 2,

            'doTracking': False,
            'outputHitTxt': None,

            'lrt': False,
            'lrt_use_basicHitFilter': False,
            'lrt_use_mlHitFilter': False,
            'lrt_use_straightTrackHT': False,
            'lrt_use_doubletHT': False,
            'lrt_doublet_d0_range': 120.0,
            'lrt_doublet_d0_bins': 216,
            'lrt_doublet_qpt_range': 0.02,
            'lrt_doublet_qpt_bins': 216,
            'lrt_mon_phiRange': (0.2, 0.5),
            'lrt_mon_d0Range': (-100,100),
            'lrt_mon_z0Range': (-300,300),
        },
        'hough': { # Runs the 2d hough transform
            # Common
            'name':    'hough',
            'package': 'algo',

            # Input
            'FirstInputToolN': 1,
            'realHitsOverlay': False,
            'HitFiltering': False,
            'SpacePoints': False,
            'SpacePointFiltering': False, 
            'writeOutputData': True,

            # Road Finding Selection
            'hough': True,
            'hough_1d': False,
            'hough_rootoutput': False,

            # NN Filtering - toggle this one
            'TrackNNAnalysis': False,

            # Overlap removal - default or fast
            'doFastOR' : False,

            # Hough
            'xVar': 'phi',
            'yVar': 'q/pt',
            'phi_min': 0.3,
            'phi_max': 0.5,
            'qpt_min': -1.0,
            'qpt_max': 1.0,
            'd0_slices': [],
            'xBins': 216,
            'yBins': 216,
            'xBufferBins': 6,
            'yBufferBins': 2,
            'slicing': True,
            'threshold': [7],
            'convolution': [],
            'combine_layers': [],
            'scale': [],
            'convSize_x': 0,
            'convSize_y': 0,
            'hitExtend_x': [2,1,0,0,0,0,0,0],
            'localMaxWindowSize': 0,
            'roadSliceOR': False,
            'fieldCorrection': True,

            # Track Fitter
            'chi2DofRecoveryMin': 40,
            'chi2DofRecoveryMax': 20000,
            'doMajority': 1,
            'nHits_noRecovery': -1,
            'GuessHits': True,
            'DoMissingHitsChecks': False,
            'IdealCoordFitType': 0,
            'EtaPatternRoadFilter': None,
            'PhiRoadFilter' : None,
            'DoDeltaGPhis' : False,

            # Second stage fitting
            'secondStage' : False,

            # Fast Monitoring
            'fastMon': False,
            'canExtendHistRanges': False,

            'DataClass': 2,

            'doTracking': False,
            'outputHitTxt': False,

            # Performance monitoring
            'barcodeFracMatch' : 0.5,

            # Everything for large radius tracking
            'lrt': False,
            'lrt_use_basicHitFilter': False,
            'lrt_use_mlHitFilter': False,
            'lrt_use_straightTrackHT': False,
            'lrt_use_doubletHT': False,
            # Overrides to turn off any hit filtering
            'lrt_skip_hit_filtering' : False,
            # Minimum pT cut to include in analysis, in GeV
            'lrt_ptmin' : 5,
            # Truth particles to allow
            # By default any PDGID and low barcode
            'allowHighTruthBarcode' : False,
            'm_LRT_pdgID' : 0,
            # Doublet HT flags
            'lrt_doublet_d0_range': 120.0,
            'lrt_doublet_d0_bins': 216,
            'lrt_doublet_qpt_range': 0.02,
            'lrt_doublet_qpt_bins': 216,
            # Straight track HT flags
            'lrt_straighttrack_xVar': 'phi',
            'lrt_straighttrack_yVar': 'd0',
            'lrt_straighttrack_phi_min': 0.3,
            'lrt_straighttrack_phi_max': 0.5,
            'lrt_straighttrack_d0_min': -300.0,
            'lrt_straighttrack_d0_max': 300.0,
            'lrt_straighttrack_xBins': 216,
            'lrt_straighttrack_yBins': 216,
            'lrt_straighttrack_xBufferBins': 6,
            'lrt_straighttrack_yBufferBins': 2,
            'lrt_straighttrack_slicing': True,
            'lrt_straighttrack_threshold': [7],
            'lrt_straighttrack_convolution': [],
            'lrt_straighttrack_combine_layers': [0,1,2,3,4,5,6,7],
            'lrt_straighttrack_scale': [1,1,1,1,1,1,1,1],
            'lrt_straighttrack_convSize_x': 0,
            'lrt_straighttrack_convSize_y': 0,
            'lrt_straighttrack_hitExtend_x': [2,1,0,0,0,0,0,0],
            'lrt_straighttrack_stereo': False,
            'lrt_straighttrack_localMaxWindowSize': 0,
            # For monitoring (not totally implemented)
            'lrt_mon_phiRange': (0.2, 0.5),
            'lrt_mon_d0Range': (-100,100),
            'lrt_mon_z0Range': (-300,300),            
        },
        'dev_21-02-15': { # For development use      
            'name':    'dev_21-02-15',
            'package': 'algo',

            # Input
            'FirstInputToolN': 1,
            'realHitsOverlay': False,
            'HitFiltering': False,
            'SpacePoints': False,
            'SpacePointFiltering': False, 
            'writeOutputData': True,

            # Road Finding Selection
            'hough': False,
            'hough_1d': False,
            'hough_rootoutput': False,

            # NN Filtering
            'TrackNNAnalysis' : False,

            # Overlap removal - default or fast
            'doFastOR' : False,            

            # Hough
            'xVar': 'phi',
            'yVar': 'q/pt',
            'phi_min': 0.3,
            'phi_max': 0.5,
            'qpt_min': -1.0,
            'qpt_max': 1.0,
            'd0_slices': [],
            'xBins': 64,
            'yBins': 216,
            'xBufferBins': 2,
            'yBufferBins': 2,
            'slicing': True,
            'threshold': [70],
            'convolution': [1, 10, 1],
            'combine_layers': [],#[1,2,3,4,5,6,7,8],  # i.e. [1,2,3,1,2,3,1,2] will combine (1st, 4th, 7th) (2nd, 5th, 8th) (3rd, 6th) layers
            'scale': [], # i.e.  [1,1,2,2,3,3,4,4] will scale the image size by 1/2 for (2nd,3rd) layers, by 1/3 for (5th, 6th) layers, by 1/4 for (7th, 8th) layers, must be compatible with combine layers
            'convSize_x': 3,
            'convSize_y': 1,
            'hitExtend_x': [],
            'localMaxWindowSize': 0,
            'roadSliceOR': False,
            'fieldCorrection': True,

            # Performance monitoring
            'barcodeFracMatch' : 0.5,

            # Track Fitter
            'chi2DofRecoveryMin': 40,
            'chi2DofRecoveryMax': 100000000000,
            'doMajority': 0,
            'nHits_noRecovery': -1,
            'GuessHits': True,
            'DoMissingHitsChecks': False,
            'IdealCoordFitType': 0,
            'DoDeltaGPhis' : False,

            'EtaPatternRoadFilter': None,
            'PhiRoadFilter' : None,

            # Second stage fitting
            'secondStage' : False,

            # Fast Monitoring
            'fastMon': False,
            'canExtendHistRanges': False,

            'DataClass': 2,

            'doTracking': True,
            'outputHitTxt': False,
            'lrt': False,
            'lrt_use_basicHitFilter': False,
            'lrt_use_mlHitFilter': False,
            'lrt_use_straightTrackHT': False,
            'lrt_use_doubletHT': False,
            'lrt_doublet_d0_range': 120.0,
            'lrt_doublet_d0_bins': 216,
            'lrt_doublet_qpt_range': 0.02,
            'lrt_doublet_qpt_bins': 216,
            'lrt_mon_phiRange': (0.2, 0.5),
            'lrt_mon_d0Range': (-100,100),
            'lrt_mon_z0Range': (-300,300),            
        },
}
