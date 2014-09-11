# Dumping the constants from JetCalib folders into text file
# either define a GlobalTag, or for each foldername in dump_tool list
# default dump_tool is ['EtaMassEnergyCorr']
# gove a corresponding tag in dump_tag list
# Jets for which the constants should be dumped are listed in dump_jets list
# default dump_jets are ["AntiKt4Topo","AntiKt4TopoTower"]
# Optional JetDB point to a source DB (CALO - standard DB connection for jobs, sqlite file,...)
# Optional RunNumber gives the IOV for constants, default is 999999 (practically current run)
import sys

if not 'dump_tool' in dir():
   dump_tool=["EtaMassEnergyCorr"]

if not 'dump_jets' in dir():
   dump_jets=["AntiKt4Topo","AntiKt4TopoTower"]

if not 'GlobalTag' in dir():
   if not 'dump_tag' in dir():
      dump_tag=['']
   if len(dump_tool) != len(dump_tag):
      print "Not equal length of list of tools and list of tags....."
      exit(-1)

if not 'outfile' in dir():
   outfile="mydump.txt"

if not 'JetDB' in dir():
   #JetDB="sqlite://;schema=/afs/cern.ch/user/p/pavol/scratch0/JetCalib/DBtest/cooldummy.db;dbname=OFLP200"
   JetDB="CALO"

if not 'RunNumber' in dir():
   RunNumber=999999

import AthenaCommon.AtlasUnixGeneratorJob

# Set up minimal environment, no event loop, no event selector
theApp.EvtSel = "NONE"
theApp.EvtMax=1

from JetCalibTools.JetCalibToolsConf import JetCalibCorrDumper
from JetCalibTools.DBUtils import *
from JetCalibTools.SetupJetCalibrators import *
import IOVDbSvc.IOVDb

from JetRec.JetRecFlags import jetFlags
jetFlags.JetCalibDB.set_Value_and_Lock(JetDB)

corr_tool_list = []

setups = { "EtaMassEnergyCorr" : doEtaJESCorrection,
           "OffsetCorrection"  : doEMOffset
         }

# now loop over asked tools and jets, and construct list of tools

for mytool in dump_tool:
    for jet in dump_jets:
        plist=getNameFromJetname(jet,True,True) 
        conftool=setups[mytool](plist[0],plist[1],plist[2])
        print conftool
        ToolSvc += conftool
        corr_tool_list += [ conftool ]

# now run our dumper        
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

jetCorrDump = JetCalibCorrDumper()
jetCorrDump.ClusterCorrectionTools = [ t.getFullName() for t in corr_tool_list]
jetCorrDump.FileName = outfile

topSequence += jetCorrDump

print topSequence.JetCalibCorrDumper

svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.OverrideRunNumber=True

from IOVDbSvc.CondDB import conddb
if 'GlobalTag' in dir():
   conddb.setGlobalTag(GlobalTag)
else:   
   for i in range(0,len(dump_tool)):
      print "Override: ",jetFlags.JetCalibDBFolderBase()+"/"+dump_tool[i]," with: ",dump_tag[i]
      conddb.addOverride(jetFlags.JetCalibDBFolderBase()+"/"+dump_tool[i],dump_tag[i])

svcMgr.IOVDbSvc.dbConnection = JetDB
svcMgr.IOVDbSvc.OutputLevel = DEBUG
svcMgr.IOVSvc.OutputLevel = DEBUG
svcMgr.IOVSvc.preLoadData=True
svcMgr.MessageSvc.OutputLevel = DEBUG

