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
job.MuonCalibrationFit.GRL_Name = '_GRL_' 
job.MuonCalibrationFit.MonitoringPlots = _MONITORING_
job.MuonCalibrationFit.MaxRetrials = _MAXRETRIALS_
job.MuonCalibrationFit.ProbThreshold = _PROBTHRESHOLD_
job.MuonCalibrationFit.RequireErrors = _REQUIRE_ERRORS_
job.MuonCalibrationFit.MinMuonsPtForJpsi = _MIN_JPSI_PT_CUT_
job.MuonCalibrationFit.MaxMuonsPtForJpsi = _MAX_JPSI_PT_CUT_
job.MuonCalibrationFit.MinLeadingMuonPtForZ = _MIN_Z_LEAD_PT_CUT_
job.MuonCalibrationFit.MinSubLeadingMuonPtForZ =_MIN_Z_SUB_PT_CUT_ 
job.MuonCalibrationFit.Input = '_INPUT_'
job.MuonCalibrationFit.InputRegionsFile = '_INPUTREGION_' 
job.MuonCalibrationFit.InputForID = '_INPUTFORID_'
job.MuonCalibrationFit.InputRegionsFileForID = '_INPUTREGIONFORID_' 
job.MuonCalibrationFit.Output = '_OUTPUT_'
job.MuonCalibrationFit.OutputRegionsFile = '_OUTPUTREGION_' 
job.MuonCalibrationFit.DataFile = '_DATA_' 
job.MuonCalibrationFit.MonteCarloFile = '_MONTECARLO_' 
job.MuonCalibrationFit.BackgroundFile = '_BACKGROUND_' 
job.MuonCalibrationFit.JpsiReweightingFile = '_JPSI_REW_' 
job.MuonCalibrationFit.ZReweightingFile = '_Z_REW_' 
job.MuonCalibrationFit.JpsiMass_Bins = _JPSI_MASS_BINS_ 
job.MuonCalibrationFit.JpsiMass_Min = _JPSI_MASS_MIN_ 
job.MuonCalibrationFit.JpsiMass_Max = _JPSI_MASS_MAX_ 
job.MuonCalibrationFit.JpsiMass_Pts = '_JPSI_MASS_PTS_'
job.MuonCalibrationFit.ZMass_Bins = _Z_MASS_BINS_ 
job.MuonCalibrationFit.ZMass_Min = _Z_MASS_MIN_ 
job.MuonCalibrationFit.ZMass_Max = _Z_MASS_MAX_ 
job.MuonCalibrationFit.ZMass_Pts = '_Z_MASS_PTS_'
job.MuonCalibrationFit.ZRho_Bins = _Z_RHO_BINS_ 
job.MuonCalibrationFit.ZRho_Min = _Z_RHO_MIN_ 
job.MuonCalibrationFit.ZRho_Max = _Z_RHO_MAX_ 
job.MuonCalibrationFit.ZRho_Pts = '_Z_RHO_PTS_'
job.MuonCalibrationFit.Use_S0 = _USE_S0_
job.MuonCalibrationFit.Use_S1 = _USE_S1_
job.MuonCalibrationFit.Use_P0 = _USE_P0_
job.MuonCalibrationFit.Use_P1 = _USE_P1_
job.MuonCalibrationFit.Use_P2 = _USE_P2_
job.MuonCalibrationFit.S0_Init = _INIT_S0_
job.MuonCalibrationFit.S1_Init = _INIT_S1_
job.MuonCalibrationFit.P0_Init = _INIT_P0_
job.MuonCalibrationFit.P1_Init = _INIT_P1_
job.MuonCalibrationFit.P2_Init = _INIT_P2_
job.MuonCalibrationFit.MaxEvents = _MAX_EVENTS_

ServiceMgr.MessageSvc.defaultLimit = 1000000

# --- Set application parameters --- 
theApp.EvtMax = 1
