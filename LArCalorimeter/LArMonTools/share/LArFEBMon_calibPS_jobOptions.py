# --- for athena online running ---
if 'EventBlockSize' not in dir():
    EventBlockSize=0

###### LAr FEBMon Tool Configuration ###############
from LArMonTools.LArMonToolsConf import LArFEBMon

conddb.addFolder('LAR','/LAR/Configuration/DSPThreshold/Thresholds') 

theLArFEBMon = LArFEBMon(name="LArFEBMon",
                         ProcessNEvents = EventBlockSize,
                         IgnoreMissingHeaderPS = False,
                         IgnoreMissingHeaderEMB = True
                         )
#ToolSvc += theLArFEBMon
LArMon.AthenaMonTools+=[ theLArFEBMon ] 


#to change an option later, do e.g
#ToolSvc.LArFEBMon.maxOfTimingHisto =100
#ToolSvc.LArFEBMon.OutputLevel =DEBUG
