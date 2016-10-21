# steering file for BS->ESD step
# see myTopOptions.py for more info
#doCBNT=False
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import glob

if not ('EvtMax' in dir()):
   acf.EvtMax=10


if not 'BSRDOInput' in dir():
#    BSRDOInput=["../AthenaModernRDOtoBS/daq.csc13.0000000.Single.Stream.LB0000.Athena._0001.data"]
#     BSRDOInput=["rfio:/castor/cern.ch/user/s/strom/test/daq.ttbar.data"]  
#      BSRDOInput=["../testAllPT/daq..0000000.Single.Stream.LB0000.Athena._0001.data"]
      acf.BSRDOInput=["../testAllPT/EF._0001.data"]

if len(BSRDOInput) > 1:
  print "WARNNING this JO only works on one file, ignore extras",BSRDOInput
  acf.BSRDOInput = BSRDOInput[0:0]
#
# try looking for the data file in the expected directory
#
if  glob.glob(BSRDOInput[0]):
  print "INFO:  found file",BSRDOInput
#
# try using the writing file if this exists
#
elif glob.glob(BSRDOInput[0]+".writing"):
  BSRDOInput[0] = BSRDOInput[0]+".writing"
  print "using file ",BSRDOInput[0]
#
# maybe the name changed
#
elif glob.glob( BSRDOInput[0][:BSRDOInput[0].rfind('/')]+"/*.data"): 
  print "looking for ",BSRDOInput[0][:BSRDOInput[0].rfind('/')]+"/*.data"
  acf.BSRDOInput = glob.glob( BSRDOInput[0][:BSRDOInput[0].rfind('/')]+"/*.data")
  print "WARNING: trying file: ",BSRDOInput
#
# failing that, use a fixed file
#
else:
  BSRDOInput[0] = "/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/data"+BSRDOInput[0][2:]
  print "WARNING: trying file: ",BSRDOInput

if not 'doWriteESD' in dir():
  rec.doWriteESD=True

rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False
rec.doESD=False
rec.doWriteTAG=False 

rec.doCBNT=False

doTrigger=True

#doTrigger=False
#-------
# Read from ByteStream
#------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.InputFormat.set_bytestream()

readBS=True

#very temporary hack -- should get this automatically 
#from TriggerJobOpts.TriggerFlags import TriggerFlags
#TriggerFlags.readLVL1configFromXML=True
#TriggerFlags.inputLVL1configFile="TriggerRelease/Rel14_1_0rel2_May27_lumi1E31_outputLVL1config.xml"


include ("RecExCommon/RecExCommon_flags.py")
# should be done afterwards so that TriggerFlags are configured ok

# has been run at RDO->BS step (even EF ?)
# doTrigger=False
#from RecExConfig.RecFlags import recAlgs
# recAlgs.doTrigger=False



# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# the correct tag should be specified 
# use "OFLCOND-SIM-00-00-03" for ATLAS-GEO-XX
#
ServiceMgr.IOVDbSvc.GlobalTag="OFLCOND-SIM-00-00-03"
#ServiceMgr.IOVDbSvc.GlobalTag="OFLCOND-CSC-00-01-00"

#the file from /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/references/data 
#corresponds to MC cabling of SCT
from TriggerRelease.Modifiers import ForceMCSCTMapping
ForceMCSCTMapping().postSetup()
