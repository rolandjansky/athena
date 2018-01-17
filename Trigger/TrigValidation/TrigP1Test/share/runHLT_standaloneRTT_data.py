# This is just a wrapper job option made for RTT -- will also find
# files and eos and copy them to RTT


from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import os,commands
def getEosDirectoryList(path,lbRange,sfoRange):

    cmd = 'eos -b ls %s/ ' % (path)
    file = []
    pool=os.getenv("WORKDIR")
    if pool == "NONE":
        print "ERROR: Workdir not set returning no files"
        return file
    else:
        print "INFO: storing files here -- ", pool
#._lb0576._SFO-9._0001.1
    for i in commands.getoutput(cmd).split('\n'):
        sfoNumber = i[i.rfind("._SFO-")+6:i.rfind("_SFO-")+6]
        lumiBlock =  i[i.rfind("._lb")+5:i.rfind("._lb")+8]
        print 'lumiBlock',lumiBlock
        print 'sfoNumber',sfoNumber

        if lumiBlock.isdigit() and sfoNumber.isdigit():
            if int(lumiBlock) > lbRange[0]-1 and int(lumiBlock) < lbRange[1]+1 and int(sfoNumber) > sfoRange[0]-1 and int(sfoNumber) <sfoRange[1]+1:  
                (status,output) = commands.getstatusoutput("xrdcp -s root://eosatlas.cern.ch/%s/%s %s/." % (path,i,pool) )
                print i, status
                if status == 0:
                    file += ["%s/%s" % (pool,i) ]
                else:
                    print "ERROR: unable to copy file:",i," status ",status
    return file

if not( 'sfoRange' in dir()):
  sfoRange = [5,5]	 

if not ( 'lbRange' in dir()): 
  lbRange = [9,9]

if not ( 'dsName' in dir()):
  dsName = "/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data12_8TeV.00202798.physics_EnhancedBias.merge.RAW/"

if not ('EvtMax' in dir()):
  acf.EvtMax=2000

#
# only needed when using old enhanced bias files with out correct metadata
#if not ('setDetDescr' in dir()):
#  setDetDescr="ATLAS-GEO-08-00-02"

#if not ('setGlobalTag' in dir()):
#  setGlobalTag="COMCOND-HLTP-002-00"

if not ( 'useCOMCONDDB' in dir()):
  useCOMCONDDB=True

#testInitialBeamV3=True


#
# needs to be BSRDOInput for runHLT_standalone  
BSRDOInput = getEosDirectoryList(dsName,lbRange,sfoRange)



from AthenaCommon.Include import include

include("TriggerRelease/runHLT_standalone.py")

# not sure why this is here.
#if hasattr(jobproperties,"PerfMonFlags"):
   # We don't need this for athenaMT/PT
#   jobproperties.PerfMonFlags.doPersistencyMonitoring = False

#   if hasattr(jobproperties.PerfMonFlags,"doHephaestusMon"):
#      jobproperties.PerfMonFlags.doHephaestusMon = True

   # Minimal performance monitoring
   # To avoid increase in memory usage due to perfmon itself
#   if 'perfmonNoDetail' in dir():
#      jobproperties.PerfMonFlags.doFastMon = True
#      jobproperties.PerfMonFlags.doDetailedMonitoring = False

# use cost monitoring
#from TrigCostMonitor.TrigCostMonitorConfig import setupCostAlg,setupCostJob 
#setupCostAlg()   
#setupCostJob()


