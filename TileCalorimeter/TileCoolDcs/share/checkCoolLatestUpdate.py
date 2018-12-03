#!/bin/env python
import time
from TileCoolDcs.TileDCSDataGrabber import TileDCSDataGrabber

dg = TileDCSDataGrabber()


partitions = ["LBA", "LBC","EBA", "EBC"]
variables  = ["FORDAQ_MBHV", "hvOut10"]

#=== ask for one day in the future to be sure
now = int(time.time()) + 60*60*24


print "\n\n   ====    SUMMARY OF LAST FOLDER/CHANNEL UPDATES   ======\n\n"


for var in variables:
    for part in partitions:
        for num in xrange(1,65):
            strNum = ("0"+str(num))[-2:]
            drawer = part+strNum
            folder, chanNum = dg.info.get_folder_and_channel(var, drawer)
            iovSince = dg.getEntry(drawer, var, now)[1]
            print folder, drawer," (",chanNum,")", " ---> ", time.ctime(iovSince/dg.unix2cool)," (",iovSince,")"

