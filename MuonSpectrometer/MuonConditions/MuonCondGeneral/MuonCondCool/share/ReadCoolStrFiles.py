# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# ReadCoolStrFiles.py
# demonstrates reading back of string/file data from COOL, data having
# been previously stored (e.g. by WriteStrFiles.py)
# see the mainpage.h doc for more details
#
# define two instances of the ReadWriteCoolStrFile algorithm to read the data

theApp.Dlls+=["DetDescrCondTools","DetDescrCondExample"]
theApp.TopAlg+=["ReadWriteCoolStrFile/RWInstA","ReadWriteCoolStrFile/RWInstB"]

# parameters for first instance - reads from channel 1 in folder /TEST/COOLSTR
RWA=Algorithm("RWInstA")
RWA.Write=False
RWA.Read=True
RWA.Channel=1
RWA.Folder="/TEST/COOLSTR"

# parameters for first instance - reads from channel 2 in folder /TEST/COOLSTR
# also extract data out into a file extracted.dat on first event
RWB=Algorithm("RWInstB")
RWB.Write=False
RWB.Read=True
RWB.Channel=2
RWB.Extract=True
RWB.Rfile="extracted.dat"
RWB.Folder="/TEST/COOLSTR"

# general options to access conditions database
# normally in real jobs this would be set up elsewhere
include("IOVDbSvc/IOVDbSvc_jobOptions.py")
IOVDbSvc=Service("IOVDbSvc")
# specify default connection
IOVDbSvc.dbConnection="impl=cool;techno=sqlite;schema=mysqlfile.db;X:COOLTEST"
IOVDbSvc.Folders+=["/TEST/COOLSTR"]

# alternatively, assume default connection is setup elsewhere, and specify
# connection info specific to this folder
#IOVDbSvc.Folders+=["impl=cool;techno=sqlite;schema=mysqlfile.db;X:COOLTEST</dbConnection> /TEST/COOLSTR"]

# note that in real use, connection would be specified using strings setup
# for example in IOVDbSvc/IOVRecExCommon.py


