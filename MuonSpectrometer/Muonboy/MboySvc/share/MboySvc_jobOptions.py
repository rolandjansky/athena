include.block ( "MboySvc/MboySvc_jobOptions.py" )
#--------------------------------------------------------------
# MboySvc
#-----------------------------------------------------------------
theApp.Dlls   += [ "MboySvc" ]
#-----------------------------------------------------------------
theApp.ExtSvc += [ "MboySvc" ]

#--------------------------------------------------------------
#load MboyEDMTool
include( "MboyEDMTool/MboyEDMTool_jobOptions.py" )

