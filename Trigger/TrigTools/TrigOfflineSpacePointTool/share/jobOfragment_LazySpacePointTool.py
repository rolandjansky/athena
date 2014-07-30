###############################################################
#
# Job options file
#
#==============================================================
# DLL's to be loaded:

# space point formation tools:
theApp.Dlls += ["SiSpacePointTool","SiTrigSpacePointFormation"]
# clusterization:
theApp.Dlls += ["InDetPrepRawDataFormation","InDetTrigPrepRawDataFormat","SiClusterizationTool"] 

theApp.Dlls += [ "TrigOfflineSpacePointTool" ]


