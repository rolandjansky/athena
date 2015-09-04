
from BTagging.JetFitterCharm import get_jet_fitter_charm
JetFitterCharmNeg = get_jet_fitter_charm(
    jet_collection_list = BTaggingFlags.Jets, 
    calibration_tool = BTagCalibrationBrokerTool, 
    output_level = BTaggingFlags.OutputLevel, 
    output_threshold = 3, 
    name_extension='CharmNeg', 
    do_neg=True, 
    vx_finder_name = 'JetFitterCharmVxFinder' )

