#!/bib/env/python
# TileSynchronizeOnlBchWithOfl
# lukas.pribyl@cern.ch March 2010
# In this script the IgnoredInHlt flags are set <=> the isBad=true for a run number = runOfl
# Note that the sqlite file must contain both ONL and OFL folder

import os,sys

if len(sys.argv)<2:
    print "Please, provide run number as a parameter"
    sys.exit(0)
else:
    run = int(sys.argv[1])
    if run<1:
        print "Please, provide valid run number as a parameter"
        sys.exit(0)

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobPython import TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("writeBch")
import logging
log.setLevel(logging.DEBUG)

#===================================================================
#====================== FILL DB BELOW ==============================
#===================================================================

#--- run number and folder to synchronize to
from PyCool import cool  
runOfl = cool.Int32Max
folder = "/TILE/OFL02/STATUS/ADC"
folderTag = TileCalibUtils.getFullTag(folder, "UPD4-08")

#=== get offlina isBad channels 
db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')

#--- create ofline bad channel manager
mgrOfl = TileBchTools.TileBchMgr()
mgrOfl.setLogLvl(logging.DEBUG)
log.info("Initializing with offline bad channels at tag=%s and time=%s" % (folderTag,(runOfl,0)))
mgrOfl.initialize(db, folder, folderTag, (runOfl,0))

#--- create online bad channel manager
runOnl = run
folderOnl = "/TILE/ONL01/STATUS/ADC"
folderTagOnl = ""
mgrOnl = TileBchTools.TileBchMgr()
mgrOnl.setLogLvl(logging.DEBUG)
mgrOnl.initialize(db, folderOnl, folderTagOnl, (runOnl,0))

#=== print online channel status
log.info("============================================================== ")
log.info("online channel status BEFORE update")
mgrOnl.listBadAdcs()

#=== synchronize
for ros in xrange(1,5):
    for mod in xrange(0,64):
        for chn in xrange(0, 48):
            statlo = mgrOfl.getAdcStatus(ros,mod,chn,0)
            stathi = mgrOfl.getAdcStatus(ros,mod,chn,1)

            #--- add IgnoreInHlt if either of the ADCs has isBad
            #--- add OnlineGeneralMaskAdc if the ADCs has isBad            
            if statlo.isBad() and stathi.isBad():
                mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
            elif statlo.isBad():
                mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
            elif stathi.isBad():
                mgrOnl.addAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                mgrOnl.addAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)
            else:
                #--- delete IgnoreInHlt and OnlineGeneralMaskAdc if both ADCs are not Bad
                mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.IgnoredInHlt)
                mgrOnl.delAdcProblem(ros, mod, chn, 0, TileBchPrbs.OnlineGeneralMaskAdc)
                mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.IgnoredInHlt)
                mgrOnl.delAdcProblem(ros, mod, chn, 1, TileBchPrbs.OnlineGeneralMaskAdc)


#=== print online channel status
log.info("online channel status AFTER update")
mgrOnl.listBadAdcs()

#=== commit changes
mgrOnl.commitToDb(db, folderOnl, folderTagOnl, TileBchDecoder.BitPat_onl01, "tilebeam", "synchronizing with offline bad channels", (runOnl,0))

#=== close DBs
db.closeDatabase()

