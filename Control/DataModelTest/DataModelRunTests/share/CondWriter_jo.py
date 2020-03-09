#
# $Id$
#
# File: share/CondWriter_jo.py
# Author: snyder@bnl.gov
# Date: Jul 2017
# Purpose: Write some conditions objects for testing purposes.
#

## basic job configuration (for generator)
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp


#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

include ('DataModelRunTests/loadWriteDicts.py')


#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = 30

#
# For purposes of this test, we assume that timestamp (in sec) matches
# the event number (starting with 0) and that LBN counts every 3 events.
#
# We write four folders:
# /DMTest/TestAttrList (runlbn):
#  Attribute list.  New IOV for every LBN.  xint=(lbn+1)*10
# /DMTest/S2 (runlbn):
#  DMTest::S2.  New IOV for every 2 LBNs.  payload: lbn*50
# /DMTest/RLTest (runlbn):
#  Attribute list, defined as below.
# /DMTest/TSTest (timestamp):
#  Attribute list, defined as below.

#  lbn:     0..1..2..3..4..5..6..7..8..9..
#
# lbn iov:  1..2.....3..4........5..6..7..
#  ts iov:  1..2..34..5......6.7..8...9...   * 100
#
# event:              11111111112222222222
# (ts)      012345678901234567890123456789

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------


from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
condstream = AthenaOutputStreamTool ('CondStream',
                                     OutputFile = 'condtest.pool.root')

from DataModelTestDataCommon.DataModelTestDataCommonConf import \
     DMTest__CondWriterAlg
topSequence += DMTest__CondWriterAlg (Streamer = condstream)


# Configure conditions DB output to local sqlite file.
try:
    os.remove('condtest.db')
except OSError:
    pass
try:
    os.remove('condtest.pool.root')
except OSError:
    pass
import IOVDbSvc.IOVDb
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=condtest.db;dbname=OFLP200"


# Avoid races when running tests in parallel.
FILECATALOG = 'CondWriter_catalog.xml'

include ('DataModelRunTests/commonTrailer.py')
