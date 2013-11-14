include.block ( "MuonboyAth/MuonboyTB_jobOptions.py" )
#--------------------------------------------------------------
# MuonboyAth for TB
#--------------------------------------------------------------
# To load Alignement 
include( "MuonAlignmentCnv/MuonAlignmentCnvAF_jobOptions.py" )

#--------------------------------------------------------------
# AMDB database is needed by muonboy
include( "AmdcAth/AmdcAthTB_jobOptions.py" )
AmdcsimrecAthenaSvc.NameOfTheAmdbFile = "amdb_simrec.H8_2004_CTB_BIR_SURVEY.a.05.data"

#--------------------------------------------------------------
#load MboySvc
include( "MboySvc/MboySvcTB_jobOptions.py" )

#--------------------------------------------------------------
#load MuonboyAth
include( "MuonboyAth/Muonboy_jobOptions.py" )

#-----------------------------------------------------------------
#MboyRec: digits transfert
# AthPosOff = 0 Use Athena positions
#             1 Use Amdb position
MboyRec = Algorithm( "MboyRec" )
MboyRec.AthPosOff = 1

#-----------------------------------------------------------------
# ProcessingStage = 1 produce TrkSegment
#                 = 2 produce TrkTrack     in addition
#                 = 3 produce TrkParticle  in addition
#                 = 4 produce MuonCombined in addition
MboyRec = Algorithm( "MboyRec" )
MboyRec.ProcessingStage = 2

#-----------------------------------------------------------------
if not 'doPersint' in dir():  
     include( "MboyView/MboyView_jobOptions.py" )
