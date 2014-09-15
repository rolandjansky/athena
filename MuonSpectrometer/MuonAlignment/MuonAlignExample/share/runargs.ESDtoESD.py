from MuonRecExample.MuonRecFlags import muonRecFlags
from AthenaCommon.Constants import *

# Choices for alignmentJob:
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
  inputFile=['/tmp/roberth/data11_7TeV.00191933.physics_Muons.merge.DESD_SGLMU.f415_m1026/data11_7TeV.00191933.physics_Muons.merge.DESD_SGLMU.f415_m1026._lb0205._0001.1']

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

  
from RecExConfig.RecFlags     import rec
from RecExConfig.RecAlgsFlags import recAlgs

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
if tag!="":
  athenaCommonFlags.PoolInputQuery.set_Value_and_Lock(tag)
  rec.readTAG.set_Value_and_Lock(True)
  athenaCommonFlags.FilesInput = inputFile

if inputFile[0].find('TMU15GEV.20101116')>-1:

  MagFieldAthenaSvc = Service( "MagFieldAthenaSvc" );
  svcMgr += MagFieldAthenaSvc
  MagFieldAthenaSvc.NameOfTheSource = "BYPYTH"
  MagFieldAthenaSvc.noTiltNoShift = 0
  #MagFieldAthenaSvc.NameOfTheBfieldFile = "/afs/cern.ch/atlas/project/magfield/Map/AtlasFull/bmagatlas_09_fullAsym20400.data.bz2" 
  MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas_09_fullAsym20400.data.bz2"

  MagFieldAthenaSvc.SolenoidField = "map7730bes2.grid"
  MagFieldAthenaSvc.dxsol     = 0.0
  MagFieldAthenaSvc.dysol     = 1.6
  MagFieldAthenaSvc.dzsol     = 0.0
  MagFieldAthenaSvc.dthetasol = 0.0
  MagFieldAthenaSvc.dphisol   = 0.0
  MagFieldAthenaSvc.dpsisol   = 0.0


rec.doESD.unlock()
rec.doESD.set_Value_and_Lock(False)

rec.Commissioning.set_Value_and_Lock(commissioning)

rec.doCBNT.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(True)
rec.doMonitoring.set_Value_and_Lock(False)
rec.doNameAuditor.set_Value_and_Lock(False)

recAlgs.doMuGirl.set_Value_and_Lock(False)

rec.doDumpProperties=True
rec.doTrigger       =False
rec.doAOD           =True

rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)

#rec.doWriteAOD      =False
#rec.doCalo          =False

#rec.OutputLevel.set_Value_and_Lock(VERBOSE)

#rec.doAODCaloCells.set_Value_and_Lock(False)
#rec.doAODall.set_Value_and_Lock(False)
#rec.doEgamma=False
#from JetRec.JetRecFlags import jetFlags
#jetFlags.Enabled=False

#from AthenaCommon.DetFlags import DetFlags
#DetFlags.Muon_setOn() 
#if alignmentJob=="L1_MSID":
#  DetFlags.ID_setOn()
#if alignmentJob!="L3CSC":
#  DetFlags.CSC_setOff()
#  muonRecFlags.doCSCs.set_Value_and_Lock(False)
#else:
#  DetFlags.CSC_setOn()
#  muonRecFlags.doCSCs.set_Value_and_Lock(True)

#print "!!!!!!!!!!!!!!!!!!!!!!! (check for ID on/off) DetFlags: "
#DetFlags.Print()

from PyJobTransformsCore.runargs import RunArguments

runArgs = RunArguments()

runArgs.preExec = []

# Maximum number of events to process
runArgs.skipEvents = skipEvents
runArgs.maxEvents  = maxEvents 

# Input file that contains ESD's
if tag=="":
  runArgs.inputESDFile = inputFile

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

elif alignmentJob=="L3Mdt_muidSA":
  runArgs.preInclude.append('MuonAlignExample/L3MdtFlags_muidSA.py')

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

include("RecJobTransforms/skeleton.ESDtoESD.py")
#include("PATJobTransforms/skeleton.MergePool.py") 

