# exercise dictionary
if True:
    d={"obj1":["tag1","tag2"],"obj2":"another" }
    thetag=d["obj2"]
    print thetag  # single string
    thetag=d["obj1"]
    print thetag  # list of string
    # thetag=d["obj3"]
    print thetag  # crash if wrong
    print d.has_key("obj3")
    print d.has_key("obj2")
    print d.keys()
    # the following does not work
    #o="obj2"
    #x=""
    #print d.get( o[, x])
    # print d.get("obj3"[, ""])    

from AthenaCommon.Logging import logging

#logRecExCommon_topOptions = logging.getLogger( 'py:RecExCommon_topOptions' )
doNameAuditor=True
doTrigger=False
doWriteESD=False
doWriteAOD=False
doWriteTAG=False
doAOD=False
include ( "RecExCommon/RecExCommon_flags.py" )
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Calo_setOff()


#to read G4 POOL RDO
if GlobalFlags.InputFormat.is_pool():
    # to read pool Data
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )



    # G4 Pool input 
    # it is possible to specify a list of files to be processed consecutively
    # If using logical file name or using back navigation the relevant input
    # files should be listed in the PoolFileCatalog.xml
    EventSelector = Service( "EventSelector" )
    EventSelector.InputCollections = PoolRDOInput



# Set output level threshold
#    (  ALL, VERBOSE, DEBUG, INFO, WARNING, ERROR, FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel               = OutputLevel
#increase the number of letter reserved to the alg/tool name from 18 to 30
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M" 
# to change the default limit on number of message


# MessageSvc.debugLimit = 10000   # all debug message etc...
MessageSvc.defaultLimit = 9999999  # all messages


# Number of events to be processed 
theApp.EvtMax = EvtMax



theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

# Display detailed size and timing statistics for writing and reading
AthenaPoolCnvSvc = Service( "AthenaPoolCnvSvc" )
AthenaPoolCnvSvc.UseDetailChronoStat = True

#
# write out a short message upon entering or leaving each algorithm
#
if doNameAuditor:
    theAuditorSvc.Auditors  += [ "NameAuditor" ]


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from __main__ import topSequence


#other global variables (to be improved )
ALLOWBACKNAV=False
from __main__ import ALLOWBACKNAV
ALLOWIGNOREEXISTINGDATAOBJECT=False
from __main__ import ALLOWIGNOREEXISTINGDATAOBJECT
ALLOWDISABLE=False
from __main__ import ALLOWDISABLE
ALLOWIGNORECONFIGERROR=False
from __main__ import ALLOWIGNORECONFIGERROR


from RecExConfig.ObjKeyStore import *
objKeyStore = ObjKeyStore()


print " Initial content of objKeyStore "
objKeyStore.Print()

from __main__ import objKeyStore


# hack allowed here but not in standard config
ALLOWIGNOREEXISTINGDATAOBJECT=True
ALLOWDISABLE=True
ALLOWIGNORECONFIGERROR=True


# for example : disable SimpleRawObjGetter, this will cascade to
# downstream algorithm
if False:
    from RecExAlgs.SimpleRawObjGetter import SimpleRawObjGetter
    theSimpleRawObjGetter = SimpleRawObjGetter(disable=True)


# entering normal configuration, no hack alowed anymore
ALLOWIGNOREEXISTINGDATAOBJECT=False
ALLOWDISABLE=False
ALLOWIGNORECONFIGERROR=False

# for example :if try to disable here, this would be ineffective 
if False:
    from RecExAlgs.SimpleRawObjGetter import SimpleRawObjGetter
    theSimpleRawObjGetter = SimpleRawObjGetter(disable=True)




# put quasi empty first algorithm so that the first real
# algorithm does not see the memory change due to event manipulation
theApp.Dlls += [ "GaudiAlg"  ]
theApp.TopAlg += [ "EventCounter"  ]
EventCounter = Algorithm( "EventCounter" )
EventCounter.Frequency=100  # one print every 100 event

# other algorithms




# Print create SimpleCookedObjGetter, this will trigger SimpleRawObjGetter

from RecExAlgs.SimpleCookedObjGetter import *
aSimpleCookedObjGetter = SimpleCookedObjGetter()


print "Py: now trying new instance "

from RecExAlgs.SimpleRawObjGetter import *
bSimpleRawObjGetter = SimpleRawObjGetter()

if bSimpleRawObjGetter.usable():
    print "Py:top raw getFullName from b should be same instance ",bSimpleRawObjGetter.outputTypeKey()
    print "Py:top raw outputTypeKey in alg ",bSimpleRawObjGetter.SimpleRawObjMakerHandle().RawObjOutputName

# it is still possible to alter an alg property from there
# bSimpleRawObjGetter.SimpleRawObjMakerHandle().RawMessage=" from topOptions b"



theOutput=bSimpleRawObjGetter.output()
print theOutput





# set up all detector description description 
# include ("RecExCommon/AllDet_detDescr.py")



#
# G4 : RDO Pool converters
#
#if read or write ESD load the converters
if ( doWriteESD or readESD) or doWriteAOD or doWriteRDO :
    # Converters:
    include ( "RecExCommon/RecoOutputPoolCnv_jobOptions.py" )

if not readAOD and doTruth:        #needed to read MuonEntryRecord
    include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )


if DetFlags.readRDOPool.any_on() or DetFlags.readRIOPool.any_on() or readESD :
    if DetFlags.readRDOPool.ID_on():
    #FIXME tile reading very slow if InDet dict not there. Bug in Pool.     
        include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )

    if DetFlags.readRDOPool.LAr_on() :
        include( "LArAthenaPool/LArAthenaPool_joboptions.py" )

    if DetFlags.readRDOPool.Tile_on():
        include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )

    if DetFlags.readRDOPool.Muon_on() or DetFlags.readRIOPool.Muon_on()  :
    	include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
   

    include( "EventAthenaPool/EventAthenaPool_joboptions.py" )



if doTruth:
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )


        
print "List all Dlls"
print theApp.Dlls
print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg

print " Final content of objKeyStore "
objKeyStore.Print()


# exercise objKeyStore for fun
if objKeyStore.isInInput("bidon"):
    print "Py: in InPut :bidon"

if objKeyStore.isInTransient("bidon"):
    print "Py: bidon"

if objKeyStore.isInTransient("bidon","truc"):
    print "Py: bidon truc"

if objKeyStore.isInTransient("SimpleCookedObj"):
    print "Py: SimpleCookedObj"


if objKeyStore.isInTransient("SimpleCookedObj","truc"):
    print "Py: SimpleCookedObj truc"

if objKeyStore.isInTransient("SimpleCookedObj","SimpleCookedObjBlaBla"):
    print "Py: SimpleCookedObj SimpleCookedObjBlaBla"
    
