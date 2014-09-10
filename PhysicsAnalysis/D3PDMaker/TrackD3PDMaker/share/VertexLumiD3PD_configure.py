#
## @file TrackD3PDMaker/share/VertexGroupD3PD_configure.py
## @brief PreInlcude file for Reco_trf.py to enable NBC and split vertices algorithms
## @author Simone Pagan Griso
## @date Mar, 2012
##

# Do not use beamspot constraint tools and cuts and
#  enable non beamspot constrined vertex algorithm
# Note: The setting can still be over-written with a preExec
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)
InDetFlags.doVertexFindingForMonitoring.set_Value_and_Lock(True)
