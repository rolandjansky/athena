import sys
import os
import commands
import subprocess

from AthenaCommon.Logging import logging
recoLog = logging.getLogger('merge_hist')

recoLog.info( '****************** STARTING HIST MERGING *****************' )

#==============================================================
# Job Configuration parameters:
#==============================================================
from RecJobTransforms.RecConfig import recConfig
for cf in runArgs.jobConfig:
    include(cf)
recoLog.info( '**** Transformation configuration arguments' )
recoLog.info( str(recConfig) )

#==============================================================
# Job definition parameters:
#==============================================================

#DetDescrVersion = runArgs.geometryVersion
theApp.EvtMax = 0
inputFileName = runArgs.inputROOTFile
outFileName=runArgs.outputROOTFile

runPostProcessing = False

filename = "inputhists.list"
file = open(filename,'w')
list = ''
for infile in inputFileName:
  file.write(infile + "\n")
  list += infile+' '
file.close()

testname = "outputhists.list"
file = open(testname,'w')    
file.write(outFileName + "\n")             
file.close()                            

recoLog.info( inputFileName )
recoLog.info( outFileName )

#==============================================================
# Do the job!
#==============================================================



cmd  = "rm -f hist_merge.log "
recoLog.info( cmd )
(retcode,error) = commands.getstatusoutput(cmd)
newcommand= "DQHistogramMerge.py %s %s %s"%(filename,outFileName,runPostProcessing)
recoLog.info(newcommand)
###################################
## Fix for stray processes. Run DQHistogramMerge using subprocess module with stdout and stderr
## piped to a file which will cause it to be killed if parent is killed.
## Sami Kama sami.kama@cern.ch
##################################
#writing in a temp file should allow termination eventually even if parent is killed
tmpbuff=os.tmpfile()
dqhistpipe=subprocess.Popen(["DQHistogramMerge.py", filename, outFileName, "%s"%runPostProcessing],
                                stdout=tmpbuff, stderr=tmpbuff, shell=False)
status=dqhistpipe.wait()

print "---------------------------------------------------------------------------------------"
print '## Output of \'' + newcommand + '\':'
#print output
try:
    logFile=open('hist_merge.log','w')
    tmpbuff.seek(0)
    for line in tmpbuff:
        print line,
        logFile.write(line)
finally:
    tmpbuff.close()
    logFile.close()
print '## DQHistogramMerge.py finished with retcode = %s' % (status)
print "---------------------------------------------------------------------------------------"
leavecode = '## DQHistogramMerge.py finished with retcode = %s' % (status)
recoLog.info( leavecode )

newcommand= "DQHistogramMerge.py %s %s False"%(testname,"dummy.root")
recoLog.info(newcommand)
tmpbuff=os.tmpfile()
dqhistpipe=subprocess.Popen(["DQHistogramMerge.py", testname,"dummy.root", "False"],
                                stdout=tmpbuff, stderr=tmpbuff, shell=False)
status=dqhistpipe.wait()

print "---------------------------------------------------------------------------------------"
print '## Output of \'' + newcommand + '\':'
try:
    logFile=open('validate_merge.log','w')
    tmpbuff.seek(0)
    for line in tmpbuff:
        print line,
        logFile.write(line)
finally:
    tmpbuff.close()
    logFile.close()
print '## DQHistogramMerge.py finished with retcode = %s' % (status)
print "---------------------------------------------------------------------------------------"
leavecode = '## DQHistogramMerge.py validate finished with retcode = %s' % (status)
leavecodecmd = "echo \"" + leavecode + "\" >> hist_merge.log"
os.system(leavecodecmd)
newcommand= "ScanHistFile.py %s "%(outFileName)
recoLog.info(newcommand)
tmpbuff=os.tmpfile()
dqhistpipe=subprocess.Popen(["ScanHistFile.py", outFileName],
                            stdout=tmpbuff, stderr=tmpbuff, shell=False)
status=dqhistpipe.wait()
print "---------------------------------------------------------------------------------------"
print '## Output of \'' + newcommand + '\':'
try:
    logFile=open('ScanHistFile.log','w')
    tmpbuff.seek(0)
    for line in tmpbuff:
        print line,
        logFile.write(line)
finally:
    tmpbuff.close()
    logFile.close()
print '## ScanHistFile.py finished with retcode = %s' % (status)
print "---------------------------------------------------------------------------------------"
leavecode = '## ScanHistFile.py validate finished with retcode = %s' % (status)
leavecodecmd = "echo \"" + leavecode + "\" >> hist_merge.log"
os.system(leavecodecmd)
    
    

