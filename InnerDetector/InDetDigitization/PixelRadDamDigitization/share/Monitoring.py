#
# Add monitoring ot the digitization.
# Usage:
#   athena DigitizationTst.py Monitoring.py
#
# If the Special Pixel Map is to be monitored, then
# modify the RDOName by adding _SPM to the name used
# for the normal RDOs (default "PixelRDOs")
#
#--------------------------------------------------------------
#load the main monitoring algorithm and tool service               
theApp.Dlls += [ "PixelMonitoring" ];                        
theApp.TopAlg += [ "PixelMonitor" ];                   
PixelMonitor = Algorithm( "PixelMonitor" ) 
ToolSvc = Service("ToolSvc")

#load a tool to run in the algorithm.  Fill histograms with RDO data   
PixelMonitor.PixelMonTools += ["RDOTool"]
RDOTool= Service("ToolSvc.RDOTool")
#load job options files for these tools      
ToolSvc.RDOTool.RDOName = "PixelRDOs"
###ToolSvc.RDOTool.RDOName = "PixelRDOs_SPM"
ToolSvc.RDOTool.doA = False  #Turns on EndcapA occupancy maps
ToolSvc.RDOTool.doB = False  #Turns on Barel occupancy maps (SLOW!!!)
ToolSvc.RDOTool.doC = False  #Turns on EndcapC occupancy maps

theApp.Dlls += [ "GaudiSvc" ]
THistSvc = Algorithm( "THistSvc" )
THistSvc.Output = ["stat DATAFILE='digitMon.root' TYP='ROOT' OPT='RECREATE'"]


# End of job options file
#
#
