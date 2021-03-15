
#--------------------------------------------------------------
# JobOptions for user setting in SCTCalib/SCTCalibWriteSvc
# - Minimum inputs to run each algorithm
#--------------------------------------------------------------

#--------------------------------------------------------------
# RunNumber and number of events
#--------------------------------------------------------------
#RunNumber = 332896
#RunNumber = 364214
RunNumber = 359678
#RunNumber = 370059
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
DoNoisyStrip     = True
DoNoisyLB        = False 
DoHV             = False
DoDeadStrip      = False
DoDeadChip       = False
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
DoHitMaps        = True 
ReadHitMaps      = True # True = 2 steps of HitMap production and analysis, False = simultaneous production and analysis
DoBSErrors       = False

#------------------------------------------------------------
# Properties in SCTCalib
#------------------------------------------------------------
#--- local DB
WriteToCool = True

# hitmaps
LbsPerWindow           = 30

#--- Noisy strips
NoisyUpdate            = False     # True = Write out difference from the existing data, False = Write out all
NoisyWriteAllModules   = True
NoisyMinStat           = 10000     # 50000 (enough), 10000 (still possible), 5000 (for commissioning)
NoisyStripAll          = True      # True = All noisy strips into DB, False = Only newly found strips
NoisyStripThrDef       = True      # True = Offline, False = Calib
NoisyStripThrOffline   = 1.500E-2  # Threshold in Offline
NoisyStripThrOnline    = 1.500E-3  # Threshold in Online/Calibration (TBD)
NoisyWaferFinder       = True      # True = turn on noisy wafer criteria, False = turn off
NoisyWaferWrite        = True      # True = include noisy wafer, False = do not include
NoisyWaferAllStrips    = False     # True = write out all strips, False = only noisy strips
NoisyWaferThrBarrel    = 1.000E-3
NoisyWaferThrECA       = 1.000E-3
NoisyWaferThrECC       = 1.000E-3
NoisyWaferFraction     = 0.500
NoisyChipFraction      = 0.500
NoisyUploadTest        = True      #only false during the first few runs
NoisyModuleAverageInDB = -1.
NoisyStripLastRunInDB  = -999
NoisyStripAverageInDB  = -999.
NoisyModuleList        = 5000
NoisyModuleDiff        = 0.500
NoisyStripDiff         = 5120
            
#--- Dead strips/chips
DeadStripMinStat      = 5000
DeadStripMinStatBusy  = 5000
DeadChipMinStat       = 5000 
DeadStripSignificance = 6
DeadChipSignificance  = 6
BusyThr4DeadFinding   = 1E-4
NoisyThr4DeadFinding  = 5.0E-2
DeadChipUploadTest    = True
DeadStripUploadTest   = True
DeadNotQuiet          = False
QuietThresholdStrip   = 0.5
QuietThresholdChip    = 0.5

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
ConditionsTagSTF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=ON,  BarrelToroid=ON
ConditionsTagNTF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=ON,  BarrelToroid=OFF
ConditionsTagNSF = 'CONDBR2-ES1PA-2016-03'    # Solenoid=OFF, BarrelToroid=ON
ConditionsTagNF  = 'CONDBR2-ES1PA-2016-03'    # Solenoid=OFF, BarrelToroid=OFF

#-------------------------------------------------------------- 
# Read /SCT/Derived/Monotoring in COOL 
NoisyReadNumRuns = 3   # Used in a criteria for upload of NoisyStrips 
TagID4NoisyUploadTest = 'SctDerivedMonitoring-RUN2-UPD4-005'
