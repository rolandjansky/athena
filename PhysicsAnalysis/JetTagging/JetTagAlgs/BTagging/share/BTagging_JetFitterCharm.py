
from BTagging.JetFitterCharm import get_jet_fitter_vx_finder
JetFitterCharmVxFinder = get_jet_fitter_vx_finder(
    vx_finder_name = "JetFitterCharmVxFinder", 
    b_tag_tool     = myBTagTool, 
    init_helper    = NewJetFitterInitializationHelper, 
    routines       = NewJetFitterRoutines, 
    utils          = NewInDetJetFitterUtils, 
    extrapolator   = JetFitterExtrapolator, 
    track_selector = NewInDetJetFitterTrackSelectorTool, 
    mode3dfinder   = mode3dfinder, 
    seq_vx_fitter  = sequentialVtxFitter, 
    vxp_container  = InDetKeys.PrimaryVertices(),
    cut_factor     = 2.0, 
    )

from BTagging.JetFitterCharm import get_jet_fitter_charm
JetFitterCharm = get_jet_fitter_charm(
    jet_collection_list = BTaggingFlags.Jets, 
    calibration_tool = BTagCalibrationBrokerTool, 
    output_level = BTaggingFlags.OutputLevel, 
    output_threshold = 3, 
    vx_finder_name = 'JetFitterCharmVxFinder' )

