########################################
#
# Example setup for ByteStreamEmonSvc
# 
#########################################

## -------------------------------------------------------------------
## list of local flags

if ('partitionName' not in dir() or partitionName is None):
    partitionName = 'ATLAS'

if ('publishName' not in dir() or publishName is None):
    publishName = 'Global_PT'

if ('streamType' not in dir() or streamType is None):
    streamType = 'physics'

if ('streamName' not in dir() or streamName is None):
    streamName = 'Egamma:Muons'

if ('streamLogic' not in dir() or streamLogic is None):
    streamLogic = 'Ignore'

if ('lvl1Name' not in dir() or lvl1Name is None):
    lvl1Name = 'L1_MU2:L1_MU3:L1_EM10'

if ('lvl1Logic' not in dir() or lvl1Logic is None):
    lvl1Logic = 'Ignore'

if ('keyname' not in dir()):
    keyname = 'SFI'

if ('keycount' not in dir()):
    keycount = 10

if ('buffersize' not in dir()):
    buffersize = 10

if ('updateperiod' not in dir()):
    updateperiod = 200

if ('timeout' not in dir()):
    timeout = 240000

if ('groupName' not in dir()):
    groupName = 'RecExOnline'

if ('isserverName' not in dir()):
    isserverName = 'Histogramming'

if ('isGlobalMonitoring' not in dir()):
    isGlobalMonitoring = False

## -------------------------------------------------------------------
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
ByteStreamEmonInputSvc.Partition = partitionName

# #######################################################
# The source of events, can be 'REB' (ROS Event Builder),
# 'SFI' (output of L2, default), 'efd' (output of EF)
# #######################################################
#print "IRH got Key: keyname: "+keyname
ByteStreamEmonInputSvc.Key = keyname

# special settings for pixel stand-alone partition
if (partitionName == 'PixelDD'):
    ByteStreamEmonInputSvc.Key = 'ReadoutApplication'
    ByteStreamEmonInputSvc.KeyValue = [ 'ROSEventBuilder' ]

# ############################################################
# A list of of key values, e.g. a list of SFIs to contact.
# If not defined, one event provider of this type (i.e. any SFI)
# ############################################################
#ByteStreamEmonInputSvc.KeyValue = [ 'SFI-1', 'SFI-2' ]

# #######################################
# Alternative: N providers of type 'SFI'
# KeyValue is ignored if this is set.
# ######################################
ByteStreamEmonInputSvc.KeyCount = keycount

# #######################################
# Set this to the IS server where you want
# to publish histograms, too. If unset, no
# histograms are published.
# #######################################
if (isGlobalMonitoring and partitionName in ['ATLAS', 'TDAQ']):
    ByteStreamEmonInputSvc.ISServer = 'Histogramming-Global-iss'
else:
    ByteStreamEmonInputSvc.ISServer = isserverName

# ########################################
# The provider name under which your histograms
# appear in OH.
# ########################################
ByteStreamEmonInputSvc.PublishName = publishName

# ###################################################
# Should histograms be cleared at new run? default: yes
# ###################################################
ByteStreamEmonInputSvc.ClearHistograms = True

# ####################################################
# A regular expression to restrict which histograms are published.
# Default: no selection - include all, exclude none
# ####################################################
# AK: 11-07-2014 in rel.19 SHIFT and EXPERT histogram folders are disabled. This means that no histograms are published, since SHIFT is enabled and EXPERT disabled.
#if (isGlobalMonitoring):
#    ByteStreamEmonInputSvc.Include = '/SHIFT/.*'
#    ByteStreamEmonInputSvc.Exclude = '/EXPERT/.*'

# ###############################################
# Frequency of updates (in number of events, not secs...)
# ###############################################
#ByteStreamEmonInputSvc.Frequency = 5

ByteStreamEmonInputSvc.BufferSize = buffersize
ByteStreamEmonInputSvc.UpdatePeriod = updateperiod
ByteStreamEmonInputSvc.Timeout = timeout

# ##############################
# one of 'Ignore', 'Or', 'And'
# ##############################
ByteStreamEmonInputSvc.LVL1Logic = lvl1Logic

# #########################################
# One of 'TAP', 'TBP' or 'TAV' (default)
# #########################################
#ByteStreamEmonInputSvc.LVL1Origin = 'TAV'

# ###########################################################
# A list of L1 bit names to select on. This requires the
# L1CT.TrigConfL1Items to be published in IS.
# This can be used instead of or in addition to LVL1Bits.
# ###########################################################
if (lvl1Logic != 'Ignore'):
    ByteStreamEmonInputSvc.LVL1Names = lvl1Name.split(':')

# ##########################################################
# A list of numerical trigger bits instead of names. This is
# Or'ed with LVL1Names
# ##########################################################
#ByteStreamEmonInputSvc.LVL1Items = [ 10, 20, 72, 245 ]

# ###########################################
# Selection by stream tag:
# One of 'Or', 'And', 'Ignore' (default)
# ###########################################
ByteStreamEmonInputSvc.StreamLogic = streamLogic

# ###########################################
# One of 'physics' or 'calibration'
# ############################################
if (streamName == 'express'):
    ByteStreamEmonInputSvc.StreamType = 'express'
else:
    ByteStreamEmonInputSvc.StreamType = streamType

# ############################################
# A list of stream tag names
# ############################################
if (streamName == ""):
    ByteStreamEmonInputSvc.StreamNames = list()
else:
    ByteStreamEmonInputSvc.StreamNames = streamName.split(':')

# #################################################
# Shall athena exit if the partition is shutdown ?
# For offline athena tasks mainly.
# #################################################
ByteStreamEmonInputSvc.ExitOnPartitionShutdown = False

# #################################################
# Set groupName to "RecExOnline" by default so that 
# each process belonging to the "RecExOnline" group 
# gets a different set of events.
# Set groupName to empty to allow other process get 
# the same events.
# #################################################
ByteStreamEmonInputSvc.GroupName = groupName

ByteStreamCnvSvc = Service('ByteStreamCnvSvc')
theApp.ExtSvc += [ 'ByteStreamCnvSvc' ]
