#!/usr/bin/python

import os,commands,sys

if(len(sys.argv)>1):
    
    print "looking for EOS files in ",sys.argv[1]
    dsName = sys.argv[1]
    cmd  = 'eos ls %s/ ' % (dsName)
    files = []
    count_files = 0
    print cmd
    for i in commands.getoutput(cmd).split('\n'):
        print i
        if i.find("_0001.1")==-1:
            continue
        fileNumber = count_files
        count_files = count_files + 1
        print 'fileNumber',fileNumber
        files += ['root://eosatlas.cern.ch/%s/%s' % (dsName,i) ]
    print files
    f = open('fileList.sh', 'w')
    f.write('athena.py  -c "jp.AthenaCommonFlags.EvtMax.set_Value_and_Lock(-1);fileList=')
    f.write(str(files))
    f.write('" -b TrigAnalysisTest/testAthenaTrigESD_HLTMonitoring.py')

