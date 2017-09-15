#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
# --- controls what is written out. ESD includes AOD, so it's normally enough
doWriteESD      = False
doWriteAOD      = False
# --- do auditors ?
doAuditors      = False
# --- do EDM monitor (debug mode only)
doEdmMonitor    = False 
# --- write out a short message upon entering or leaving each algorithm
doNameAuditor   = False

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

# detector description version
#if not 'DetDescrVersion' in dir(): 
#  DetDescrVersion = "ATLAS-CSC-02-00-00"

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
globalflags.InputFormat = 'pool'
# --- set geometry version
#globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

#from AthenaCommon.BeamFlags import jobproperties
# --- default is zero luminosity
#jobproperties.Beam.numberOfCollisions = 0.0  
#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------
#from RecExConfig.RecFlags import rec
#rec.Commissioning=False

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth       = (globalflags.InputFormat() == 'pool')

# --- activate non default algorithms
InDetFlags.doTrkNtuple = True
InDetFlags.doPixelTrkNtuple = True
InDetFlags.doMonitoringPixel = True
InDetFlags.doPrintConfigurables = True

# IMPORTANT NOTE: initialization of the flags and locking them is done in InDetRec_jobOptions.py!
# This way RecExCommon just needs to import the properties without doing anything else!
# DO NOT SET JOBPROPERTIES AFTER THIS LINE! The change will be ignored!
#--------------------------------------------------------------
# load jobOptions fragments:
# 
# master joboptions file
# Christoph's noisy maps
#
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")
#include("PixelCalibAlgs/NoiseMapBuilder.py")
#from AthenaCommon.AlgSequence import AlgSequence
#topSequence = AlgSequence()
#from PixelCalibAlgs.PixelCalibAlgsConf import NoiseMapBuilder
#topSequence += NoiseMapBuilder()

#--------------------------------------------------------------
# Event number and input files
#--------------------------------------------------------------

if not 'EventMax' in dir():
  EventMax = 10
  
theApp.EvtMax = EventMax

# Name of the collection to reconstruct
DatasetDir = "/castor/cern.ch/atlas/atlascerngroupdisk/perf-idtracking/rtt/mc10_7TeV.105200.T1_McAtNlo_Jimmy.digit.RDO.e598_s933_s946_d366/"

import string
filelist = os.popen("rfdir "+ DatasetDir).readlines()
collection = []
for i in filelist:
  collection.append("root://castoratlas/"+ DatasetDir+string.split(i)[8])
ServiceMgr.PoolSvc.AttemptCatalogPatch=True
ServiceMgr.EventSelector.InputCollections = collection
