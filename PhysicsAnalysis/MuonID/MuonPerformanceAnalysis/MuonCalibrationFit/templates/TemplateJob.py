# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# --- Full job is a list of algorithms ---
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# --- Add top algorithms to be run ---
from MuonCalibrationFit.MuonCalibrationFitConf import MuonCalibrationFit
job += MuonCalibrationFit( 'MuonCalibrationFit' )

job.MuonCalibrationFit.BatchMode = _BATCHMODE_
job.MuonCalibrationFit.Detector = '_DETECTOR_'
job.MuonCalibrationFit.FitType = '_FITTYPE_'
job.MuonCalibrationFit.OutputLevel = _LEVEL_
job.MuonCalibrationFit.BkgParametrization = '_BKGPARAM_'
job.MuonCalibrationFit.Splitting = _SPLITTING_ 
job.MuonCalibrationFit.NumRandomValues = _NUMRANDOM_ 
job.MuonCalibrationFit.RunMode = '_RUNMODE_' 
job.MuonCalibrationFit.MaxRetrials = _MAXRETRIALS_
job.MuonCalibrationFit.ProbThreshold = _PROBTHRESHOLD_
job.MuonCalibrationFit.Input = '_INPUT_'
job.MuonCalibrationFit.InputRegionsFile = '_INPUTREGION_' 
job.MuonCalibrationFit.InputForID = '_INPUTFORID_'
job.MuonCalibrationFit.InputRegionsFileForID = '_INPUTREGIONFORID_' 
job.MuonCalibrationFit.Output = '_OUTPUT_'
job.MuonCalibrationFit.OutputRegionsFile = '_OUTPUTREGION_' 
job.MuonCalibrationFit.DataFile = '_DATA_' 
job.MuonCalibrationFit.MonteCarloFile = '_MONTECARLO_' 
job.MuonCalibrationFit.BackgroundFile = '_BACKGROUND_' 
job.MuonCalibrationFit.JpsiMass_Bins = _JPSI_MASS_BINS_ 
job.MuonCalibrationFit.JpsiMass_Pts = '_JPSI_MASS_PTS_'
job.MuonCalibrationFit.ZMass_Bins = _Z_MASS_BINS_ 
job.MuonCalibrationFit.ZMass_Pts = '_Z_MASS_PTS_'
job.MuonCalibrationFit.Use_S0 = _USE_S0_
job.MuonCalibrationFit.Use_S1 = _USE_S1_
job.MuonCalibrationFit.Use_P0 = _USE_P0_
job.MuonCalibrationFit.Use_P1 = _USE_P1_
job.MuonCalibrationFit.Use_P2 = _USE_P2_

# --- Set application parameters --- 
theApp.EvtMax = 1
