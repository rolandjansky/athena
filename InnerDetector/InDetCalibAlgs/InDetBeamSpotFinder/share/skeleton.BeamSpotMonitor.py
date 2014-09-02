#initial setup parameters
from AthenaCommon.Logging import logging
recoLog = logging.getLogger('esd_to_beamspotDPD')
recoLog.info( '****************** STARTING ESD->DPD MAKING *****************' )

if not 'runArgs' in dir():
    from PyJobTransformsCore.runargs import RunArguments
    runArgs = RunArguments()
from RecExConfig.RecFlags import rec

#rec.AutoConfiguration = ['everything']
runArgs.autoConfiguration = ['everything']
rec.AutoConfiguration = runArgs.autoConfiguration
print rec.AutoConfiguration
print rec
#print CCC

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
if hasattr(runArgs,"inputESDFile"):
    globalflags.InputFormat.set_Value_and_Lock('pool')
    rec.readESD.set_Value_and_Lock( True )
    rec.readRDO.set_Value_and_Lock( False )
    athenaCommonFlags.PoolESDInput.set_Value_and_Lock( runArgs.inputESDFile )
#else:
#    athenaCommonFlags.PoolESDInput=["/afs/cern.ch/user/j/jwalder/scratch0/data/firstData/data09_900GeV.00141749.express_express.merge.DESD_COLLCAND.f186_m305._lb0063-lb0082.1"]


if hasattr(runArgs,"outputDPDFile"):
    outputDPDname = runArgs.outputDPDFile
else:
    outputDPDname = "BeamspotDPD.pool.root"


if hasattr(runArgs,"maxEvents"):  
    athenaCommonFlags.EvtMax.set_Value_and_Lock( runArgs.maxEvents )
    EvtMax = runArgs.maxEvents

if hasattr(runArgs,"conditionsTag"):
    if len(runArgs.conditionsTag) >0 :
        globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.conditionsTag)
        ConditionsTag = runArgs.conditionsTag


from RecExConfig.InputFilePeeker import inputFileSummary
print inputFileSummary
#if 'metadata' in inputFileSummary:
#    if '/EXT/DCS/MAGNETS/SENSORDATA' in inputFileSummary['metadata']:
#        for i in  inputFileSummary['metadata']['/EXT/DCS/MAGNETS/SENSORDATA']:
#            print i['value']


        
if 'beam_energy' in inputFileSummary:
    print "JW Beam", inputFileSummary['beam_energy']
else:
    print "JW Beam: None"
if 'beam_type' in inputFileSummary:
    print "JW Type: ", inputFileSummary['beam_type']
else:
    print "JW Type: None"

geoFilePeeker=''

if 'geometry' in inputFileSummary: 
    if inputFileSummary['geometry'] != None:
        geoFilePeeker = inputFileSummary['geometry']
        print "JWX", geoFilePeeker 
#print inputFileSummary['evt_type']
if 'evt_type' in inputFileSummary:
    if 'IS_SIMULATION' in inputFileSummary['evt_type']:
      print "is sumiulation" 
#print JW2 

    
if hasattr(runArgs,"geometryVersion"):
    if len(runArgs.geometryVersion) >0 :
        globalflags.DetDescrVersion.set_Value_and_Lock(runArgs.geometryVersion)
        DetDescrVersion = runArgs.geometryVersion
    elif len( geoFilePeeker) > 0:
        globalflags.DetDescrVersion.set_Value_and_Lock(geoFilePeeker)
        DetDescrVersion = geoFilePeeker
else:
    globalflags.DetDescrVersion = "ATLAS-GEO-08-00-02"
    DetDescrVersion = globalflags.DetDescrVersion


import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = athenaCommonFlags.PoolESDInput()
import AthenaCommon.SystemOfUnits as Units
globalflags.DetGeo = 'atlas'

if 'evt_type' in inputFileSummary:
    if 'IS_SIMULATION' in inputFileSummary['evt_type']:
        globalflags.DataSource = 'geant4'
    else:
        globalflags.DataSource = 'data'
else:        
    globalflags.DataSource = 'data'


# Include flags and scripts for RecExCommon

from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

#include the fragment that makes the dpd files
include("InDetBeamSpotFinder/DPD_BeamSpotFragment.py")

jobConfig = {}
jobConfig['inputfiles'] = runArgs.inputESDFile
jobConfig['bytestream'] = False
jobConfig['evtmax'] = runArgs.maxEvents
jobConfig['outputlevel'] = WARNING
jobConfig['beamspotfile'] = runArgs.beamposFile
jobConfig['beamspotdb'] = runArgs.beamspotDb
jobConfig['beamspottag'] = runArgs.beamposTag
jobConfig['alignmentfile'] = ''
jobConfig['alignmentindettag'] = runArgs.indetAlignTag
jobConfig['alignmenttrttag']   = runArgs.trtAlignTag
jobConfig['GlobalTag'] = runArgs.conditionsTag
jobConfig['DetDescrVersion'] = globalflags.DetDescrVersion()
jobConfig['doTruth'] = False
jobConfig['doPrintIndetConfig'] = False
jobConfig['doMonitoring'] = True
jobConfig['DataSource'] = globalflags.DataSource()
jobConfig['alignmentfile'] = runArgs.indetAlginFile
# include("InDetBeamSpotExample/JobSetupFragment.py")

#include('InDetBeamSpotExample/900GeV-FirstData-BSToDPDTemplate.py')
#include('900GeV-FirstData-ESDToDPDTemplate.py')

doWriteAOD = False
doWriteESD = False

rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.useBeamConstraint.set_Value_and_Lock(False)



if hasattr(runArgs,"InDetBeamSpotExampleJobOption"):
    if len(runArgs.InDetBeamSpotExampleJobOption) >0 :
        print "Including: JobOption", runArgs.InDetBeamSpotExampleJobOption
        for i in runArgs.InDetBeamSpotExampleJobOption:
            include(i)
    else:
        print "No jobOption specified"
else:
    print "No jobOption specified"


