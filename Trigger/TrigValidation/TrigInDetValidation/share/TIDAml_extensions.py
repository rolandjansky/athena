ftf = findAlgorithm(topSequence, "TrigFastTrackFinder_JetFS")
ftf.doSeedRedundancyCheck = True
ftf.UseTrigSeedML  = 1 #can be 0, 1, 2, or 3, 0 means the
# ML-based seed filtering is off
ftf.TrigSeedML_LUT = 'trigseed_ML_medium.lut' 
ftf.OutputLevel=DEBUG 

