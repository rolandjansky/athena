

readAOD=True
doWriteESD=False 
doWriteAOD=False 
doAOD=False
doAODLVL1=False
doWriteTAG=False

doTrigger=False
EvtMax=-1

include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()


include ("RecExCommon/RecExCommon_topOptions.py")

GeoModelSvc = Service( "GeoModelSvc")
GeoModelSvc.IgnoreTagDifference = True


# hack starts here:

class DummyEventSelector   :
    InputCollections= []

class DummyServiceMgr  :
    EventSelector= DummyEventSelector

#create a dummy ServiceMgr
ServiceMgr=DummyServiceMgr()

#hack ends here
