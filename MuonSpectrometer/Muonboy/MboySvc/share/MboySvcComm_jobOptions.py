include.block ( "MboySvc/MboySvcComm_jobOptions.py" )
#--------------------------------------------------------------
# MboySvc for Commissioning
#--------------------------------------------------------------
#load MboySvc
include( "MboySvc/MboySvc_jobOptions.py" )

#-----------------------------------------------------------------
MboySvc = Service( "MboySvc" )
MboySvc.AtlasCosmicsSample     = 1

MboySvc.WireLengthOfConstRT    = 99999.9 

MboySvc.LevelOfRecons          = 3

MboySvc.AlignmentCorr          = 0

