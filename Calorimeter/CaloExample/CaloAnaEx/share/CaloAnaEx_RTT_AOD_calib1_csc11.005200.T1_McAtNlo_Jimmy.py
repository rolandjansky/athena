#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/RecExCommon_flags.py for more available flags
#

# readG3 and AllAlgs needs be set before the include, since several
# secondary flags are configured according to that one
#
# readG3=True # true if read g3 data
# AllAlgs = False # if false, all algorithms are switched off by defaults 
# doiPatRec = False
import os
print 'get_files checkFile.py'
os.system('get_files checkFile.py')


doWriteESD=False
readESD=True
doWriteTAG=False
doWriteAOD=True
noESDTrigger=True

doTrigger=False
doAODLVL1=False

doMissingET=False

# number of event to process
EvtMax=-1
#EvtMax=2


include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
#DetFlags.ID_setOff()
#DetFlags.Muon_setOff()

#DetDescrVersion="ATLAS-CSC-01-02-00"
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-CSC-01-02-00')


# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

# hack starts here:

class DummyEventSelector   :
    InputCollections= []

class DummyServiceMgr  :
    EventSelector= DummyEventSelector

#create a dummy ServiceMgr
ServiceMgr=DummyServiceMgr()

#hack ends here
