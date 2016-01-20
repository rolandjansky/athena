###############################################################
#
# Job options 
#
#==============================================================

#--------------------------------------------------------------
# ATLAS default Application Configuration options
#--------------------------------------------------------------

# Use McEventSelector so we can run with AthenaMP
import AthenaCommon.AtlasUnixGeneratorJob

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

# the global detflags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# Add top algorithms to be run
from TrkDigTools.TrkDigToolsConf import Trk__PlanarModuleStepper
PlanarModuleStepper = Trk__PlanarModuleStepper('PlanarModuleStepper')
ToolSvc += PlanarModuleStepper

# Add top algorithms to be run
from TrkDigUnitTests.TrkDigUnitTestsConf import Trk__DigitizationModuleTest
DigitizationModuleTest = Trk__DigitizationModuleTest('DigitizationModuleTest')
# Output
DigitizationModuleTest.OutputLevel            = VERBOSE
# Stepper Tool
DigitizationModuleTest.DigitizationStepper    = PlanarModuleStepper
# Display test 
DigitizationModuleTest.BinsX                  = 328
DigitizationModuleTest.BinsY                  = 144
DigitizationModuleTest.PitchX                 = 0.05
#DigitizationModuleTest.PitchXmax              = 10.
DigitizationModuleTest.PitchY                 = 0.4
DigitizationModuleTest.ModuleHalfThickness    = 0.125
DigitizationModuleTest.ModuleReadoutDirection = -1
DigitizationModuleTest.LorentzAngle           = 0. #-0.35
#DigitizationModuleTest.TestTrapezoidal        = True
# the run parameters
DigitizationModuleTest.MaxClusterSizeX        = 4
DigitizationModuleTest.MaxClusterSizeY        = 7  
# number of tests                             
DigitizationModuleTest.NumberOfTestsPerEvent  = 100

job += DigitizationModuleTest   # 1 alg, named "DigitizationModuleTest"

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

###### launch VP1
#from VP1Algs.VP1AlgsConf import VP1Alg
#VP1Alg.InitiallyLoadedVP1Files = [ "TrackingSurfaceConfiguration.vp1" ]
#job += VP1Alg()

# You can set the global output level on the message svc (not
# recommended) or by using the -l athena CLI parameter

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax = 1000

# add the validation output stream
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "val DATAFILE='DigitizationModuleTest.root' TYPE='ROOT' OPT='RECREATE'" ]


if False :
    # --- load AuditorSvc
    from AthenaCommon.ConfigurableDb import getConfigurable
    # --- write out summary of the memory usage
    #   | number of events to be skip to detect memory leak
    #   | 20 is default. May need to be made larger for complete jobs.
    ServiceMgr.AuditorSvc += getConfigurable('ChronoAuditor')()
    theApp.AuditAlgorithms = True
    theApp.AuditServices   = True
    
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doMonitoring             = True
    jobproperties.PerfMonFlags.doDetailedMonitoring     = True

    from PerfMonComps.JobOptCfg import PerfMonSvc
    ServiceMgr += PerfMonSvc()


#==============================================================
#
# End of job options file
#
###############################################################
