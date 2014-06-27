# MuonClusterOnTrackCreator.py
# David Adams (for Woo Chun Park)
# July 2007
#
# Define the default IMuonClusterOnTrackCreator
#
# Note that we use CscClusterOnTrackCreator which gives the possibility to refit CSC clusters.

print "MuonClusterOnTrackCreator.py: Defining theMuonClusterOnTrackCreator"

from MuonClusterOnTrackCreator.MuonClusterOnTrackCreatorConf import Muon__CscClusterOnTrackCreator 
theMuonClusterOnTrackCreator = Muon__CscClusterOnTrackCreator( name="CscClusterOnTrackCreator" )

from CscClusterization.CalibCscStripFitter import theCalibCscStripFitter
from CscClusterization.QratCscClusterFitter import theQratCscClusterFitter

theMuonClusterOnTrackCreator.CscStripFitter = theCalibCscStripFitter
theMuonClusterOnTrackCreator.CscClusterFitter = theQratCscClusterFitter
