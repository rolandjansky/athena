
#--------------------------------------------------------------
# JobOptions for user setting in SCTCalib/SCTCalibWriteSvc
# - Minimum inputs to run each algorithm
#--------------------------------------------------------------

#--------------------------------------------------------------
# RunNumber and number of events
#--------------------------------------------------------------
RunNumber = 256721
EvtMax    = -1  

#--------------------------------------------------------------
# Geometry and Conditons Tag
# - If empty, one tag is automatically chosen in topOptions
#--------------------------------------------------------------
GeometryTag   = '' # empty in default
ConditionsTag = '' # empty in default

#--------------------------------------------------------------
# Global flags and beam flag
#--------------------------------------------------------------
DataSource = 'data'       # 'data' or 'geant4'
beamType   = 'cosmics' # 'cosmics', 'singlebeam' or 'collisions'

#------------------------------------------------------------
# Flags for input 
# - TrkVal/BS : NoisyStrips, HV, DeadChip, DeadStrip
# - HIST      : NoiseOccupancy, RawOccupancy, Efficiency, BSErrorDB, LorentzAngle 
#------------------------------------------------------------
ReadBS         = True
#------------------------------------------------------------
# Algorithm turned on
# - Set only one flag to True and others to False
#------------------------------------------------------------
#--- Algs using TrkVal or BS
DoNoisyStrip     = False
DoNoisyLB        = False 
DoHV             = False
DoDeadStrip      = False
DoDeadChip       = True
#--- Algs using HIST
DoNoiseOccupancy = False 
DoRawOccupancy   = False
DoEfficiency     = False
DoBSErrorDB      = False 
DoLorentzAngle   = False 

#------------------------------------------------------------
# Default setup for specific flags in SCTCalib
# - This default is for NoisyStrips
#------------------------------------------------------------
UseDCS           = False 
UseConfiguration = True 
UseCalibration   = True 
UseMajority      = True 
UseBSError       = False
#agrohsje not used: UseMonitor       = False #True
DoHitMaps        = True 
ReadHitMaps      = True # True = 2 steps of HitMap production and analysis, False = simultaneous production and analysis
DoBSErrors       = False

#------------------------------------------------------------
# Properties in SCTCalib
#------------------------------------------------------------
#--- local DB
WriteToCool = True

# hitmaps
LbsPerWindow           = 20

#--- Noisy strips
NoisyUpdate            = True     # True = Write out difference from the existing data, False = Write out all
#NoisyMinStat           = 100    # 50000 (enough), 10000 (still possible), 5000 (for commissioning)
NoisyMinStat           = 10000    # 50000 (enough), 10000 (still possible), 5000 (for commissioning)
NoisyStripAll          = True     # True = All noisy strips into DB, False = Only newly found strips
NoisyStripThrDef       = True     # True = Offline, False = Calib
NoisyStripThrOffline   = 1.500E-2 # Threshold in Offline
NoisyStripThrOnline    = 1.500E-3 # Threshold in Online/Calibration (TBD)
NoisyWaferFinder       = True     # True = turn on noisy wafer criteria, False = turn off
NoisyWaferWrite        = True     # True = include noisy wafer, False = do not include
NoisyWaferAllStrips    = False     # True = write out all strips, False = only noisy strips
NoisyWaferThrBarrel    = 1.000E-3
NoisyWaferThrECA       = 1.000E-3
NoisyWaferThrECC       = 1.000E-3
NoisyWaferFraction     = 0.500
NoisyChipFraction      = 0.500
NoisyUploadTest        = True     #only false during the first few runs
#NoisyUploadTest        = True
NoisyModuleAverageInDB = -1.
NoisyStripLastRunInDB  = -999
NoisyStripAverageInDB  = -999.
NoisyModuleList        = 5000
NoisyModuleDiff        = 0.500
NoisyStripDiff         = 5120
            
