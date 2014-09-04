# jobOptions file for LAr Cell Reconstruction from LArRawChannel 
# with full corrections. 

# SUBCALO enum LAREM LARHEC LARFCAL TILE NSUBCALO NOT_VALID
from CaloIdentifier import SUBCALO

# Properties of LArCellRec:
theApp.Dlls += [ "LArCellRec" ]
theApp.Dlls += [ "LArTools","LArCalibUtils" ]
theApp.Dlls += [ "LArRawUtils","LArRecUtils" ]

#add cell maker tools to top cell maker alg (defined elsewhere)
# if all LAR required only one builder
if DetFlags.makeRIO.LAr_allOn() and not CaloCellMakerFlags.doLArHitToCellDirect :
    CaloCellMaker.CaloCellMakerToolNames += [
                      "LArCellBuilderFromLArRawChannelTool/LArCellBuilderLAr" ]
    # CellBuilder properties
    ToolSvc.cellbuilderLAr.LArRegion = ""
    # ToolSvc.cellbuilderLAr.EThreshold =-100000. * MeV  
else:
    # one builder per lar calo
    if DetFlags.makeRIO.em_on():
        if CaloCellMakerFlags.doLArHitToCellDirect:
            theName="LArCellBuilderFromLArHitTool/LArCellbuilderem"
            ToolSvc.LArCellBuilderem.WithNoise= doCaloNoise
        else:
            theName="LArCellBuilderFromLArRawChannelTool/LArCellbuilderem"

        CaloCellMaker.CaloCellMakerToolNames += [ theName ]
        ToolSvc.LArCellBuilderem.LArRegion = "LAr_EM"
    if DetFlags.makeRIO.HEC_on():
        if CaloCellMakerFlags.doLArHitToCellDirect:
            theName="LArCellBuilderFromLArHitTool/LArCellbuilderHEC"
            ToolSvc.LArCellBuilderHEC.WithNoise= doCaloNoise
        else:
            theName="LArCellBuilderFromLArRawChannelTool/LArCellbuilderHEC"
        CaloCellMaker.CaloCellMakerToolNames += [ theName ]
        ToolSvc.LArCellBuilderHEC.LArRegion = "LAr_HEC"
    if DetFlags.makeRIO.FCal_on():
        if CaloCellMakerFlags.doLArHitToCellDirect:
            theName="LArCellBuilderFromLArHitTool/LArCellBuilderFCal"
            ToolSvc.LArCellBuilderFCal.WithNoise= doCaloNoise
        else:
            theName="LArCellBuilderFromLArRawChannelTool/LArCellbuilderFCal"
        CaloCellMaker.CaloCellMakerToolNames += [ theName ]        
        ToolSvc.LArCellBuilderFCal.LArRegion = "LAr_FCal"

#possible properties for LArCellBuilderFromLArHitTool (suitable for any LAr calo)
    #if CaloCellMaker.doLArHitToCellDirect:
        # #THRESHOLDS :
        # #------------
        # #thresholds that cells must satisfy to be stored in the CellContainer
        # #set only one threshold please (NB: E in MeV )
        # #E > ...
        # ToolSvc.cellbuilderEM.ThresholdOnE           = -999999.
        # #E > ...*sigma
        # ToolSvc.cellbuilderEM.ThresholdOnEinSigma    = -999999.
        # #abs(E) > ...
        # ToolSvc.cellbuilderEM.ThresholdOnAbsE        = -999999.; 
        # #abs(E) > ...*sigma
        # ToolSvc.cellbuilderEM.ThresholdOnAbsEinSigma = -999999.; 
        # #threshold on energy of the Hits
        # # cellbuilderEM.eHitThreshold =0.000 ;
        # #WINDOWS :
        # #---------
        # #you can go faster in selecting window(s) instead of the whole calorimeter
        # #-> window(s) defined around photon and electron of the truth
        # #(from Geant) with pt> 5 gev (cuts defined in LArHitMap::DefineWindows )
        # #-> window size : deltaEta=0.4 deltaPhi=0.5
        # #This should only be used with care to study high pt photon and electron
        # ToolSvc.cellbuilderEM.Windows=FALSE 

