#!/usr/bin/env python

# @file mp_genevt_test.py
# @purpose: simple file to create a few ttbar events and read them back
#           with athena-mp

input_file_name = 'mc.event.pool'
output_file_name= 'reaccessed.mc.event.pool'

import PyUtils.AthFile as af
af.server.flush_cache()

import os
import AthenaCommon.ChapPy as accp
app = accp.AthenaApp()
app << """
EVTMAX=1000
OUTPUT='%(input_file_name)s'
""" % globals()
app.include('McParticleTests/iotest_WriteGenEvent_jobOptions.py')

print "=== create an EVGEN file..."
rc = app.run(stdout=os.devnull)
if rc:
    raise RuntimeError(rc)
print "=== create an EVGEN file... [ok]"


app = accp.AthenaApp(cmdlineargs=['--nprocs=-1'])
app << """
EVTMAX=1000
INPUT=['%(input_file_name)s']
OUTPUT='%(output_file_name)s'
""" % globals()

app.include('McParticleTests/iotest_ReadGenEvent_jobOptions.py')

mp_logfile = open('mp.readback.logfile.txt', 'w+')
print "=== read the EVGEN file back (with athena-mp)... (logfile=%s)" % (mp_logfile.name,)
rc = app.run(stdout=mp_logfile)
if rc:
    raise RuntimeError(rc)
print "=== read the EVGEN file back (with athena-mp)... [ok]"

print ":"*80
print "::: results:"
input_file = af.fopen(input_file_name).infos
print "input_file: [%s]\n nentries: %s" % (input_file['file_name'],
                                           input_file['nentries'],)

output_file = af.fopen('reaccessed.mc.event.pool').infos
print "output_file: [%s]\n nentries: %s" % (output_file['file_name'],
                                            output_file['nentries'],)
print "::: bye."
print ":"*80
