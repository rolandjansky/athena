#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Needs to be updated when there is a new L1 menu
lvl1XML='/afs/cern.ch/atlas/software/builds/AtlasP1HLT/17.1.5.3/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v4.xml'

ignoreL1Items=[ ]
#Ignore trivial events
#ignoreL1Items=['L1_MBTS_1', 'L1_TE20']
ignoreL1Items=['L1_BGRP0', 'L1_BGRP1', 'L1_NIMDIR26', 'L1_JE120', 'L1_BPTX0_0', 'L1_BPTX1_0', 'L1_BPTX0_0_1',
               'L1_BPTX1_0_1','L1_BGRP0', 'L1_BGRP1', 'L1_MBTS_1_BGRP1', 'L1_MBTS_2_BGRP1', 'L1_MBTS_1_1_BGRP1',
               'L1_MBTS_A', 'L1_MBTS_C', 'L1_MBTS_A_EMPTY', 'L1_MBTS_C_EMPTY', 'L1_MBTSA1', 'L1_MBTSA3', 'L1_MBTSA4',
               'L1_MBTSA5', 'L1_MBTSA6', 'L1_MBTSA7', 'L1_MBTSA8', 'L1_MBTSA13', 'L1_MBTSA14', 'L1_MBTSA15', 'L1_MBTSC1',
               'L1_MBTSC5', 'L1_MBTSC7', 'L1_MBTSC9', 'L1_MBTSC10', 'L1_MBTSC15', 'L1_TRT', 'L1_NIM_S9C1C19',
               'L1_MBTS_A_BPTX0', 'L1_MBTS_C_BPTX0', 'L1_MBTS_A_BPTX1', 'L1_MBTS_C_BPTX1', 'L1_MBTS_1_EMPTY',
               'L1_MBTS_1_1_EMPTY', 'L1_MBTS_2_EMPTY', 'L1_BPTX0_BGRP0', 'L1_BPTX1_BGRP0', 'L1_BPTX0_BGRP0_BGRP1',
               'L1_MBTSA0', 'L1_MBTSA2', 'L1_MBTSA9', 'L1_MBTSA10', 'L1_MBTSA11', 'L1_MBTSC2', 'L1_MBTSC3', 'L1_MBTSC4',
               'L1_MBTSC6', 'L1_MBTSC8', 'L1_MBTSC11', 'L1_MBTSC12', 'L1_MBTSC13', 'L1_MBTSC14','L1_MBTSA12','L1_MBTSC0',
               'L1_BCM_MUL_1', 'L1_BCM_MUL_2', 'L1_BCM_MUL_3', 'L1_BCM_MUL_4', 'L1_BCM_MUL_5', 'L1_BCM_MUL_6', 'L1_BCM_HT',
               'L1_BCM_MUL_1_EMPTY', 'L1_BCM_MUL_2_EMPTY', 'L1_BCM_MUL_3_EMPTY', 'L1_BCM_MUL_4_EMPTY', 'L1_BCM_MUL_5_EMPTY', 'L1_BCM_MUL_6_EMPTY', 'L1_BCM_HT_EMPTY',
               'L1_BPTX1_BGRP0_BGRP1', 'L1_RD1_FILLED', 'L1_RD0_FILLED']


import eformat
import sys
import os
import time
if len(sys.argv) < 2:
  print " usage: %s  <file1> [xml]" % sys.argv[0]
  sys.exit(1)

if len(sys.argv)>2:
  lvl1XML=sys.argv[2]

ctpNames={}
file=open(lvl1XML,'r')
if not file:
  print 'failed to open',lvl1XML
  sys.exit(1)

#dirty hack to read ctpid->name mapping
for line in file.readlines():
  if line.find('<TriggerItem')!=-1:
    ctpid=-1
    name='UNKNOWN'
    for item in line.split(' '):
      if item.find('ctpid')==0:
        ctpid=item.split('"')[1]
      if item.find('name')==0:
        name=item.split('"')[1]
    if ctpid==-1 or name=='UNKNOWN':
      print "ERROR, could not decode:",line
    else:
      ctpNames[int(ctpid)]=name

file.close()

input_file = sys.argv[1]

print "Opening file: %s" % ( input_file)

input = eformat.istream([input_file])

def chainsPassed(info):
  chains=[]
  cnt=0;
  for word in info:
    for i in range(32):
      if word&(1<<i):
        chains+=[cnt]
      cnt+=1
  return chains

def L1chainsPassed(info):
  chains=[]
  cnt=0;
  for word in info:
    for i in range(32):
      if word&(1<<i) and not ctpNames[cnt] in ignoreL1Items:
        chains+=[ctpNames[cnt]]
      cnt+=1
  return chains
pasEvt=0
cntEvt=0
output=None
evtList=[]      
for event in input:
#  if cntEvt > 135:
#    break


  L1TBP=L1chainsPassed(event.lvl1_trigger_info()[0:8])
  L1TAP=L1chainsPassed(event.lvl1_trigger_info()[8:16])
  L1TAV=L1chainsPassed(event.lvl1_trigger_info()[16:24])
#  L2PassedChains=chainsPassed(event.lvl2_trigger_info())
#  EFPassedChains=chainsPassed(event.event_filter_info())
  StreamTags=event.stream_tag()
  print 'Event:',cntEvt,'L1ID:',event.lvl1_id(),'BCID:',event.bc_id(),'LB:',event.lumi_block(),'RUN:',event.run_no()
  print 'L1 type: 0x%02x' % event.lvl1_trigger_type()
  print 'L1 Before Prescale:',L1TBP
#  print 'L1 After  Prescale:', L1TAP
  print 'L1 After  Veto:',L1TAV
  print 'StreamTags:',StreamTags
  print
  nlist=len(L1TAV)
  print 'size of list ',
  if nlist > 1:
    evtList.append(cntEvt)
    print 'save it in list ' 
    print " ".join(map(str, evtList))
    pasEvt+=1
  if pasEvt > 32:
    break       
  cntEvt+=1
#.join(evtList)
      
print
print
print 'Summary:'
print '',cntEvt,'events analyzed'
print 'passed events ',pasEvt,' in Event List:'
#print evtList 
print " ".join(map(str, evtList))
