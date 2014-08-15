#!/usr/bin/env python

# @file mp_basic_test.py
# @purpose: simple file to create a few elephantino events with athena-mp

input_file_name = 'my.data.pool'
output_file_name= 'reaccessed.my.data.pool'

import PyUtils.AthFile as af
af.server.flush_cache()

import os
import AthenaCommon.ChapPy as accp
app = accp.AthenaApp()
app << """
EVTMAX=1000
OUTPUT='%(input_file_name)s'
""" % globals()
app.include('AthExThinning/AthExThinning_makeData.py')

print "=== create an elephantino file..."
rc = app.run(stdout=os.devnull)
if rc:
    raise RuntimeError(rc)
print "=== create an elephantino file... [ok]"


app = accp.AthenaApp(cmdlineargs=['--nprocs=-1'])
app << """
EVTMAX=1000 #-1
INPUT=['%(input_file_name)s']
OUTPUT='%(output_file_name)s'
""" % globals()

app.include('AthExThinning/ReadNonThinnedData_jobOptions.py')

mp_logfile = open('mp.elephantino.readback.logfile.txt', 'w+')
print "=== read the elephantino file back (with athena-mp)... (logfile=%s)" % (mp_logfile.name,)
rc = app.run(stdout=mp_logfile)
if rc:
    raise RuntimeError(rc)
print "=== read the elephantino file back (with athena-mp)... [ok]"

input_file  = af.fopen(input_file_name).infos
output_file = af.fopen(output_file_name).infos

print ":"*80
print "::: results:"

print """\
input_file: [%s]
  nentries: %s""" % (
  input_file['file_name'],
  input_file['nentries'],
  )

print """\
output_file: [%s]
   nentries: %s""" % (
   output_file['file_name'],
   output_file['nentries'],
   )

print "::: bye."
print ":"*80
