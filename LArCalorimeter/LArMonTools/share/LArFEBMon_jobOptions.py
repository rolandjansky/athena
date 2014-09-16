# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

#Run is Online or Offline:
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags 
OnlineMode=athenaCommonFlags.isOnline()

###### LAr FEBMon Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArFEBMon

if conddb.dbname == "CONDBR2":
   conddb.addFolder('LAR','/LAR/Configuration/DSPThresholdFlat/Thresholds') 
   keyDSPThr="/LAR/Configuration/DSPThresholdFlat/Thresholds"
else:
   conddb.addFolder('LAR','/LAR/Configuration/DSPThreshold/Thresholds') 
   keyDSPThr="LArDSPThresholds"

theLArFEBMon = LArFEBMon(name="LArFEBMon",
                         ProcessNEvents = EventBlockSize,
                         IgnoreMissingHeaderPS = False,
                         IgnoreMissingHeaderEMB = False,
                         keyDSPThresholds = keyDSPThr,
                         IsOnline = OnlineMode
                         )
ToolSvc += theLArFEBMon
LArMon.AthenaMonTools+=[ theLArFEBMon ] 

# BadChannelTool configuration
from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
theLArBadChannelTool=LArBadChanTool()
theLArBadChannelTool.ReadFromASCII=False
ToolSvc+=theLArBadChannelTool
ToolSvc.LArFEBMon.LArBadChannelTool=theLArBadChannelTool

#to change an option later, do e.g
#ToolSvc.LArFEBMon.maxOfTimingHisto =100
#ToolSvc.LArFEBMon.OutputLevel =DEBUG
