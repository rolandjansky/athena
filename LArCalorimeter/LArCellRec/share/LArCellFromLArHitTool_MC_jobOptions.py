# jobOptions file for LAr Cell Reconstruction from LArHit
# with full corrections and with noise

# Input: LArHit
include( "LArSim/LArSim_MC_jobOptions.py" )

# New Database
include( "LArCondCnv/LArCondCnv_G3_jobOptions.py" )

# Properties of LArCellRec:
theApp.Dlls += [ "LArCellRec" ]
# Properties of LArCellRec:
theApp.Dlls += [ "LArRawUtils",  "LArRecUtils", "LArCellRec","LArCalibUtils" ]

#add cell maker tools to top cell maker alg (defined elsewhere)
CaloCellMaker.CaloCellMakerToolNames += [
                           "LArCellBuilderFromLArHitTool/cellbuilderEM",
		           "LArCellBuilderFromLArHitTool/cellbuilderHEC",
			   "LArCellBuilderFromLArHitTool/cellbuilderFCal" ]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
# CellBuilder properties
ToolSvc.cellbuilderEM.LArRegion = "LAr_EM"
ToolSvc.cellbuilderEM.WithNoise=FALSE #choose if you want with or without noise
ToolSvc.cellbuilderEM.NoiseTool="CaloNoiseTool/calonoisetool"
if GlobalFlags.DataSource.isGeant3():
    ToolSvc.cellbuilderEM.Geant3= True ;
# THRESHOLDS :
# ------------
# thresholds that cells must satisfy to be stored in the CellContainer
# set only one threshold please (NB: E in MeV )
#E > ...
ToolSvc.cellbuilderEM.ThresholdOnE           = -999999.
#E > ...*sigma
ToolSvc.cellbuilderEM.ThresholdOnEinSigma    = -999999.
#abs(E) > ...
ToolSvc.cellbuilderEM.ThresholdOnAbsE        = -999999.; 
#abs(E) > ...*sigma
ToolSvc.cellbuilderEM.ThresholdOnAbsEinSigma = -999999.; 
#threshold on energy of the Hits
#  cellbuilderEM.eHitThreshold =0.000 ;
# WINDOWS :
# ---------
#you can go faster in selecting window(s) instead of the whole calorimeter
# -> window(s) defined around photon and electron of the truth
# (from Geant) with pt> 5 gev (cuts defined in LArHitMap::DefineWindows )
# -> window size : deltaEta=0.4 deltaPhi=0.5
# This should only be used with care to study high pt photon and electron
ToolSvc.cellbuilderEM.Windows=FALSE; 



# CellBuilder properties
ToolSvc.cellbuilderHEC.LArRegion = "LAr_HEC"
ToolSvc.cellbuilderHEC.WithNoise=FALSE #choose if you want with or without noise
ToolSvc.cellbuilderHEC.NoiseTool="CaloNoiseTool/calonoisetool"
if GlobalFlags.DataSource.isGeant3():
    ToolSvc.cellbuilderHEC.Geant3= True ;

# THRESHOLDS :
# ------------
# thresholds that cells must satisfy to be stored in the CellContainer
# set only one threshold please (NB: E in MeV )
#E > ...
ToolSvc.cellbuilderHEC.ThresholdOnE           = -999999.
#E > ...*sigma
ToolSvc.cellbuilderHEC.ThresholdOnEinSigma    = -999999.
#abs(E) > ...
ToolSvc.cellbuilderHEC.ThresholdOnAbsE        = -999999.; 
#abs(E) > ...*sigma
ToolSvc.cellbuilderHEC.ThresholdOnAbsEinSigma = -999999.; 
#threshold on energy of the Hits
#  cellbuilderEM.eHitThreshold =0.000 ;
# WINDOWS :
# ---------
#you can go faster in selecting window(s) instead of the whole calorimeter
# -> window(s) defined around photon and electron of the truth
# (from Geant) with pt> 5 gev (cuts defined in LArHitMap::DefineWindows )
# -> window size : deltaEta=0.4 deltaPhi=0.5
# This should only be used with care to study high pt photon and electron
ToolSvc.cellbuilderHEC.Windows=FALSE; 


# CellBuilder properties
ToolSvc.cellbuilderFCal.LArRegion = "LAr_FCal"
ToolSvc.cellbuilderFCal.WithNoise=FALSE #choose if you want with or without noise
ToolSvc.cellbuilderFCal.NoiseTool="CaloNoiseTool/calonoisetool"

# THRESHOLDS :
# ------------
# thresholds that cells must satisfy to be stored in the CellContainer
# set only one threshold please (NB: E in MeV )
#E > ...
ToolSvc.cellbuilderFCal.ThresholdOnE           = -999999.
#E > ...*sigma
ToolSvc.cellbuilderFCal.ThresholdOnEinSigma    = -999999.
#abs(E) > ...
ToolSvc.cellbuilderFCal.ThresholdOnAbsE        = -999999.; 
#abs(E) > ...*sigma
ToolSvc.cellbuilderFCal.ThresholdOnAbsEinSigma = -999999.; 
#threshold on energy of the Hits
#  cellbuilderEM.eHitThreshold =0.000 ;
# WINDOWS :
# ---------
#you can go faster in selecting window(s) instead of the whole calorimeter
# -> window(s) defined around photon and electron of the truth
# (from Geant) with pt> 5 gev (cuts defined in LArHitMap::DefineWindows )
# -> window size : deltaEta=0.4 deltaPhi=0.5
# This should only be used with care to study high pt photon and electron
ToolSvc.cellbuilderFCal.Windows=FALSE; 



