


from AthenaCommon.Logging import logging

#logRecExCommon_topOptions = logging.getLogger( 'py:RecExCommon_topOptions' )

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


# put quasi empty first algorithm so that the first real
# algorithm does not see the memory change due to event manipulation
theApp.Dlls += [ "GaudiAlg"  ]
theApp.TopAlg += [ "EventCounter"  ]
EventCounter = Algorithm( "EventCounter" )
EventCounter.Frequency=100  # one print every 100 event

# other algorithms

include ("RecExAlgs/SimpleRawObjMaker_jobOptions.py")
include ("RecExAlgs/SimpleCookedObjMaker_jobOptions.py")








# set up all detector description description 
include ("RecExCommon/AllDet_detDescr.py")



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

