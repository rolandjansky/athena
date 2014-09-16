if Type == 'Ramp':
    theApp.TopAlg+=["LArRampBuilder"] # test avec un t majuscule a top
    LArRampBuilder=Algorithm("LArRampBuilder")
    LArRampBuilder.SubtractDac0=FALSE
    LArRampBuilder.DAC0=-1           # As the first ramp point no longer is taken at DAC=0, this point should not have a special place.
    LArRampBuilder.StoreRawRamp=TRUE
    LArRampBuilder.StoreRecRamp=TRUE
    LArRampBuilder.Polynom=1
    LArRampBuilder.WriteDB=False
    LArRampBuilder.RecoType="Parabola"
    LArRampBuilder.RampRange = 4000;  # The first DAC is not substracted, hence saturation occurs above 4000
    LArRampBuilder.correctBias=True;
    LArRampBuilder.ConsecutiveADCs=0; # As the number of ramppoints was increased to 6, the demand that two points are 50 DAC counts apart no longer works
    LArRampBuilder.KeyList =[ LArMonFlags.LArDigitKey() ]

elif Type == 'Delay':
    theApp.Dlls   += ["LArCalibUtils"]
    theApp.TopAlg += ["LArCaliWaveBuilder"]
    LArCaliWaveBuilder=Algorithm("LArCaliWaveBuilder")
    LArCaliWaveBuilder.KeyList         = [LArMonFlags.LArDigitKey()]
    LArCaliWaveBuilder.ADCsaturation   = 4090
    LArCaliWaveBuilder.SubtractPed     = False
    LArCaliWaveBuilder.RecAllCells     = False
    LArCaliWaveBuilder.WriteDB         = False
    LArCaliWaveBuilder.PreviousRunToDB = 1
    # LArCaliWaveBuilder.FolderName      = "LArElecCalibTB04"
    #
    
    from LArCalibTools.LArCalibToolsConf import LArWaves2Ntuple
    topSequence += LArWaves2Ntuple( "LArCaliWaves2Ntuple" )
    LArCaliWaves2Ntuple = topSequence.LArCaliWaves2Ntuple
    LArCaliWaves2Ntuple.NtupleName     = "CALIWAVE"
    LArCaliWaves2Ntuple.KeyList     = ["LArCaliWave"]
    
