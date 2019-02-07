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
if not 'Partition' in dir():
  Partition = 'ATLAS'
print "ByteStreamEmonInputSvc: Setting partition to : %s" % (Partition)
ByteStreamEmonInputSvc.Partition = Partition

# #########################################
# The source of events, SFI for full events
# #########################################
if not 'SamplingLevel' in dir():
  SamplingLevel="efd"
ByteStreamEmonInputSvc.Key = SamplingLevel

# ############################################################
# A list of of key values, e.g. a list of SFIs to contact.
# If not defined, one event provider of this type (i.e. any SFI)
# ############################################################
#ByteStreamEmonInputSvc.KeyValue  = ["SFI-1", "SFI-2", "SFI-3" ]

# #######################################
# Alternative: N providers of type 'SFI'
# KeyValue is ignored if this is set.
# ######################################
if not 'Nodes' in dir():
  Nodes=10
ByteStreamEmonInputSvc.KeyCount = Nodes

# #######################################
# Set this to the IS server where you want
# to publish histograms, too. If unset, no
# histograms are published.
# #######################################
#ByteStreamEmonInputSvc.ISServer = 'Histogramming'

# ########################################
# The provider name under which your histograms
#  appear in OH.
# ########################################
#ByteStreamEmonInputSvc.PublishName = 'TRT_PT'

# ###################################################
# Should histograms be cleared at new run ? default: yes
# ###################################################
#ByteStreamEmonInputSvc.ClearHistograms = True

# ####################################################
# A regular expression to restrict which histograms are published.
# ####################################################
# ByteStreamEmonInputSvc.Include = '.*'
# ByteStreamEmonInputSvc.Exclude = ''

# ###############################################
# Frequency of updates (in number of events, not secs...)
# ###############################################
# ByteStreamEmonInputSvc.Frequency = 5

# #################################
# Set dispersion flag to false (so other can sample the same event
# #################################
if not 'Dispersion' in dir():
  Dispersion=False
ByteStreamEmonInputSvc.Dispersion=Dispersion

# #################################
# Set timeout (in ms?)
# #################################
ByteStreamEmonInputSvc.Timeout=600000

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
if 'LVL1Names' in dir():
  ByteStreamEmonInputSvc.LVL1Logic = "Or"
  ByteStreamEmonInputSvc.LVL1Names = LVL1Names.split(' ')

# A list of numerical trigger bits instead of names. This is
# Or'ed with LVL1Names
#ByteStreamEmonInputSvc.LVL1Items = [ 10, 20, 72, 245 ]

# ###########################################
# Selection by stream tag:
# One of 'Or', 'And', 'Ignore' (default)
# ###########################################
## Set stream logic ignore for 'Random' stream
if ( not 'Stream' in dir()) or (Stream == 'Default') or (Stream.startswith("Test")):
  ByteStreamEmonInputSvc.StreamLogic = "Ignore"
else:
  ByteStreamEmonInputSvc.StreamLogic = "Or"

# ###########################################
# One of 'physics' or 'calibration'
# ############################################
#Make sure it is defined
if not 'StreamType' in dir():
  StreamType="physics"
ByteStreamEmonInputSvc.StreamType = StreamType 

# ############################################
# A list of stream tag names
# ############################################
ByteStreamEmonInputSvc.StreamNames = Stream.split(' ')

# #################################################
# The event buffer size
# #################################################
ByteStreamEmonInputSvc.BufferSize = 30

# #################################################
# Shall athena exit if the partition is shutdown ?
# For offline athena tasks mainly.
# #################################################
#ByteStreamEmonInputSvc.ExitOnPartitionShutdown = False

ByteStreamCnvSvc = Service( "ByteStreamCnvSvc" )
theApp.ExtSvc += [ "ByteStreamCnvSvc"]

###
# Switch output level if requested
###
if not 'BSEmonMsgLvl' in dir():
   BSEmonMsgLvl=WARNING
ByteStreamEmonInputSvc.OutputLevel=BSEmonMsgLvl

if BSEmonMsgLvl <= DEBUG:
  print "\n\n\t ByteStreamEmonInput setup:\n",ByteStreamEmonInputSvc,"\n\n"
