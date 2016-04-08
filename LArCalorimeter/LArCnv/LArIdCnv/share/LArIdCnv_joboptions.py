include.block("LArIdCnv/LArIdCnv_joboptions.py")
#
#  Joboptions for the loading of the of LArID converters
#
# add the LArCablingService DLL:
include( "LArTools/LArTools_jobOptions.py" )

# DLLs 
theApp.Dlls += [ "LArIdCnv" ]
