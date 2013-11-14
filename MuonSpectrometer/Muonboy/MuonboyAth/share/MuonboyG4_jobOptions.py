include.block ( "MuonboyAth/MuonboyG4_jobOptions.py" )
#--------------------------------------------------------------
# MuonboyAth for G4
#--------------------------------------------------------------
#load MboySvc
include( "MboySvc/MboySvcG4_jobOptions.py" )

#--------------------------------------------------------------
#load MuonboyAth
include( "MuonboyAth/Muonboy_jobOptions.py" )

#-----------------------------------------------------------------
if not 'doPersint' in dir():  
     include( "MboyView/MboyView_jobOptions.py" )
