from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.Constants import *

# Choices for alignmentJob:
# 
#   'L3Mdt'
#   'L1_MSID'
#   'L3CSC'

# the following defaults are set if global variables not already set
try:
  alignmentJob
except:
  alignmentJob  = "L3Mdt"

try:
  doEndcapOnly
except:
  doEndcapOnly  = True

try:
  doBarrelOnly
except:
  doBarrelOnly  = False

try:
  readEventList
except:
  readEventList = False

try:
  inputFile
except:
  inputFile=['/tmp/roberth/CSC.20100624.singlepart_mu5GeV.50K.RDO.pool.root']

try:
  maxEvents
except:
  maxEvents=-1

try:
  skipEvents
except:
  skipEvents=0

try:
  preIncludes 
except:
  preIncludes=[]

try:
  tag
except:
  tag=""

try:
  commissioning
except:
  commissioning=False

###############################################
#from AthenaCommon.GlobalFlags import GlobalFlags
#GlobalFlags.DataSource.set_data()
#GlobalFlags.DetGeo.set_atlas()

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs

rec.Commissioning.set_Value_and_Lock(commissioning)

rec.doCBNT.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doNameAuditor.set_Value_and_Lock(False)

recAlgs.doMuGirl.set_Value_and_Lock(False)

rec.doDumpProperties=True
rec.doTrigger       =False

rec.doAOD           =True

from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

runArgs.preExec = []

# Maximum number of events to process
runArgs.skipEvents = skipEvents
runArgs.maxEvents  = maxEvents 

# Input file that contains ESD's
if tag=="":
  runArgs.inputRDOFile = inputFile

runArgs.outputLevel = DEBUG

# Output file that contains AOD's
# runArgs.outputAODFile = '/tmp/roberth/AOD.pool.root'

# List of strings separated by commas
runArgs.autoConfiguration = ['everything']

runArgs.preInclude=[]

# jobOption to set alignment flags
if alignmentJob=="L1_MSID":
  runArgs.preInclude.append('MuonAlignExample/L1_MSID_Flags.py')

elif alignmentJob=="L23Mdt":
  runArgs.preInclude.append('MuonAlignExample/L23MdtFlags.py')
  runArgs.preInclude.append('MuonAlignExample/MuonStandalone.py')

elif alignmentJob=="L3Mdt":
  runArgs.preInclude.append('MuonAlignExample/L3MdtFlags.py')
#  runArgs.preInclude.append('MuonStandalone.py')

elif alignmentJob=="L3CSC":
  runArgs.preInclude.append('MuonAlignExample/L3CSCFlags.py')

elif alignmentJob=="L3MdtToroidOff":
  runArgs.preInclude.append('MuonAlignExample/L3MdtToroidOffFlags.py')

else:
  print "wrong alignmentJob!!!!!!!"
  sys.exit(3)

if doEndcapOnly:
  runArgs.preInclude.append('MuonAlignExample/doEndcapOnly.py')

if doBarrelOnly:
  runArgs.preInclude.append('MuonAlignExample/doBarrelOnly.py')

# option to read event list
if readEventList:
  runArgs.preInclude.append('MuonAlignExample/SelectEventNumber.py')

if preIncludes!=[]:
  for preInclude in preIncludes:
    runArgs.preInclude.append(preInclude)

runArgs.postInclude=['MuonAlignExample/MuonAlignment.py']

excSvc=Service("ExceptionSvc")
excSvc.Catch='NONE'
excSvc.Errors='ALL'
print excSvc

include("RecJobTransforms/skeleton.RAWtoESD.py")
print excSvc
