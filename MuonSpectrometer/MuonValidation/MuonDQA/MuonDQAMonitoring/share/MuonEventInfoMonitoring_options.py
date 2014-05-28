from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags

if MuonDQAFlags.doMuonEvtMon:
   #-----------------------MuonEventInfoMon---------------------------------------
   from MuonDQAMonitoring.MuonDQAMonitoringConf import MuonDQA__MuonEventInfoMonTool
   MuonDQA__MuonEventInfoMonTool = MuonDQA__MuonEventInfoMonTool (
   name = "MuonEventInfoMonTool" 
   ) 
   MuonDQA__MuonEventInfoMonTool.LastEvent = MuonDQAFlags.EvtMax 
   MuonDQA__MuonEventInfoMonTool.TriggerTagAdd = True
   ToolSvc += MuonDQA__MuonEventInfoMonTool
   monMan.AthenaMonTools += [ MuonDQA__MuonEventInfoMonTool ]  
