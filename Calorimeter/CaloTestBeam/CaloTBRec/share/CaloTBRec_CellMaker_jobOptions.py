#*******************************************************************
#
# JobOption to analyse Combined testbeam data : Calo reconstruction
#   
#  same jobOpt as RecExCommon, with DetFlags.makeRIO.Calo_on()
#*******************************************************************

#abc --------- RawChannel -> Cell ------------
#abc
#abc These 2 lines are identical to RecExCommon :
#abc
include( "CaloRec/CaloCellMaker_config.py" )
include( "CaloRec/CaloCellMaker_jobOptions.py" )

#abc  ==> modified because collections have a different name here:
if doTile:
    include( "TileTBRec/TileTBRCToCellSelect_jobOptions.py" )

if doLAr:
    #already above ToolSvc = Service( "ToolSvc" )
    ToolSvc.cellbuilderLAr.LArRegion = "LAr_EM"
    ToolSvc.cellbuilderLAr.EThreshold =-100000. * MeV  
