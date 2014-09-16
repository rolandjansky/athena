# JobOption for the LArHVScaleTool
#
# Warning : this fragment assumes that the GlobalFlags have been set
#           It cannot be run in standalone !!!
# 
# author C. Bourdarios-Adam , january 2005.

theApp.Dlls += ["LArRecUtils","LArCondUtils"]

ToolSvc = Service ( "ToolSvc" )

# Set geomety ( "Atlas", "H8", "H6" ) and the 
# level of details :

#     "Fixed"  will use the hardcoded-problems
#     "PerRun" will read the HV DB once and cache the scales
#     "Full" will read the HV DB each time ( and catch possible changes )

#     "None" points out that only H8 is available up to now

if GlobalFlags.DetGeo.is_ctbh8() :
    ToolSvc.LArHVScaleTool.geometry = "H8"
    ToolSvc.LArHVScaleTool.details = "Fixed"
    include( "LArCondCnv/LArCondCnv_HV_H8_jobOptions.py" )

else :
    if  GlobalFlags.DetGeo.is_ctbh6() :
        ToolSvc.LArHVScaleTool.geometry = "H6"
        ToolSvc.LArHVScaleTool.details = "None"
        
    else :
        ToolSvc.LArHVScaleTool.geometry = "Atlas"
        ToolSvc.LArHVScaleTool.details = "None"
    
