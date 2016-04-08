#*******************************************************************
#
# JobOption to analyse Combined testbeam data : Calo reconstruction
#   
#  same jobOpt as RecExCommon, with DetFlags.makeRIO.Calo_on()
#*******************************************************************

#from CaloRec.CaloRecConf import CaloCellMaker
#CaloCellMaker = CaloCellMaker()
#CaloCellMaker.CaloCellsOutputName = "AllCalo"
#from CaloRec.CaloCellGetter import CaloCellGetter
#CaloCellGetter()
include ("CaloRec/CaloRec_jobOptions.py")

#Apply identical settings for caonoisetool (do not know, who is using it) and CaloNoiseToolDefault
include ( "CaloTools/CaloNoiseTool_TB_jobOptions.py" )
if not doSim:
    ToolSvc.calonoisetool.UseSymmetry=FALSE
    ToolSvc.LArAutoCorrTotalTool.NMinBias=0 
    ToolSvc.calonoisetool.WithOF=FALSE

if doLArOFC:
    ToolSvc.calonoisetool.WithOF=TRUE
    if not doSim:
        ToolSvc.LArOFCTool.FromDatabase=TRUE


#from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
ToolSvc.LArCellBuilderFromLArRawChannelTool.LArRegion = "LAr_EM"
ToolSvc.LArCellBuilderFromLArRawChannelTool.EThreshold =-1.e32* MeV # lowered the threshold to get rid of unuseful warnings! 

#abc : apply HV correction to real data
#if not doSim:
#    if doLArHVCorrection:
#        from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
#        LArCellCorrectorTool = CaloCellContainerCorrectorTool("LArCellCorrectorTool")
#        ToolSvc += LArCellCorrectorTool
#        ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames = [ "LArCellHVCorrection" ]
#        include( "LArRecUtils/LArHVScaleTool_jobOptions.py" ) 
        

#if doTile:
#   if not hasattr( ToolSvc, "TileCellBuilder" ):
#          from TileRecUtils.TileRecUtilsConf import TileCellBuilder
#          theTileCellBuilder=TileCellBuilder()
#          ToolSvc += theTileCellBuilder

#   from TileRecUtils.TileRecFlags import jobproperties
#   if doSim:
          # set correct name of input container in CTB simulation
#          jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"

#   ToolSvc.TileCellBuilder.maskBadChannels = True
#   ToolSvc.TileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()

#   CaloCellMaker.CaloCellMakerToolNames += [ ToolSvc.TileCellBuilder.getFullName() ]



#from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
#CaloCellContainerFinalizerTool = CaloCellContainerFinalizerTool()
#ToolSvc += CaloCellContainerFinalizerTool
#CaloCellMaker.CaloCellMakerToolNames += [ CaloCellContainerFinalizerTool ]
#from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
#CaloCellContainerCheckerTool = CaloCellContainerCheckerTool()
#ToolSvc += CaloCellContainerCheckerTool
#CaloCellMaker.CaloCellMakerToolNames += [CaloCellContainerCheckerTool]

#topSequence += CaloCellMaker
