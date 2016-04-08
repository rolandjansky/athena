###############################################################
#
# jobOptions fragment to make ESD from iPatTracks 
#
#==============================================================

#--------------------------------------------------------------                        
# Trk::Tracks available as "Tracks" directly from iPatRec
#--------------------------------------------------------------
InputTrackCollection       = ""
InputTrackCollectionTruth  = ""
if not 'TrackCollectionKeys' in dir():
   TrackCollectionKeys        = []
if not 'TrackCollectionTruthKeys' in dir():
   TrackCollectionTruthKeys   = []

# this is the iPatRec track collection name, put it on imput for the next algorithm
InputTrackCollection      = "Tracks"
       
if doTruth:
       InputTrackCollectionTruth = "TrackTruthCollection"
       InputDetailedTrackTruth   = "ConvertedIPatTracksDetailedTruth"
       #
       # set up the truth info for this container
       #
       include ("InDetRecExample/ConfiguredInDetTrackTruth.py")
       InDetIPatRecTruth = ConfiguredInDetTrackTruth(InputTrackCollection,
                                                     InputDetailedTrackTruth,
                                                     InputTrackCollectionTruth)
       #
       # add to keys lists for statistics
       #
       TrackCollectionKeys      += [ InputTrackCollection ]
       TrackCollectionTruthKeys += [ InputTrackCollectionTruth ]

#--------------------------------------------------------------
# TrackParticles, vertexing etc
#--------------------------------------------------------------
from AthenaCommon.BeamFlags import jobproperties
include ("InDetRecExample/InDetRecPostProcessing.py")

#==============================================================
# End of iPatRec tracks to ESD jobOptions fragment
#
###############################################################
