#--------------------------------------------------------------
# Full job is a list of algorithms
#--------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

#--------------------------------------------------------------
# Enable monitoring
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doSemiDetailedMonitoring = True

from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

from AthenaCommon.Logging import logging
ftkLog = logging.getLogger('FTKPattGen')


from AthenaCommon.AppMgr import ToolSvc
from TrigFTKBankGen.TrigFTKBankGenConf import *


## uncommet the following lines of DEBUG_BEAMSPOT is selected
#IOVDbSvc = Service("IOVDbSvc")
#IOVDbSvc.GlobalTag="OFLCOND-SDR-BS14T-IBL-CORR-06-02"
#import MagFieldServices.SetupField


#--------------------------------------------------------------
# initialize algo(s)
#--------------------------------------------------------------
PattBankGenRoot = FTKPattGenRootAlgo("FTKPattGenRootAlgo")


#--------------------------------------------------------------
# 
#--------------------------------------------------------------


# Number of events to be processed
#theApp.EvtMax = 1
if hasattr(runArgs,"maxEvents") :
	theApp.EvtMax = runArgs.maxEvents
else :
	theApp.EvtMax = 1


#from GaudiSvc.GaudiSvcConf import THistSvc
from AthenaServices.AthenaServicesConf import AtRanluxGenSvc

ServiceMgr += AtRanluxGenSvc()
rndstream="RANDOM"
seed1=1
seed2=1
if hasattr(runArgs, 'rndStreamName'):
        rndstream=runArgs.rndStreamName

if hasattr(runArgs, 'seed'):
        seed1=runArgs.seed

if hasattr(runArgs, 'seed2'):
        seed2=runArgs.seed2

ServiceMgr.AtRanluxGenSvc.Seeds= [ rndstream+" %d %d" %(seed1,seed2) ]

ServiceMgr.AtRanluxGenSvc.EventReseeding = False

#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["outputHisto DATAFILE='outputHisto.root' OPT='RECREATE'"]

#--------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
#--------------------------------------------------------------

# Output level for PattBankGenAlgo only
PattBankGenRoot.OutputLevel = INFO



#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------

# --- mandatory runArgs
#runArgsFromTrfMandatory = ['fitconstants_path', 'slices_path', 'sectors_path']
runArgsFromTrfMandatory = ['pmap_path', 'rmap_path', 'ssmap_path','curreg','hwmodeid','trials','IBLMode',
			   'fitconsts', 'slices', 'sectors']

# Maps' path
from PyJobTransforms.trfUtils import findFile
import os.path

# pmap
if hasattr(runArgs, 'pmap_path'):
	if os.path.exists(runArgs.pmap_path):
		PattBankGenRoot.pmap_path = runArgs.pmap_path
	else:
		print "Cannot find file: "+runArgs.pmap_path
		raise RuntimeError, "RMAP path seems not to exist. Abort"
else:
	raise RuntimeError, "PMAP was not set. Abort"

# rmap
if hasattr(runArgs, 'rmap_path'):
	if os.path.exists(runArgs.rmap_path):
		PattBankGenRoot.rmap_path = runArgs.rmap_path
	else:
		print "Cannot find file: "+runArgs.rmap_path
		raise RuntimeError, "RMAP path seems not to exist. Abort"
else:
	raise RuntimeError, "RMAP was not set. Abort"

# ssmap
if hasattr(runArgs, 'ssmap_path'):
	if os.path.exists(runArgs.ssmap_path):
		print "ssmap_path " + runArgs.ssmap_path
		PattBankGenRoot.ssmap_path = runArgs.ssmap_path
	else:
		print "Cannot find file: "+runArgs.ssmap_path
		raise RuntimeError, "SSMAP path seems not to exist. Abort."
else:
	raise RuntimeError, "SSMAP was not set. Abort"


# --- Check that the arguments in the mandatory list are set
for mandArg in runArgsFromTrfMandatory:
	print "mandArg " + mandArg
	if not hasattr(runArgs, mandArg):
		raise RuntimeError, 'Failed to find mandatory runtime argument for transform: ' + mandArg
	else:
		attr = getattr(runArgs, mandArg)
		print attr
		setattr(PattBankGenRoot, mandArg, getattr(runArgs, mandArg))


# --- optional arguments
runArgsFromTrfOptional = {'curreg': 32,
#			  'seed': 0,                     #Seed of random generator
                          'rndStreamName': "RANDOM",    #athena name of random stream 
#			  'smear': True, 
			  'hwmodeid': 0,
			  'ModuleLUTPath': "",
			  'IBLMode': 1,
                          'FixEndcapL0' : 0,
			  'pconst_overlap': 2,
			  'trials': 100000,              # Number of trials of the trackInvertion function
 # The following parameters have to be CONSISTENT with sector & constant production
# if min==max, hard-coded defaults are used in class FTKSectorSlice
# For new sector & constant production, the correct parameters will be stored
# in the slice files. In that case the settings below will be ignored
			  'slice_phi_min': 0.,
			  'slice_phi_max': 0.,
			  'slice_c_min': 0.,
			  'slice_c_max': 0.,
			  'slice_d0_min': 0.,
			  'slice_d0_max': 0.,
			  'slice_z0_min': 0.,
			  'slice_z0_max': 0.,
			  'slice_cot_min': 0.,
			  'slice_cot_max': 0.,
# the following parameters specify which slices are used to select sector candidates for a given track
# the "c" slice is disabled because there were some ambiguities with
# its definition for the slices [whether c=1/pt or c=1/(2pt) ],
# potetially leading to inefficiencies
                          'usePhiSlice' : True,
                          'useCSlice' : False,
                          'useD0Slice' : True,
                          'useZ0Slice' : True,
                          'useCotSlice' : True,
# the following parameters choose the track parameter ranges in which patterns are created
# if min==max, tracks are created to cover the range given by the slice file
# it is important to specify the range for c=1/pt because there are ambiguities with the slice definition for this parameter
                          'track_phi_min': 0.,
                          'track_c_min': 0.,
                          'track_d0_min': 0.,
                          'track_z0_min': 0.,
                          'track_eta_min': 0.,
                          'track_phi_max': 0.,
                          'track_c_max': 0.,
                          'track_d0_max': 0.,
                          'track_z0_max': 0.,
                          'track_eta_max': 0.,
# define whether patterns are generated flat in eta or or in cot(theta)
                          'flat_in_cot' : False,
# beamspot, relative to which the patterns are generated
			  'beamspotX': 0.0,
			  'beamspotY': 0.0,
                          'ITkMode': False,
                          'keep7of8': 0,
                          'tolerance7of8': 0.1,

                          'OutputFile': "patterns.root",  # output file (should end with .root)
                          'ModuleGeometryFile': "none",
                          'badModuleFile': "",
                          'sectorSelection' : 1
			  }
skipArgs = []


# --- Set the PattBankGenRoot properties
for runArgName in runArgsFromTrfOptional.keys():
	print 'Setting PattBankGenRoot argument '+runArgName
	if runArgName in skipArgs:
		print 'Skipping ' + runArgName
		continue

	if hasattr(runArgs, runArgName):
		print "if " + runArgName
		setattr(PattBankGenRoot, runArgName, getattr(runArgs, runArgName))
	else:
		print "else " + runArgName
		setattr(PattBankGenRoot, runArgName, runArgsFromTrfOptional[runArgName])


alg += PattBankGenRoot

#==============================================================
# End of job options file
#==============================================================
