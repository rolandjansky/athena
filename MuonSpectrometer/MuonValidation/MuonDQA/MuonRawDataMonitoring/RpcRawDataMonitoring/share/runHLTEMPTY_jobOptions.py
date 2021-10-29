############################################################################################################
### Input stream
############################################################################################################
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import RpcRawDataMonitoring.RpcOccupancy_Config as AthRead  # python/RpcOccupancy_Config

import os

if 'EvtMax' in dir():
    athenaCommonFlags.EvtMax = EvtMax
else:
    athenaCommonFlags.EvtMax = -1

file_list = []
if os.path.exists('input.txt'):
  infile = open('input.txt', "r")
  file_list = infile.readlines()
  file_list = [ filename.strip() for filename in file_list ]
  print "read files path from input.txt ."
  print "files paths: \n", file_list
else:
  file_list =  ['/eos/atlas/atlascerngroupdisk/det-rpc/data/DESDM_MCP/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024._0967.1']
  print "file input.txt does not exist"
  print "files paths: \n", file_list


# athenaCommonFlags.FilesInput = ['/eos/atlas/atlascerngroupdisk/det-rpc/data/DESDM_MCP/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024/data18_13TeV.00358615.physics_Main.merge.DESDM_MCP.f961_m2024._0084.1']
athenaCommonFlags.FilesInput = file_list

if 'outfile' not in dir():
    outfile = 'rpcmon_HLTEMPTY'

print '======================================================================'    
print 'Output file name: %s\n' %(outfile+".root")
print '======================================================================'    

############################################################################################################
### Configure tools and services
############################################################################################################

if 'writeGeo' not in dir():
    writeGeo = False
    
if 'writeIDTracks' not in dir():
    writeIDTracks = True

#--------------------------------------------------------------------------------------
# Include and configure RecExCommon
#
include('RpcRawDataMonitoring/CommonConfig_RpcRawDataMonitoring.py')

include('RecExCommon/RecExCommon_topOptions.py')

#--------------------------------------------------------------------------------------
# Must setup cabling services AFTER RecExCommon
#
import MuonCnvExample.MuonCablingConfig
import MuonRPC_Cabling.MuonRPC_CablingConfig

#--------------------------------------------------------------------------------------
# Configure region selector tools
#
from RegionSelector.RegSelSvcDefault import RegSelSvcDefault
theRegSelSvc = RegSelSvcDefault()
theRegSelSvc.enableMuon = True
theRegSelSvc.enableMDT  = False
theRegSelSvc.enableCSC  = False
theRegSelSvc.enableTGC  = False
theRegSelSvc.enableRPC  = True
ServiceMgr += theRegSelSvc

#--------------------------------------------------------------------------------------
# Set output using thistsvc
#
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += [outfile+" DATAFILE='%s' OPT='RECREATE'" %(outfile+".root")]

svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.infoLimit = 0

#--------------------------------------------------------------------------------------
# Configure algorithm sequence
#
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

trig_decision_tool = CfgMgr.Trig__TrigDecisionTool('TrigDecisionTool', TrigDecisionKey = 'xTrigDecision')
ToolSvc += trig_decision_tool

job += AthRead.histos_HLT_EMPTY(CfgMgr, outfile)


#--------------------------------------------------------------------------------------
# Print debug info
print "if ServiceMgr==svcMgr:  ", ServiceMgr==svcMgr


if 'debug' in dir():
    print ToolSvc
    print "ServiceMgr --------"
    print ServiceMgr
    print "svcMgr ------------"
    print svcMgr.MessageSvc

if 'dumpSG' in dir():
    StoreGateSvc = Service('StoreGateSvc')
    StoreGateSvc.Dump = dumpSG

