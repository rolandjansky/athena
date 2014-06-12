#!/bin/env python

import os,sys,commands
from PyCool import cool
from CaloCondPhysAlgs.CaloBlobWriter import createSqlite

def usage():
   print "Syntax"
   print sys.argv[0],"<sqlitename> {<textfile> <Run1> <LB1> <Run2> <LB2>}"

if len(sys.argv)<2:
   usage()
   sys.exit(-1)

#parameters:
sqliteName=sys.argv[1]
folderName="/LAR/TimeCorrectionOfl/CellTimeOffset"
foldertag="".join(folderName.split("/")) + "-empty"
iovMin = cool.ValidityKeyMin
iovMax = cool.ValidityKeyMax
filename=None
defaultvalue=0.0

if len(sys.argv)>2:
   filename=sys.argv[2]
   if not os.access(filename,os.R_OK):
      print "ERROR Can't read input file %s" % inputFileName
      sys.exit(-1)
   print "Input text file:",filename 
   pass
   
if len(sys.argv)>4:
   run=int(sys.argv[3])
   LB=int(sys.argv[4])
   iovMin=run <<32 | (LB & 0xFFFFFFFF)
   print "IOV-from run ",run,"LB",LB
   pass

if len(sys.argv)>6:
   run=int(sys.argv[5])
   LB=int(sys.argv[6])
   iovMax=run <<32 | (LB & 0xFFFFFFFF)
   print "IOV-until run ",run,"LB",LB
   pass


createSqlite(sqliteName,folderName,foldertag,iovMin,iovMax,filename,defaultvalue)

print "To upload:"
print "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py",sqliteName,"COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"
