# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def addLArCalibFlags(flags):
    
    flags.Input.isMC=False
    flags.addFlag("LArCalib.BadChannelDB","COOLOFL_LAR/COMP200")
    flags.addFlag("LArCalib.BadChannelTag","-UPD3-00")
    flags.addFlag("LArCalib.Pedestal.Folder","/LAr/ElecCalibOfl/Pedestals/Pedestal")
    flags.addFlag("LArCalib.AutoCorr.Folder","/LAr/ElecCalibOfl/AutoCorrs/AutoCorr")
    flags.addFlag("LArCalib.GroupingType","ExtendedSubDetector")
    flags.addFlag("LArCalib.Output.POOLFile","ouput.pool.root")
    flags.addFlag("LArCalib.Output.ROOTFile","")

    flags.addFlag("LArCalib.Gain",0)

    flags.addFlag("LArCalib.doValidation",True)

    #Flags to find the input files
    flags.addFlag("LArCalib.Input.Dir",".")
    flags.addFlag("LArCalib.Input.RunNumbers",[])
    flags.addFlag("LArCalib.Input.Type","calibration_LArElec-Pedestal")
    flags.addFlag("LArCalib.Input.Files",_getInputFiles)


def _getInputFiles(prevFlags):
    from LArCalibProcessing.GetInputFiles import GetInputFiles
    allFiles=[]
    for rn in prevFlags.LArCalib.Input.RunNumbers:
        pattern="data.*%(rn)07d.%(type)s.*" % {'rn' : rn,'type' : prevFlags.LArCalib.Input.Type }
        allFiles+=GetInputFiles(prevFlags.LArCalib.Input.Dir,pattern)
        
    return allFiles

    
