###############################################################
#
# iPatShortTracks job options file
#
#==============================================================
#top algorithm to be run and component library to be loaded
theApp.TopAlg += [ "iPatShortTracks" ]
include( "iPatRecAlgs/iPatRecAlgs_jobOptions.py" )

#--------------------------------------------------------------
# Algorithm Private Options: iPatShortTracks
#--------------------------------------------------------------
iPatShortTracks = Algorithm( "iPatShortTracks" )
iPatShortTracks.ShortTracksName	= "iPatShortTracks"
iPatShortTracks.TracksName	= "iPatTracks"
iPatShortTracks.minPt		= 3.0*GeV
iPatShortTracks.numberLayers	= 5
#==============================================================
#
# End of iPatShortTracks_jobOptions file
#
###############################################################
