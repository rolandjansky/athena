# steering file for BS->ESD step
# see myTopOptions.py for more info
#doCBNT=False

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from AthenaCommon.Logging import logging 
log = logging.getLogger("testAthenaTrigBStoESD.py:") 

# find input BS file using a glob because the name seems to change a lot
#if not 'BSRDOInput' in dir():
#   #BSRDOInput=["../AthenaTrigRDOtoBS/data_test.00005200.Single_Stream.daq.RAW._lb0000._Athena._0001.data"]
#   import glob
#   pattern="../AthenaTrigRDOtoBS/raw*.data"
#   bsfiles=glob.glob(pattern)
#   if (len(bsfiles)==0):
#      log.fatal(" ERROR FATAL No input BS file could be found matching '" + pattern + "'")
#      exit(-1)
#   elif (len(bsfiles)>1):
#      log.warning("Multiple input BS files found, taking last one: " + bsfiles[-1])
#      print "All matches to pattern '" + pattern + "': ", bsfiles
#      acf.BSRDOInput=[bsfiles[-1]]
#   else:
#      log.info("Using input BS file " + bsfiles[0])
#      acf.BSRDOInput=bsfiles
 

#acf.FilesInput=acf.BSRDOInput()
 




# specifying conditions by hand until auto configure is fixed
# this will break every time the default file changes
#log.warning("setting geometry and coniditions by hand, will break if default RDO file changes")

from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-RUN12-SDR-22')
globalflags.InputFormat='bytestream'

#globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-RUN12-SDR-17")
#globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-21-02-02")



# check if file exists
import os
if not os.path.exists(acf.BSRDOInput()[0]):
   log.fatal(" ERROR FATAL Input file acf.BSRDOInput()[0] does not exist: " + acf.BSRDOInput()[0])
   exit(-2)

exit


#if not 'doWriteESD' in dir():

rec.doWriteESD=True
rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False
rec.doDPD=False
rec.doESD=True
rec.doWriteTAG=False

rec.doTrigger=False


#from AthenaCommon.AppMgr import ServiceMgr
#ServiceMgr.IOVDbSvc.GlobalTag="OFLCOND-DR-BS7T-ANom-11"

include ("RecExCond/RecExCommon_flags.py")
# should be done afterwards so that TriggerFlags are configured ok

# has been run at RDO->BS step (even EF ?)
# doTrigger=False
#from RecExConfig.RecFlags import recAlgs
# recAlgs.doTrigger=False



# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#Don't abort on status code error
StatusCodeSvc.AbortOnError=False