#--- Dead strips/chips
DeadStripMinStat      = 0
#DeadStripMinStat      = 1600000
DeadStripMinStatBusy  = 200000
DeadChipMinStat       = 0 
#DeadChipMinStat       = 200000 
DeadStripSignificance = 10
DeadChipSignificance  = 10
BusyThr4DeadFinding   = 1E-4
NoisyThr4DeadFinding  = 1.500E-3
DeadChipUploadTest    = True
DeadStripUploadTest   = True

#--- HIST
NoiseOccupancyTriggerAware = True
NoiseOccupancyMinStat = 5000
RawOccupancyMinStat   = 5000
EfficiencyMinStat     = 5000
BSErrorDBMinStat      = 5000
LorentzAngleMinStat   = 5000

#--- Lorentz Angle Debug Mode
LorentzAngleDebugMode = True 

#--------------------------------------------------------------
# Tags for local DB and XML files in SCTCalib/SCTCalibWriteSvc
#--------------------------------------------------------------
TagID4NoisyStrips    = 'SctDerivedMonitoring-RUN2-UPD4-005' # UPD4, open IOV for BLK 
#TagID4NoisyStrips    = 'SctDerivedMonitoring-RUN2-UPD4-004' # UPD4, open IOV for BLK
TagID4DeadStrips     = 'SctDerivedDeadStrips-003-00'
TagID4DeadChips      = 'SctDerivedDeadChips-003-00'
TagID4NoiseOccupancy = 'SctDerivedNoiseOccupancy-003-01' 
TagID4RawOccupancy   = 'SctDerivedRawOccupancy-003-00'
TagID4Efficiency     = 'SctDerivedEfficiency-003-00'
TagID4BSErrors       = 'SctDerivedBSErrorsRun2-001-00'
TagID4LorentzAngle   = 'SctDerivedLorentzAngleRun2_v2-001-00'

#--------------------------------------------------------------
# A list of GeometryTags
# - One tag is chosen in topOptions from magnet current info
#--------------------------------------------------------------
# commented, A.N., 2014-11-28
# GeometryTagSTF = 'ATLAS-GEO-20-00-01'    # Solenoide=ON,  BarrelTroid=ON
# GeometryTagNTF = 'ATLAS-GEO-20-00-01'    # Solenoide=ON,  BarrelTroid=OFF
# GeometryTagNSF = 'ATLAS-GEO-20-00-01'    # Solenoide=OFF, BarrelTroid=ON
# GeometryTagNF  = 'ATLAS-GEO-20-00-01'    # Solenoide=OFF, BarrelTroid=OFF
# GeometryTagMC  = 'ATLAS-GEO-20-00-01'    # MC
# commented, M.D., 2016-11-28 
#GeometryTagSTF = 'ATLAS-R2-2015-02-00-00'    # Solenoid=ON,  BarrelToroid=ON
#GeometryTagNTF = 'ATLAS-R2-2015-02-00-00'    # Solenoid=ON,  BarrelToroid=OFF
#GeometryTagNSF = 'ATLAS-R2-2015-02-00-00'    # Solenoid=OFF, BarrelToroid=ON
#GeometryTagNF  = 'ATLAS-R2-2015-02-00-00'    # Solenoid=OFF, BarrelToroid=OFF
#GeometryTagMC  = 'ATLAS-R2-2015-02-00-00'    # MC
# added, M.D., 2016-11-28, to work with rel22
GeometryTagSTF = 'ATLAS-R2-2015-03-00-00'    # Solenoid=ON,  BarrelToroid=ON
GeometryTagNTF = 'ATLAS-R2-2015-03-00-00'    # Solenoid=ON,  BarrelToroid=OFF
GeometryTagNSF = 'ATLAS-R2-2015-03-00-00'    # Solenoid=OFF, BarrelToroid=ON
GeometryTagNF  = 'ATLAS-R2-2015-03-00-00'    # Solenoid=OFF, BarrelToroid=OFF
GeometryTagMC  = 'ATLAS-R2-2015-03-00-00'    # MC


