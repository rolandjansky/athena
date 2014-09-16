# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###### LAr FEBMon Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArFEBMon

conddb.addFolder('LAR','/LAR/Configuration/DSPThreshold/Thresholds') 

theLArFEBMon = LArFEBMon(name="LArFEBMon",
                         ProcessNEvents = EventBlockSize,
                         IgnoreMissingHeaderPS = False,
                         IgnoreMissingHeaderEMB = False
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
