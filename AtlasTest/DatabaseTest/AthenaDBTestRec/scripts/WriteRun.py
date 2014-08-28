#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
import sys,os,traceback,time

if len(sys.argv)<4:
    print "Usage",sys.argv[0],'\"DbIbString\" <run> <config1> <config2>'
    sys.exit(-1)

dbid=sys.argv[1]
run=int(sys.argv[2])
time0=run*3600
starttime=time.time()
print ">== Data will be written to database connection",dbid

for i in range(0,len(sys.argv)-3):
    configfile=sys.argv[3+i]
    print '>== Write data for run',run,'start time',time0,'config file',configfile
    comm='TestCoolRecWriter.exe \"'+dbid+'\" 1 '+configfile+' 1 '+str(run)+' 720000 '+str(time0)+ ' 3600'
    print 'Execute command:',comm
    rc=os.system(comm)
    if (rc!=0):print 'Non-zero return code',rc,'from TestCoolRecWriter'
print '>== All done, time taken is',time.time()-starttime

