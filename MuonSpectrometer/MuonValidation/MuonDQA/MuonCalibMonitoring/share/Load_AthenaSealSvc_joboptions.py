#
# JobOptions for the loading of the AthenaSealSvc
#
# Some standard joboptions for dictionaries:
import AthenaCommon.AtlasUnixGeneratorJob

theApp.EvtMax                   = 1


AthenaSealSvc = Service( "AthenaSealSvc" )

# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )

# Add in your includes to check specific dictionaries: e.g. EventInfo:
#include( "EventInfo/EventInfoDict_joboptions.py" )
include("CscCalibData/CscCalibDataDict_joboptions.py")

print AthenaSealSvc.DictNames

# Set to output level to debug (2) for more information
AthenaSealSvc.OutputLevel      = 2
MessageSvc.debugLimit  = 100000
MessageSvc.infoLimit  = 100000

# Check the dictionary in memory for completeness
AthenaSealSvc.CheckDictAtInit = TRUE
AthenaSealSvc.CheckDictionary = True;

# Must add in svc to be created via python below
theApp.CreateSvc += [ "AthenaSealSvc" ]

# Must initialize service explicitly
AthenaSealSvc.initialize()
