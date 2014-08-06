#########################################
#
# Example setup for ByteStreamEmonSvc
# 
#########################################

# Common part, copy from here

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *

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

ByteStreamEmonInputSvc.Partition = "ATLAS"

# #########################################
# The source of events, SFI for full events
# #########################################
ByteStreamEmonInputSvc.Key = "SFI"

# ############################################################
# A list of of key values, e.g. a list of SFIs to contact.
# If not defined, one event provider of this type (i.e. any SFI)
# ############################################################

# ByteStreamEmonInputSvc.KeyValue = ["SFI-1", "SFI-2" ]

# #######################################
# Alternative: N providers of type 'SFI'
# KeyValue is ignored if this is set.
# ######################################
ByteStreamEmonInputSvc.KeyCount = 20

# #######################################
# Set this to the IS server where you want
# to publish histograms, too. If unset, no
# histograms are published.
# #######################################
###ByteStreamEmonInputSvc.ISServer = 'Histogramming'

# ########################################
# The provider name under which your histograms
#  appear in OH.
# ########################################
ByteStreamEmonInputSvc.PublishName = 'HelloWorld'

# ########################################
# Buffer size
# ########################################
ByteStreamEmonInputSvc.BufferSize = 100

# ###################################################
# Should histograms be cleared at new run ? default: yes
# ###################################################
ByteStreamEmonInputSvc.ClearHistograms = True

# ####################################################
# A regular expression to restrict which histograms are published.
# ####################################################
# ByteStreamEmonInputSvc.Include = '.*'
# ByteStreamEmonInputSvc.Exclude = ''

# ###############################################
# Frequency of updates (in number of events, not secs...)
# ###############################################
# ByteStreamEmonInputSvc.Frequency = 5


# ##############################
# one of 'Ignore', 'Or', 'And'
# ##############################
ByteStreamEmonInputSvc.LVL1Logic = "Ignore"

# #########################################
# One of 'TAP', 'TBP' or 'TAV' (default)
# #########################################
#ByteStreamEmonInputSvc.LVL1Origin = "TAV"

# ###########################################################
# A list of L1 bit names to select on. This requires the
# L1CT.TrigConfL1Items to be published in IS.
# 
# This can be used instead of or in addition to LVL1Bits.
# ###########################################################
#ByteStreamEmonInputSvc.LVL1Names = ['L1_MU2', 'L1_MU3', 'L1_EM10' ]

# A list of numerical trigger bits instead of names. This is
# Or'ed with LVL1Names
#ByteStreamEmonInputSvc.LVL1Items = [ 10, 20, 72, 245 ]

# ###########################################
# Selection by stream tag:
# One of 'Or', 'And', 'Ignore' (default)
# ###########################################
ByteStreamEmonInputSvc.StreamLogic = "Ignore"

# ###########################################
# One of 'physics' or 'calibration'
# ############################################
#ByteStreamEmonInputSvc.StreamType = "calibration"

# ############################################
# A list of stream tag names
# ############################################
ByteStreamEmonInputSvc.StreamNames = [ "L2CostMonitoring" ]

# #################################################
# Shall athena exit if the partition is shutdown ?
# For offline athena tasks mainly.
# #################################################
#ByteStreamEmonInputSvc.ExitOnPartitionShutdown = False

#----------------------------------------------------------------------
# Setup services for reading BS files
#
ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
ByteStreamCnvSvc.InitCnvs += [ "EventInfo", "HLT::HLTResult" ]

theApp.ExtSvc += [ "ByteStreamCnvSvc"]

ByteStreamAddressProviderSvc = Service( "ByteStreamAddressProviderSvc" )
ByteStreamAddressProviderSvc.TypeNames += [ "HLT::HLTResult/HLTResult_L2",
                                            "HLT::HLTResult/HLTResult_EF" ]

ByteStreamEmonInputSvc.OutputLevel = 4

#----------------------------------------------------------------------
# Setup cost algorithm
#
from AthenaCommon.AlgSequence import AlgSequence
from TrigCostMonitor.TrigCostMonitorConfig import prepareCostRun

runAlg = prepareCostRun('TrigCostRun', 'emon')

if ('setDebug' in dir()):
    runAlg.OutputLevel = 2
    for tool in runAlg.tools:
        tool.OutputLevel = 2

if ('printEvent' in dir()):
    runAlg.printEvent = True

topSeq = AlgSequence()
topSeq += runAlg

if ('dumpSG' in dir()):
    StoreGateSvc = Service("StoreGateSvc")
    StoreGateSvc.Dump = True

#----------------------------------------------------------------------
# Message service and loop manager options
#
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.defaultLimit = 0
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%35W%S%7W%R%T %0W%M"
ServiceMgr.MessageSvc.showStats = True
ServiceMgr.MessageSvc.statLevel = WARNING

if not hasattr(ServiceMgr, "AthenaEventLoopMgr"):
    from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
    ServiceMgr += AthenaEventLoopMgr()
    
ServiceMgr.AthenaEventLoopMgr.EventPrintoutInterval = 100
