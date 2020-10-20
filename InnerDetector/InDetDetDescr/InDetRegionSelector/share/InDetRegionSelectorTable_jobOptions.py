###############################################################
#
# Job options fragment for InDetRegionSelectorLUT
#
#==============================================================

include.block("InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py")

# # For identfier to robid mappings
# if 'InDetCabling' not in theApp.Dlls:
#    theApp.Dlls += [ "InDetCabling" ]

# get DetFlags
from AthenaCommon.DetFlags import DetFlags
# get topSequence
topSequence = AlgSequence()
# get ToolSvc
ToolSvc = Service( "ToolSvc" )

