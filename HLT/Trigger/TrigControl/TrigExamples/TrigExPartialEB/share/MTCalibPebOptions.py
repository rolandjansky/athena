#**************************************************************
#
# MTCalibPeb example
#
#==============================================================
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.Constants import *
from AthenaCommon import CfgMgr

svcMgr.ROBDataProviderSvc.OutputLevel = OUTPUTLEVEL

svcMgr.ByteStreamAddressProviderSvc.OutputLevel = OUTPUTLEVEL
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult"
    ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
HelloWorld = CfgMgr.MTCalibPeb("HelloWorld")

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()
topSequence += HelloWorld

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from TriggerJobOpts.TriggerFlags import TriggerFlags
if TriggerFlags.doLVL2():
    hltInstance = 'L2'
elif TriggerFlags.doEF():
    hltInstance = 'EF'
else:
    hltInstance = 'HLT'

#
#--- the HLT instance = L2/EF/HLT
#
HelloWorld.HLTInstance = hltInstance
#
#--- the base name of the partial event building info SG key
#
HelloWorld.HLTResultName="HLTResult"
#
#--- retrieve the Level-1 result
#
HelloWorld.RetrieveLvl1 = TRUE
#
#--- retrieve the ROBs over the network
#
HelloWorld.RetrieveROBs = TRUE
#
#--- time between ROB retrievals over the network in micro seconds
#    (Caution: this will add to the total proceesing time a time value of
#              Time = # of ROS retrievals * TimeBetweenRobRet)
#    (<= 0: fixed retrieval time, > 0: random retrieval time in [0,value])
#
HelloWorld.TimeBetweenRobRet = 1000
#
#--- Give a dictionary of ROBs to be registered for preloading or retrieved from the
#    ROS over the network (they have to be contained in the input file)
#    Remarks:
#    1)   the string keys have to be unique
#    2)   use a consecutive numbering in the first field to preserve the order of configured operations
#    3)   keyword :ADD:  Robs will be registered for prefetching
#         keyword :GET:  Robs will be retrieved from ROS
#         keyword :COL:  collect all ROBs for event = event building 
#
#--- empty dictionary of ROBs
# HelloWorld.RobId = {}
# ------------------
HelloWorld.RobId = {
 "01 :ADD: Preload  ": [ 0x42002a, 0x42002b ],    # robs for 1st preload
 "02 :ADD: Preload  ": [ 0x42002e, 0x42002f ],    # robs for 2nd preload
 "03 :GET: Retrieve ": [ 0x42002e, 0x420060 ],    # robs for 1st retrieval
 "04 :ADD: Preload  ": [ 0x420060 ],              # robs for 3rd preload
 "05 :ADD: Preload  ": [ 0x420064 ],              # robs for 4th preload
 "06 :ADD: Preload  ": [ 0x42002e, 0x420060 ],    # robs for 5th preload
 "07 :GET: Retrieve ": [ 0x420060 ],              # robs for 2nd retrieval
 "08 :GET: Retrieve ": [ 0x420064 ],              # robs for 3rd retrieval
 "09 :COL: Ev.Build ": [ 0x0 ]                    # event building
 }
#
#--- specify Stream Tags for accepted events
#    mandatory: (stream name, stream type, obeys lumi block)
#    for calibration StreamTags optionally a list of Robs and/or sub detectors for PEB can be specified
#          (stream name, stream type, obeys lumi block, [list of Robs], [list of sub detectors])
#
HelloWorld.ConfiguredStreamTags = [
                                   ['MyPhysicStream1','physics','true'],
                                   ['MyDebugStream1','debug','false'],
                                   ['MyCalibStream1','calibration','false','[0x42002a, 0x42002b]','[0x41,0x42]'],
                                   ['DataScouting_05_Muon','calibration','false','[0x7c0005]'],
                                   ['DataScouting_13_Jet','calibration','true','[0x7c000d]']
                                   ]
#
#--- specify accepted chain numbers
#
HelloWorld.ConfiguredChainNumbers = [ 1,2,3,4,55 ]
#
#--- specify random accept rate
#    (< 0. event always rejected, > 1. event always accept)    
#
HelloWorld.RandomAcceptRate = 1.5  
#
#--- specify burn time in micro seconds (total time = BurnTime*NumberOfBurnCycles)
#    (<= 0 fixed burn time, > 0 random burn time in [0,value])
#
HelloWorld.BurnTime           = 100   # time burned in each loop iteration
HelloWorld.NumberOfBurnCycles = 10    # number of iterations
#
#==============================================================
#
# End of MTCalibPeb example
#
#**************************************************************
