# CscDataBuilder.py
#
# David Adams
# April 2007
#
# Job options to provide access to CSC data. User provides the type
# of the input data stream and the desired data type and this script
# loads the required algorithms and converters so that requested data
# will be present in storegate.
#
# Following variables are used:
#   requestDataType - Type for the data to be loaded 
#     digit - CscDigit, old-style digits
#     strip - CscStripPrepData, prep data digit
#   inputStreamType - Type for the input data file.
#     HITS - GEANT hits
#     RAW - bytestream
#     RDO - raw data objects
#
# The variable cscEventSelector is set to point to an event selector or
# bytestream input service with a single test file.
#
########################################################################

# Exception class.


from AthenaCommon.AppMgr import AlgSequence
topSequence = AlgSequence()

class CscDataBuilderError(Exception):
  def __init__(self,value):
    self.value = value
  def __str__(self):
    return repr(self.value)

#--------------------------------------------------------------
# Check requested data type.
#--------------------------------------------------------------

if not "requestDataType" in vars():
  requestDataType = "BAD"

if requestDataType == "digit":
  pass
elif requestDataType == "strip":
  pass
else:
  msg = "Invalid request data type: " + str(requestDataType)
  raise CscDataBuilderError(msg)
print "CscDataBuilder.py: Requested data type: " + requestDataType

#--------------------------------------------------------------
# Check input stream type.
#--------------------------------------------------------------

if not "inputStreamType" in vars():
  inputStreamType = "BAD"

if inputStreamType == "RAW":
  pass
elif inputStreamType == "RDO":
  pass
elif inputStreamType == "HITS":
  pass
else:
  msg = "Invalid input stream type: " + str(inputStreamType)
  raise CscDataBuilderError(msg)
print "CscDataBuilder.py: Input stream type: " + inputStreamType

#--------------------------------------------------------------
# Convert HITS to digits.
#--------------------------------------------------------------

if inputStreamType == "HITS":
  include("CSC_Digitization/CSC_Digitization_jobOptions.py")

#--------------------------------------------------------------
# Convert digits to RDO.
#--------------------------------------------------------------

if inputStreamType == "HITS" and requestDataType == "strip":
  include("MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py")

#--------------------------------------------------------------
# Convert bytestream to RDO.
#--------------------------------------------------------------

if inputStreamType == "RAW":
  pass

#--------------------------------------------------------------
# Unpack digits.
#--------------------------------------------------------------

# CSC PrepData digits.
if requestDataType == "strip":
  include("MuonRdoToPrepData/CscRdoToCscPrepData_jobOptions.py")

#--------------------------------------------------------------
# Converters to enable input.
#--------------------------------------------------------------

# the POOL converters
#include("MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Define the input data stream.
#--------------------------------------------------------------

# Event selector
if inputStreamType == "HITS" or inputStreamType == "RDO":
  include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import EventSelectorAthenaPool
  cscEventSelector = EventSelectorAthenaPool( "EventSelector" )
elif inputStreamType == "RAW":
  include("ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py")
  include("MuonByteStream/ReadCscRDO_jobOptions.py")
  svcMgr = theApp.serviceMgr()
  from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamEventStorageInputSvc
  svcMgr += ByteStreamEventStorageInputSvc("ByteStreamInputSvc")
  cscEventSelector = svcMgr.EventSelector
  from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
  svcMgr += ByteStreamCnvSvc()
  ByteStreamCnvSvc = svcMgr.ByteStreamCnvSvc

if inputStreamType == "HITS":
  cscEventSelector.InputCollections = [
    "/afs/usatlas.bnl.gov/users/dladams/data/calib0.005145.PythiaZmumu.simul.HITS.v12003101_tid003432._00001.pool.root.1"
  ]
elif inputStreamType == "RDO":
  cscEventSelector.InputCollections = [
    "/afs/usatlas.bnl.gov/users/dladams/data/calib0.005145.PythiaZmumu.digit.RDO.v12003101_tid003432._00001.pool.root.1"
  ]
elif inputStreamType == "RAW":
#  cscEventSelector.Input=["/afs/usatlas.bnl.gov/users/dladams/data/daq_CSC-EB-RCD__0001190_file01.data"]
  cscEventSelector.Input=["rfio:/castor/cern.ch/atlas/muon/CSC/daq_CSC-EB-RCD__0001215_file01.data"]
