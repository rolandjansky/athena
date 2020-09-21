#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function

from AthenaCommon.Logging import logging
tilemon_log = logging.getLogger('TileLasMonState.py')

#########################################
#
# Set db
# 
#########################################

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat.set_Value_and_Lock("bytestream")
globalflags.DetGeo.set_Value_and_Lock("atlas")
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")



#########################################
#
# Example setup for ByteStreamEmonSvc
# 
#########################################

# Common part, copy from here

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

if 'EvtMax' not in dir():
    EvtMax = -1


include("ByteStreamEmonSvc/ByteStreamEmonInputSvc_jobOptions.py")

# #####################################################
# If using the ERSBootstrap.py file, enable the output
# via ERS
# ####################################################
#MessageSvc.useErs = True

# Define the input
ByteStreamEmonInputSvc = svcMgr.ByteStreamInputSvc

# ############################################################
# The name of the partition you want to connect to is taken
# from 'TDAQ_PARTITION' if it exists, otherwise from here.
# ############################################################

if 'Partition' not in dir():
    Partition="ATLAS"
ByteStreamEmonInputSvc.Partition = Partition

from ipc import IPCPartition
from ispy import ISObject
ipc_partition = IPCPartition(Partition);

if not ipc_partition.isValid():
    tilemon_log.error( 'Partition: ' + ipc_partition.name() + ' is not valid' )
    sys.exit(1)

### ATLAS partition: Read Global Run Parameters to configure the jobs
if ByteStreamEmonInputSvc.Partition == 'ATLAS':
    try:
        run_params = ISObject(ipc_partition, 'RunParams.SOR_RunParams', 'RunParams')
    except:
        tilemon_log.warning( "Could not find Run Parameters in IS - Set default beam type to 'cosmics'")
        beamType = 'cosmics'
    else:
        run_params.checkout()
        beam_type = run_params.beam_type
        beam_energy = run_params.beam_energy
        RunNumber = run_params.run_number
        project = run_params.T0_project_tag
        tilemon_log.info( "RUN CONFIGURATION: beam type: %i, beam energy: %i, run number: %i, project tag: %s" % (beam_type, beam_energy, RunNumber, project) )
        
        # define beam type based on project tag name
        if project[7:] == "cos" or project[5:] == "test":
            beamType = 'cosmics'
        elif project[7:] == '1beam':
            beamType = 'singlebeam'
        else:
            beamType = 'collisions'
            
    tilemon_log.info( 'Set up beam type: ' + beamType )
    TileNoiseFilter = 1


# #########################################
# The source of events, SFI for full events
# #########################################
#ByteStreamEmonInputSvc.Key = "ReadoutApplication"
if 'Key' not in dir():
    Key="dcm"
ByteStreamEmonInputSvc.Key = Key
ByteStreamEmonInputSvc.KeyCount = 1000

# ############################################################
# A list of of key values, e.g. a list of SFIs to contact.
# If not defined, all event providers of this type (e.g. all SFIs)
# ############################################################
#ByteStreamEmonInputSvc.KeyValue = ["TileREB-ROS" ]
#if 'KeyValue' not in dir():
#    KeyValue=[""]
#ByteStreamEmonInputSvc.KeyValue = KeyValue

if 'KeyValue' in dir():
    ByteStreamEmonInputSvc.KeyValue = KeyValue

# ##############################
# one of 'Ignore', 'Or', 'And'
# ##############################

if 'LVL1Logic' in dir():
    ByteStreamEmonInputSvc.LVL1Logic = LVL1Logic

# ###########################################################
# A LVL1 bit pattern to select events on.
#
# From 1 to 4 64bit values, first entry = Bit 0..63 in CTP
# ###########################################################
#ByteStreamEmonInputSvc.LVL1Bits = [ 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff]

# ###########################################################
# A list of L1 bit names to select on. This requires the
# L1CT.TrigConfL1Items to be published in IS.
# 
# This can be used instead of or in addition to LVL1Bits.
# ###########################################################
#ByteStreamEmonInputSvc.LVL1Names = ['L1_MU2', 'L1_MU3', 'L1_EM10' ]