#--------------------------------------------------------------
# A list of ConditionsTags
# - One tag is chosen in topOptions from magnet current info
#--------------------------------------------------------------
#--- MC
ConditionsTagMC  = 'OFLCOND-DR-BS7T-ANom-00' # digi+rec for MC09 - Nominal
#--- BLK : bulk production
#ConditionsTagSTF = 'COMCOND-BLKPST-004-02'   # Solenoide=ON,  BarrelTroid=ON
#ConditionsTagNTF = 'COMCOND-BLKPS-004-02'    # Solenoide=ON,  BarrelTroid=OFF
#ConditionsTagNSF = 'COMCOND-BLKPT-004-02'    # Solenoide=OFF, BarrelTroid=ON
#ConditionsTagNF  = 'COMCOND-BLKP-004-02'     # Solenoide=OFF, BarrelTroid=OFF
#commented, A.N., 2014-11-28
# ConditionsTagSTF = 'COMCOND-BLKPA-006-03'     # Solenoide=ON,  BarrelTroid=ON
# ConditionsTagNTF = 'COMCOND-BLKPA-006-03'     # Solenoide=ON,  BarrelTroid=OFF
# ConditionsTagNSF = 'COMCOND-BLKPA-006-03'     # Solenoide=OFF, BarrelTroid=ON
# ConditionsTagNF  = 'COMCOND-BLKPA-006-03'     # Solenoide=OFF, BarrelTroid=OFF
#--- ES1 : prompt reconstruction
# agrohsje not properly done, check for the current one !!!
#ConditionsTagSTF = 'COMCOND-ES1PA-006-02'   # Solenoide=ON,  BarrelTroid=ON
#ConditionsTagNTF = 'COMCOND-ES1PA-006-02'    # Solenoide=ON,  BarrelTroid=OFF
#ConditionsTagNSF = 'COMCOND-ES1PA-006-02'    # Solenoide=OFF, BarrelTroid=ON
#ConditionsTagNF  = 'COMCOND-ES1PA-006-02'     # Solenoide=OFF, BarrelTroid=OFF
# # added, A.N., 2014-11-28 
ConditionsTagSTF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=ON,  BarrelToroid=ON
ConditionsTagNTF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=ON,  BarrelToroid=OFF
ConditionsTagNSF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=OFF, BarrelToroid=ON
ConditionsTagNF  = 'CONDBR2-ES1PA-2016-03'    # Solenoid=OFF, BarrelToroid=OFF
# ConditionsTagSTF = 'CONDBR2-BLKPA-2015-03'    # Solenoid=ON,  BarrelToroid=ON
# ConditionsTagNTF = 'CONDBR2-BLKPA-2015-03'    # Solenoid=ON,  BarrelToroid=OFF
# ConditionsTagNSF = 'CONDBR2-BLKPA-2015-03'    # Solenoid=OFF, BarrelToroid=ON
# ConditionsTagNF  = 'CONDBR2-BLKPA-2015-03'    # Solenoid=OFF, BarrelToroid=OFF
#--- REP : reporcessing
#ConditionsTagSTF = 'COMCOND-REPPST-006-00'   # Solenoide=ON,  BarrelTroid=ON
#ConditionsTagNTF = 'COMCOND-REPPS-006-00'    # Solenoide=ON,  BarrelTroid=OFF
#ConditionsTagNSF = 'COMCOND-REPPT-006-00'    # Solenoide=OFF, BarrelTroid=ON
#ConditionsTagNF  = 'COMCOND-REPP-006-00'     # Solenoide=OFF, BarrelTroid=OFF

#-------------------------------------------------------------- 
# Read /SCT/Derived/Monotoring in COOL 
# - Used in a criteria for upload of NoisyStrips 
NoisyReadNumRuns = 10
TagID4NoisyUploadTest = 'SctDerivedMonitoring-RUN2-UPD4-005'
#TagID4NoisyUploadTest = 'SctDerivedMonitoring-RUN2-UPD4-004'
