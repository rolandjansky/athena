# steering file for BS->ESD step
# see myTopOptions.py for more info
#rec.doCBNT=False


#if not 'BSRDOInput' in dir():
jp.AthenaCommonFlags.BSRDOInput=["data_test.00195847.Single_Stream.daq.RAW._lb0016._Athena._0001.data"]
#BSRDOInput=["._0001.data"]




# if not 'rec.doWriteESD' in dir():
rec.doWriteESD=True

rec.doWriteRDO=False
rec.doWriteAOD=False 
rec.doAOD=False 
rec.doWriteTAG=False 

#rec.doTrigger=False
#-------
# Read from ByteStream
#------
from AthenaCommon.GlobalFlags import globalflags
globalflags.InputFormat="bytestream"
# globalflags.ConditionsTag="OFLCOND-CSC-00-01-00"


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

