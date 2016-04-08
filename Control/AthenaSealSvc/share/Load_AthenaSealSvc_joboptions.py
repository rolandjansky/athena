#
# JobOptions for the loading of the AthenaSealSvc
#
# Some standard joboptions for dictionaries:

theApp.EvtMax = 1

if not hasattr(svcMgr, 'AthenaSealSvc'):
    svcMgr += CfgMgr.AthenaSealSvc()
AthenaSealSvc = svcMgr.AthenaSealSvc

# Set up service
include( "AthenaSealSvc/AthenaSealSvc_joboptions.py" )

print AthenaSealSvc.DictNames

# Set to output level to debug (2) for more information
AthenaSealSvc.OutputLevel      = 2
MessageSvc.debugLimit  = 100000
MessageSvc.infoLimit  = 100000

# Check the dictionary in memory for completeness
AthenaSealSvc.CheckDictAtInit = True
AthenaSealSvc.CheckDictionary = True

# Must add in svc to be created via python below
theApp.CreateSvc += [ "AthenaSealSvc" ]

