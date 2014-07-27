#!/usr/bin/env tdaq_python

# This ROB hitlist was generated automatically by rosconf-from-data.py
# It is a python application you can customize. 
# Consult the eformat wiki page. 
# Current date and time is 2011-03-03 00:02:12.979339

# original filename list
filename = []
import eformat
import sys
import math

## you have to use a correctly generated robhit map
from robhit import *

def roses( opt ):
 counter = 0
 counter_ros = 0
 ROSID=0x7d0000
 m_map={}
 ROBsInROS=12
 if ( opt == 0xffff ):
   ROBsInROS=len(robhit)
 elif ( opt == 0 ):
   ROBsInROS=12
 else :
   ROBsInROS = opt
   
 for rob in robhit:
  if (( rob & 0x00f00000 ) == 0x00400000) : ## select only LAr ROBs
    if ( (counter % ROBsInROS) == 0 ):
      ID=(0x7d0000)
      ID=ID+counter_ros
      ROSID=ID
      m_map[ROSID]=[]
      counter_ros=counter_ros+1
    m_map[ROSID].append(rob)
    counter=counter+1
 return m_map

def print_nice_exeyez(exraw,eyraw,ezraw):
    sigex=(exraw&0x80000000)
    sigey=(eyraw&0x80000000)
    sigez=(ezraw&0x80000000)
    if ( sigex > 0 ) : 
         sigex=-1;
    else : 
         sigex=1;
    if ( sigey > 0 ) : 
         sigey=-1;
    else : 
         sigey=1;
    if ( sigey > 0 ) : 
         sigez=-1;
    else : 
         sigez=1;
    ex=(((exraw)&0x7fffffff)/512)
    ey=(((eyraw)&0x7fffffff)/512)
    ez=(((ezraw)&0x7fffffff)/512)
    ex*=sigex
    ey*=sigey
    ez*=sigez
    print ex,ey,ez

def extract_febwise(feb):
  size_feb=feb[0]
  if ( size_feb > 10 ) : # I can expect to read the full feb header
    size_physics_payload=(feb[3] & 0xff)
    offs_physics_payload=((feb[3] & 0xff0000)>>16)
    ex=0; ey=0; ez=0;
    ns=(feb[7]&0xff)
    radd=int(math.floor((ns+1)/2.))
    if ( size_physics_payload+offs_physics_payload-8 <= size_feb ):
         # offset to ex is -8+offset_physics_payload+8+4+4+radd+64
         ex_position=offs_physics_payload+72+radd
         exraw=feb[ex_position]
         eyraw=feb[ex_position+1]
         ezraw=feb[ex_position+2]
         #print_nice_exeyez(exraw,eyraw,ezraw)
    return (feb[1],exraw,eyraw,ezraw)

def extract_febinfo(rob):
  rod=rob.rod_data()
  DSPmode=0
  #print rob.rod_minor_version(), rob.version(), rob.minor_version(),rob.rod_version()

  # Assume no variable is available
  [feb1_ff,ex1,ey1,ez1]=[0,0,0,0]
  [feb2_ff,ex2,ey2,ez2]=[0,0,0,0]
  total_size=rob.payload_size_word()
  if ( (total_size > 10) and (rod[0]<total_size) ): # there is a header
    size1=rod[0]
    feb1=rod[0:size1]
    feb_id1=feb1[1]
    DSP_firmware1=feb1[-3]
    if ( DSP_firmware1 > 0 ) :
          DSPmode=DSP_firmware1
    #print hex(DSP_firmware1)
    feb_id2=0
    size2=0
    
    if ( (size1+7+10<=total_size ) and size1+7+rod[size1+7]<=total_size  ):
      feb2=rod[size1+7:]
      size2=feb2[0]
      feb_id2=feb2[1]
      DSP_firmware2=feb2[-3]
      if ( DSP_firmware2 > 0 ) : 
            DSPmode=DSP_firmware2
      #print hex(DSP_firmware2 )
    if ( size1 > 0 ):
      [feb1_ff,ex1,ey1,ez1]=extract_febwise(feb1)
      #print 'feb1_ff,ex1,ey1,ez1=',hex(feb1_ff),hex(ex1),hex(ey1),hex(ez1)
    if ( size2 > 0 ):
      [feb2_ff,ex2,ey2,ez2]=extract_febwise(feb2)
      #print 'feb2_ff,ex2,ey2,ez2=',hex(feb2_ff),hex(ex2),hex(ey2),hex(ez2)
    
    return [ feb1_ff,ex1,ey1,ez1,feb2_ff,ex2,ey2,ez2,DSPmode]

def print_roses(m_map):
  import sys
  for k,v in m_map.iteritems():
    print "ROS ID=",hex(k)
    for value in v:
       sys.stdout.write(hex(value) )
       sys.stdout.write(' ')
    print ''

import logging
logging.info('ROB hit list (extracted from %d file/s) contains %d unique hits' % (len(filename), len(robhit)))

def form_rod():
  m=roses(0)
  print_roses(m)
  counter = 1
  #istr = eformat.istream('data10_7TeV.00167661.physics_Egamma.daq.RAW._lb0000._SFO-1._0001.data')
  #istr = eformat.istream('data11_cos.00178514.physics_CosmicCalo.daq.RAW._lb0026._SFO-5._0001.data')
  #istr = eformat.istream('data11_7TeV.00177682.physics_EnhancedBias.daq.RAW._lb0550._SFO-4._0001.data')
  istr = eformat.istream([
'data11_7TeV.00177682.physics_Egamma.daq.RAW._lb0566._SFO-11._0001.data',
])
  output = eformat.ostream()
  for e in istr:
    # take one rob to help build virtual robs
    newevent= eformat.write.FullEventFragment(e)
    
    for ros_id,list_robs_ids in m.iteritems():
      if True:
        payload=[0]
        DSPmode=0
        count_rob=0
        list_robs = e.children()
        arob = eformat.write.ROBFragment()
        found_any=False
        for rob in list_robs:
          if rob.source_id() in list_robs_ids:
            arob=rob
            found_any=True
            source_id = int(rob.source_id())
            [feb1_ff,ex1,ey1,ez1,feb2_ff,ex2,ey2,ez2,DSPfirmware] = extract_febinfo(rob)
            if ( DSPfirmware > 0 ) : DSPmode=DSPfirmware
            sume1=ex1+ey1;
            sume2=ex2+ey2;
            payload.append(feb1_ff)
            payload.append(ex1)
            payload.append(ey1)
            payload.append(ez1)
            payload.append(sume1)
            payload.append(feb2_ff)
            payload.append(ex2)
            payload.append(ey2)
            payload.append(ez2)
            payload.append(sume2)
            count_rob=count_rob+1
        payload[0] = (count_rob | 0xa<<16)
        if found_any:
           newrob=eformat.write.ROBFragment(arob)
           newrob.source_id(eformat.helper.SourceIdentifier(ros_id) )
           newrob.minor_version(12)
           newrob.rod_minor_version(12)

           newrob.rod_data(payload)
           newrob.status([])
           newevent.append(newrob)
    output.write(newevent)

    print 'show counter = ', counter
    if ( counter == -1 ):
         break
    counter = counter+1

