#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# detStatus_nameAll.py <onlpwd> <oflpwd>
# execute detStatus_NameChannels.py for all production DB folders in Oracle
# please do not run this script

import os,sys

if len(sys.argv)!=3:
    print "Usage",sys.argv[0]," <global_onl_pwd> <global_ofl_pwd>"
    sys.exit(-1)
pwd=[sys.argv[1],sys.argv[2]]
print pwd

# entries specified as (X,Y,leaffolder)
# X=0 for online, 1 for offline
# Y=0 for OFLP200, 1 for COMP200

todo=[(1,0,'LBSUMM'),
      (1,0,'DQCALCOFL'),
      (1,0,'DQMFOFL'),
      (1,0,'SHIFTOFL'),
      (0,1,'DQMFONL'),
      (0,1,'SHIFTONL'),
      (0,1,'DQMFONLLB'),
      (1,1,'DCSOFL'),
      (1,1,'DQCALCOFL'),
      (1,1,'DQMFOFL'),
      (1,1,'LBSUMM'),
      (1,1,'SHIFTOFL')
      ]
      
ndone=0
nbad=0
for (iofl,icom,folder) in todo:
    server=['ATONR_GPN','ATLAS_COOLWRITE'][iofl]
    schema=['ATLAS_COOLONL_GLOBAL','ATLAS_COOLOFL_GLOBAL'][iofl]
    dbname=['OFLP200','COMP200'][icom]
    user=schema+'_W'
    password=pwd[iofl]
    chanlist='131 134 135 141 144 145'
    comm='./detStatus_NameChannels.py "oracle://%s;schema=%s;dbname=%s;user=%s;password=%s" /GLOBAL/DETSTATUS/%s' % (server,schema,dbname,user,password,folder)
    # comm='~/w0/cool/pytools/cool_dropChannel.py %s %s %s %s /GLOBAL/DETSTATUS/%s %s' % (server,schema,password,dbname,folder,chanlist)
    #print comm
    rc=os.system(comm)
    # rc=0
    if (rc==0):
        ndone+=1
    else:
        nbad=+1

print "Loop done: %i folders updated, %i bad" % (ndone,nbad)
