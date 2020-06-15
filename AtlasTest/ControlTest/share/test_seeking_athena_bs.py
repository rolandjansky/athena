#!/usr/bin/env python
# @file test_seeking_athena.py
# @purpose tests seeking within a BS file

import os
import re
import difflib

## first, create an input BS file
input_file_list = [ 'my1.data', 'my2.data', 'my3.data' ]
output_file_name= 'reaccessed.my.data'

## import PyUtils.AthFile as af
## af.server.flush_cache()

import os
import AthenaCommon.ChapPy as accp
num_evt = 0
run_num = 0
pos = 44
for input_file_name in input_file_list:
    num_evt += 5
    run_num += 1
    app = accp.AthenaApp()
    app << """
    import AthenaCommon.AtlasUnixGeneratorJob
    theApp.EvtMax = %(num_evt)s
    svcMgr.EventSelector.RunNumber = %(run_num)s
    include( "ByteStreamCnvSvc/RDP_ByteStream_jobOptions.py" )
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.BSRDOOutput = '%(input_file_name)s'
    from ByteStreamCnvSvc import WriteByteStream
    StreamBSFileOutput = WriteByteStream.getStream( "EventStorage", "StreamBSFileOutput" )
    """ % globals()

    print "=== create an elephantino file..."
    rc = app.run(stdout=os.devnull)
    if rc:
        raise RuntimeError(rc)
    print "=== create an elephantino file... [ok]"
    pos += 45

## now do the seeking within a new job.

app = accp.AthenaApp()
app << """
EVTMAX=-1
INPUT=%(input_file_list)s
OUTPUT='%(output_file_name)s'

include('ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py')

svcMgr.EventSelector.Input = %(input_file_list)s

import AthenaServices.PyAthenaEventLoopMgr as aspy
#aspy.enable_seeking()

print '==test-seeking==: theApp.run(1)...'
theApp.run(1) # event 1
print '==test-seeking==: theApp.run(1)... [ok]'

theApp.seek(0)
theApp.nextEvent()

theApp.seek(2)
theApp.nextEvent()

theApp.seek(21)
theApp.nextEvent()

theApp.seek(6)
theApp.nextEvent()

theApp.seek(0)
theApp.nextEvent()

theApp.seekEvent(2)
theApp.seekEvent(21)
theApp.seekEvent(6)

theApp.seek(0)
theApp.nextEvent(-1)

theApp.finalize()
theApp.exit()
"""% globals()

read_logfile = open('seeking.elephantino.readback.logfile.txt', 'w+')
print "=== read the elephantino file back (with seeking)... (logfile=%s)" % (read_logfile.name,)
rc = app.run(stdout=read_logfile)
if rc:
    raise RuntimeError(rc)
print "=== read the elephantino file back (with seeking)... [ok]"

read_logfile.seek(0)

out_for_diff = [
    l.strip() for l in read_logfile
    if re.match('^AthenaEventLoopMgr   INFO   ===>>>  done processing event',
                l)
    ]

ref_file = """\
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #1 1 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #1 2 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #2, run #1 3 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #6, run #3 4 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #1, run #2 5 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #1 6 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #2, run #1 7 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #6, run #3 8 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #1, run #2 9 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #1 10 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #1, run #1 11 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #2, run #1 12 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #3, run #1 13 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #4, run #1 14 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #2 15 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #1, run #2 16 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #2, run #2 17 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #3, run #2 18 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #4, run #2 19 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #5, run #2 20 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #6, run #2 21 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #7, run #2 22 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #8, run #2 23 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #9, run #2 24 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #0, run #3 25 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #1, run #3 26 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #2, run #3 27 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #3, run #3 28 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #4, run #3 29 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #5, run #3 30 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #6, run #3 31 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #7, run #3 32 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #8, run #3 33 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #9, run #3 34 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #10, run #3 35 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #11, run #3 36 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #12, run #3 37 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #13, run #3 38 events processed so far  <<<===
AthenaEventLoopMgr   INFO   ===>>>  done processing event #14, run #3 39 events processed so far  <<<===
""".splitlines()

diff = difflib.unified_diff(out_for_diff, ref_file)
diff = os.linesep.join( [d for d in diff] )
assert diff=='', diff

print "::: bye."
print ":"*80
