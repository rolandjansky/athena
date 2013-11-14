include.block ( "MboySvc/MboySvcTB_jobOptions.py" )
#--------------------------------------------------------------
# MboySvc for TB
#--------------------------------------------------------------
#load MboySvc
include( "MboySvc/MboySvc_jobOptions.py" )

#-----------------------------------------------------------------
MboySvc = Service( "MboySvc" )
MboySvc.H8testBeamConfig       = 1  

MboySvc.WireLengthOfConstRT    = 99999.9 

MboySvc.LevelOfRecons          = 0

MboySvc.AlignmentCorr          = 3    

#-----------------------------------------------------------------
# RToption = 2  H8 MuonTBCalilbrationSvc
#            3  G4 MdtCalibrationSvc
#            4  G4 MdtDriftCircleOnTrackCreator
ToolSvc.ConfiguredMboyRTTool.RToption = 2 ;   
