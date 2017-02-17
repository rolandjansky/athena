# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def MuonScaleConfig( OutputName ):

    from RecExConfig.RecFlags  import rec
    rec.doTruth.set_Value_and_Lock(False)
    from MuonPtCalibNtupleMaker import MuonPtCalibNtupleMaker_Flags
    from MuonPtCalibNtupleMaker.MuonPtCalibNtupleMaker_Flags import MuonPtCalibNtupleMaker_Flags
    from AthenaCommon.JobProperties import jobproperties

    flags = jobproperties.MuonPtCalibNtupleMaker_Flags
    flags.outputFilename = OutputName

    rec.UserAlgs += ["MuonPtCalibNtupleMaker/MuonPtCalibNtupleMaker_AnalysisAlgs.py"]
