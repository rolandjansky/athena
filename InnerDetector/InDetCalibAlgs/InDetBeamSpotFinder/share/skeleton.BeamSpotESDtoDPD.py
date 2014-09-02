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
#primDPD.WriteSkimDecisions = True

#rec.doCBNT       = False
#rec.doWriteESD   = False
#rec.doWriteAOD   = False
#rec.doAOD        = False
#rec.doWriteTAG   = False
#rec.doPerfMon    = False
#rec.doHist       = False
#rec.doTruth      = False

#rec.readAOD        = False
#rec.doDPD          = True
#rec.doFileMetaData = True
#rec.readESD        = True
#rec.readRDO        = False


# InDetRecExample code 

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- read BS - please look at the default input file(s) to know the det descr and cond tag
doReadBS        = False
# --- do auditors ?
doAuditors      = True
doEdmMonitor    = False
doNameAuditor   = False

from InDetRecExample.InDetJobProperties import InDetFlags
from AthenaCommon.BeamFlags import jobproperties

InDetFlags.doMonitoringGlobal    = True
InDetFlags.doMonitoringPixel     = False
InDetFlags.doMonitoringSCT       = False
InDetFlags.doMonitoringTRT       = False
InDetFlags.doMonitoringAlignment = False

InDetFlags.doTruth = False

InDetFlags.useBeamConstraint.set_Value_and_Lock(False)

InDetFlags.primaryVertexCutSetup = "StartUp"
#InDetFlags.useHVForSctDCS=True
InDetFlags.doPrintConfigurables = True

# --- possibility to change the trackfitter
#InDetFlags.trackFitterType = 'KalmanFitter'


#rec.DPDMakerScripts.append("BPhysAODtoDAODOptions.py")
from AthenaCommon.DetFlags import DetFlags
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.Print()

#include ("RecExCommon/RecExCommon_topOptions.py")

include('InDetRecExample/InDetRec_all.py')

ToolSvc = Service('ToolSvc')

#include('InDetBeamSpotFinder/AddtionalVertexers.py')


print rec

from AthenaCommon.BFieldFlags import jobproperties

if jobproperties.BField.solenoidOn():
    print "JW: Solenoid On"
else:
    print "JW: Solenoid Off"

#print bprop
print "JS"
print jobproperties
print "JW Det:"
print dir(DetFlags.dcs)
print "JW: Pixel:", DetFlags.dcs.pixel_on()
print "JW: SCT: ",  DetFlags.dcs.SCT_on()
print "JW: TRT: ",  DetFlags.dcs.TRT_on()
print "JW: Commissioning: ",rec.Commissioning
#print runArgs
#print XXX
print rec
#print rec.Global.DataSource
#print inputFileSummary['datasource'] 


#Vertexing cuts
###ToolSvc.InDetDetailedTrackSelectorTool.nHitPix = 0
###ToolSvc.InDetDetailedTrackSelectorTool.nHitSct = 4
###ToolSvc.InDetDetailedTrackSelectorTool.nHitSi = 0
#ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 10
#ToolSvc.InDetDetailedTrackSelectorTool.nHitTrt = 0
###ToolSvc.InDetDetailedTrackSelectorTool.pTMin = 0.* Units.MeV
#ToolSvc.InDetDetailedTrackSelectorTool.useTrackQualityInfo = False
#ToolSvc.InDetDetailedTrackSelectorTool.IPd0Max = 5
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPd0Max = 5.0
#ToolSvc.InDetDetailedTrackSelectorTool.d0significanceMax = -1
#ToolSvc.InDetDetailedTrackSelectorTool.sigIPz0Max = 10.0


from IOVDbSvc.CondDB import conddb

if hasattr(runArgs,"indetAlignTag"):
    if len(runArgs.indetAlignTag) >0 :
        conddb.addOverride("/Indet/Align", runArgs.indetAlignTag) #InDet_Collision_2009_02
if hasattr(runArgs,"trtAlignTag"):
    if len(runArgs.trtAlignTag) >0 :
        conddb.addOverride("/TRT/Align",   runArgs.trtAlignTag) #TRT_Collision_2009_01
if hasattr(runArgs,"indetTrkErrScTag"):
    if len(runArgs.indetTrkErrScTag) >0 :
        conddb.addOverride("/Indet/TrkErrorScaling",runArgs.indetTrkErrScTag)
if hasattr(runArgs,"beamposTag"):
    if len(runArgs.beamposTag) >0 :
        conddb.addOverride("Indet/Beampos",runArgs.beamposTag) #

globalflags.print_JobProperties()

#include the fragment that makes the dpd files
include("InDetBeamSpotFinder/DPD_BeamSpotFragment.py")