if 'LVL1Names' in dir():
    ByteStreamEmonInputSvc.LVL1Names = LVL1Names

# make sure that we do not read calibration events in physics run
if 'StreamType' not in dir():
    StreamType = "calibration"
if 'StreamLogic' not in dir():
    StreamLogic = "And"
if 'StreamNames' not in dir():    
    StreamNames = [ "Tile" ]

# ###########################################
# Selection by stream tag:
# One of 'Or', 'And', 'Ignore' (default)
# ###########################################

if 'StreamLogic' in dir():
    ByteStreamEmonInputSvc.StreamLogic = StreamLogic

# ###########################################
# set stream type
# One of 'physics' or 'calibration' or 'express'
# ############################################

if 'StreamType' in dir():
    ByteStreamEmonInputSvc.StreamType = StreamType

# ############################################
# A list of stream tag names
# ############################################

if 'StreamNames' in dir():
    ByteStreamEmonInputSvc.StreamNames = StreamNames


# #################################################
# Shall athena exit if the partition is shutdown ?
# For offline athena tasks mainly.
# #################################################
#ByteStreamEmonInputSvc.ExitOnPartitionShutdown = False

if 'ISServer' not in dir():
    ISServer="Histogramming"
ByteStreamEmonInputSvc.ISServer = ISServer

if 'PublishName' not in dir():
    PublishName="TilePT-stateless-laser"
ByteStreamEmonInputSvc.PublishName = PublishName

if 'Frequency' not in dir():
    Frequency=100 #histograms update in number of events
ByteStreamEmonInputSvc.Frequency = Frequency

if 'UpdatePeriod' in dir():
    try:
        ByteStreamEmonInputSvc.UpdatePeriod = UpdatePeriod
    except Exception:
        treatException("Could not set UpdatePeriod")

if 'BufferSize' not in dir():
    BufferSize=10
try:
    ByteStreamEmonInputSvc.BufferSize = BufferSize
except Exception:
    treatException("Could not set BufferSize")

if 'PublishInclude' in dir():
    ByteStreamEmonInputSvc.Include = PublishInclude


ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
theApp.ExtSvc += [ "ByteStreamCnvSvc"]

print(ByteStreamEmonInputSvc)

include("TileMonitoring/jobOptions_TileLasMon.py")


# -----------------------------------------------------
# An example algorithm in Python
# -----------------------------------------------------
import AthenaPython.PyAthena as PyAthena

class ShowLVL1(PyAthena.Alg):
    """
    An athena algorithm written in python.
    """

    def __init__(self, name="ShowLVL1", **kw):
        kw['name'] = name
        super(ShowLVL1,self).__init__(name=name)
        # How to access properties
        self.SomeProperty = kw.get('SomeProperty', 'DefaultValue')

        self.ROBDataProviderSvc = None

    def initialize(self):
        self.msg.info("initializing [%s]", self.name())

        self.sg = PyAthena.py_svc("StoreGateSvc")
        if not self.sg:
            self.msg.error("Could not get StoreGateSvc")
            return PyAthena.StatusCode.Failure

        self.ROBDataProviderSvc = PyAthena.py_svc("ROBDataProviderSvc")
        if not self.ROBDataProviderSvc:
            self.msg.error("Could not get ROBDataProviderSvc")
            return PyAthena.StatusCode.Failure
        return PyAthena.StatusCode.Success

    def execute(self):
        self.msg.debug("executing [%s]", self.name())

        # Get EventInfo
        ev = self.sg.retrieve('EventInfo')

        # Print some LVL1 information
        print("Run:",ev.event_ID().run_number(),"Event:",ev.event_ID().event_number())
        for x in  ev.trigger_info().level1TriggerInfo():
            print("0x%08x" % x)

        return PyAthena.StatusCode.Success        

    def finalize(self):
        self.msg.debug("finalizing [%s]", self.name())
        return PyAthena.StatusCode.Success        

ShowLVL1Algo = ShowLVL1("ShowMyLVL1")

#--------------------------------------------------------------
# Private Application Configuration options, replace with
# your configuration.
#--------------------------------------------------------------
#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()
#topSequence += ShowLVL1Algo

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#ShowLVL1Algo.SomeProperty = 'Some Value'
