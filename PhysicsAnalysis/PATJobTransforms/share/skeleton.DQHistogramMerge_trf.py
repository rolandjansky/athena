###############################################################
#
# Skeleton top job options for DQHistogramMerge_trf
#
#==============================================================

#hack: we are forced to use athena (as a dummy) within the current PyJobTransformsCore
theApp.EvtMax=1

# merge and/or rename monitoring histogram file
# file with list of ROOT files to merge
mergeListFile=open('hist_merge_list.txt','w')
inFiles=runArgs.inputFile
for f in inFiles:
    mergeListFile.write( str(f) + '\n' )    
mergeListFile.close()

#
#
# This is the old method which was causing stray DQHistogramMerge processes.
#
# call DQHistogramMerge
#cmd = 'DQHistogramMerge.py hist_merge_list.txt %s False' % (runArgs.outputHISTFile)
#cmd = 'sleep 500' 
#import commands
#(status, output) = commands.getstatusoutput(cmd)

import os,subprocess
#writing in a temp file should allow termination eventually even if parent is killed
tmpbuff=os.tmpfile()
dqhistpipe=subprocess.Popen(["DQHistogramMerge.py", "hist_merge_list.txt", runArgs.outputHISTFile, "False"],
                            stdout=tmpbuff, stderr=tmpbuff, shell=False)
status=dqhistpipe.wait()

print "---------------------------------------------------------------------------------------"
print '## Output of \'DQHistogramMerge.py hist_merge_list.txt '  + runArgs.outputHISTFile + ' False\':'
try:
    tmpbuff.seek(0)
    for line in tmpbuff:
        print line,
finally:
    tmpbuff.close()
print '## DQHistogramMerge.py finished with retcode = %s' % (status)
print "---------------------------------------------------------------------------------------"

if not status==0:
    raise RuntimeError("DQ HiST merging did NOT work. Stopping!")
