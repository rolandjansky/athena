# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def addLArCalibFlags(flags):
    
    flags.Input.isMC=False
    flags.addFlag("LArCalib.isSC",False)
    flags.addFlag("LArCalib.BadChannelDB","COOLOFL_LAR")
    flags.addFlag("LArCalib.BadChannelTag","-UPD3-00")

    #Folders:
    def _prefix(prevFlags):
        if prevFlags.LArCalib.isSC:
            return "/LAR/ElecCalibOflSC/"
        else:
            return "/LAR/ElecCalibOfl/"

    flags.addFlag("LArCalib.Pedestal.Folder", lambda prevFlags: _prefix(prevFlags)+"Pedestals/Pedestal")
    flags.addFlag("LArCalib.AutoCorr.Folder", lambda prevFlags: _prefix(prevFlags)+"AutoCorrs/AutoCorr")
    flags.addFlag("LArCalib.PhysAutoCorr.Folder", lambda prevFlags: _prefix(prevFlags)+"AutoCorrs/PhysicsAutoCorr")
    flags.addFlag("LArCalib.CaliWave.Folder", lambda prevFlags: _prefix(prevFlags)+"CaliWaves/CaliWave")
    flags.addFlag("LArCalib.OFCCali.Folder",  lambda prevFlags: _prefix(prevFlags)+"OFC/CaliWave")
    flags.addFlag("LArCalib.PhysWave.Folder", lambda prevFlags: _prefix(prevFlags)+"PhysWaves/RTM")
    flags.addFlag("LArCalib.Ramp.Folder",     lambda prevFlags: _prefix(prevFlags)+"Ramps/RampLinea")
    flags.addFlag("LArCalib.OFCPhys.Folder",  lambda prevFlags: _prefix(prevFlags)+"OFC/PhysWave/RTM/")
    flags.addFlag("LArCalib.Shape.Folder",  lambda prevFlags: _prefix(prevFlags)+"Shape/RTM/")
    flags.addFlag("LArCalib.MPhysOverMCal.Folder", lambda prevFlags: _prefix(prevFlags)+"MphysOverMcal/RTM")
    flags.addFlag("LArCalib.PhysCaliTdiff.Folder",lambda prevFlags: _prefix(prevFlags)+"PhysCaliTdiff")
    flags.addFlag("LArCalib.CaliPulseParams.Folder", lambda prevFlags: _prefix(prevFlags)+"CaliPulseParams/RTM")
    flags.addFlag("LArCalib.DetCellParams.Folder", lambda prevFlags: _prefix(prevFlags)+"DetCellParams/RTM")


    flags.addFlag("LArCalib.GroupingType",lambda prevFlags: "SuperCells" if prevFlags.LArCalib.isSC else "ExtendedSubDetector")
    flags.addFlag("LArCalib.Output.POOLFile","ouput.pool.root")
    flags.addFlag("LArCalib.Output.ROOTFile","")

    flags.addFlag("LArCalib.Gain",0)

    flags.addFlag("LArCalib.doValidation",True)

    flags.addFlag("LArCalib.CorrectBadChannels",lambda prevFlags: not prevFlags.LArCalib.isSC)

    #Flags to find the input files/databases
    flags.addFlag("LArCalib.Input.Dir",".")
    flags.addFlag("LArCalib.Input.RunNumbers",[])
    flags.addFlag("LArCalib.Input.Type","calibration_LArElec-Pedestal")
    flags.addFlag("LArCalib.Input.Files",_getInputFiles)
    flags.addFlag("LArCalib.Input.SubDet","") #EM, HEC or FCAL 
    
    flags.addFlag("LArCalib.Input.Database","LAR_OFL") #In practice, a sqlite file

    flags.addFlag("LArCalib.Input.ChannelSelection","") #Read only a subset of COOL channels. Format like '3,4,5:10' 

    flags.addFlag("LArCalib.Preselection.Side",[])
    flags.addFlag("LArCalib.Preselection.BEC",[])
    flags.addFlag("LArCalib.Preselection.FT",[])
    flags.addFlag("LArCalib.Preselection.Slot",[])


    flags.addFlag("LArCalib.RTM.ExtractAll",True) # False = extract only tauR 
    flags.addFlag("LArCalib.RTM.DumpOmegaScan",False)
    flags.addFlag("LArCalib.RTM.DumpResOscill",False)



    flags.addFlag("LArCalib.OFC.Ncoll",0)
    flags.addFlag("LArCalib.OFC.UsePhysCalibTDiff",True)
    flags.addFlag("LArCalib.OFC.Normalize",True)
    flags.addFlag("LArCalib.OFC.useDelta",False)
    flags.addFlag("LArCalib.OFC.Nsamples",4)


def _getInputFiles(prevFlags):
    from LArCalibProcessing.GetInputFiles import GetInputFiles
    allFiles=[]
    for rn in prevFlags.LArCalib.Input.RunNumbers:
        pattern="data.*%(rn)07d.%(type)s.*" % {'rn' : rn,'type' : prevFlags.LArCalib.Input.Type }
        allFiles+=GetInputFiles(prevFlags.LArCalib.Input.Dir,pattern)
        
    return allFiles

    
