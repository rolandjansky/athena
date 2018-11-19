from ispy import *
import time

partitionUp=False
while not partitionUp:
    try:
        p = IPCPartition(partitionName)
        runparams = ISObject(p, "RunParams.RunParams", "RunParams")
        runparams.checkout()
        partitionUp=True

    except Exception, e:
        print "%s partition is not up, sleeping for 30 seconds" % partitionName
        time.sleep(30)

