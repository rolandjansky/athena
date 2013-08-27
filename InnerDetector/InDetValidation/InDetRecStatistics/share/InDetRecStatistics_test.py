#
# Job options file. Runs InDetRecExample + InDetRecStatistics
# with ntuple on, as a quick demo.
#
# Note that InDetRecExample needs to be set up with
# InDetRecExample/share/POOL_Setup.sh first. See
# InDetRecExample/doc/mainpage.h for more info.
#
#==============================================================


#--------------------------------------------------------------
# Run InDetRecExample, which in turn runs InDetRecStatistics
#--------------------------------------------------------------
include( "InDetRecExample/jobOptions.py" )


IDRStatAlg = Algorithm( "InDet::InDetRecStatisticsAlg" )
InDet__InDetRecStatisticsAlg.OutputLevel = DEBUG
