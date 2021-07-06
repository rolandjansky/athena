ftf = findAlgorithm(topSequence, "TrigFastTrackFinder__jet")
ftf.TripletDoPPS = False
 
ftf.TripletDoConfirm = True #activate PPP seeding with seed confirmation
ftf.TripletMaxCurvatureDiff = 0.2 # i.e. a PPP seed is confirmed if there is any other PPS seed with less than 20% difference in signed curvature
ftf.DoubletDR_Max_Confirm = 150.0 # max distance to search for ‘S’ in PPS
 
ftf.UseTrigSeedML = 4
ftf.doSeedRedundancyCheck = True

##ftf.OutputLevel=DEBUG 

