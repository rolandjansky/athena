#--------------------------------------------------------------
#
#
# InDetExample driving joboptions
#
#
#--------------------------------------------------------------

#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
# --- set defaults
globalflags.DataSource = 'geant4'
if 'doReadBS' in dir() and doReadBS:
  globalflags.InputFormat = 'bytestream'
else:
  globalflags.InputFormat = 'pool'
# --- default is zero luminosity
globalflags.Luminosity      = 'zero'
# --- set geometry version
globalflags.DetDescrVersion = DetDescrVersion
# --- printout
globalflags.print_JobProperties()

#--------------------------------------------------------------
# Set Detector setup
#--------------------------------------------------------------

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# ---- switch parts of ID off/on as follows
#DetFlags.makeRIO.pixel_setOff()
DetFlags.makeRIO.SCT_setOff()
DetFlags.makeRIO.TRT_setOff()
# --- printout
DetFlags.Print()

#--------------------------------------------------------------
# load ServiceMgr, ToolSvc, topSequence
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
#
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#
from AthenaCommon.AppMgr import ToolSvc

#--------------------------------------------------------------
# change MessageSvc 
#--------------------------------------------------------------

# output level
ServiceMgr.MessageSvc.OutputLevel  = OutputLevel
# increase the number of letter reserved to the alg/tool name from 18 to 30
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# Load POOL support, setup for reconstruction
#--------------------------------------------------------------

# --- setup POOL access in ATHENA
if 'doReadBS' in dir() and doReadBS:
  include("InDetRawDataByteStremCnv/PixelReadBS_jobOptions.py")
else:
  import AthenaPoolCnvSvc.ReadAthenaPool
# --- GeoModel
include("AtlasGeoModel/SetGeometryVersion.py" )
include("AtlasGeoModel/GeoModelInit.py" )
# ---- Beam Spot service
if InDetFlags.useBeamConstraint():
  include("InDetBeamSpotService/BeamCondSvc.py")
# --- need MagneticField
include("BFieldAth/BFieldAth_jobOptions.py")
# --- particle property service
include("PartPropSvc/PartPropSvc.py")

#------------------------------------------------------------
# Event Data Model Monitor
#------------------------------------------------------------

if doEdmMonitor:
  from TrkValAlgs.TrkValAlgsConf import Trk__EventDataModelMonitor
  InDetEdmMonitor = Trk__EventDataModelMonitor (name = 'InDetEdmMonitor')
  topSequence += InDetEdmMonitor
  print          InDetEdmMonitor


#--------------------------------------------------------------
# Load Inner Detector reconstruction
#--------------------------------------------------------------

if not ('doWriteBS' in dir() and doWriteBS):
  include( "InDetRawDataByteStreamCnv/PixelRec_jobOptions.py" )

# ------------------------------------------------------------
# switch off history service and no dump
# ------------------------------------------------------------

ServiceMgr.StoreGateSvc.ActivateHistory = False
ServiceMgr.StoreGateSvc.Dump            = False

# ------------------------------------------------------------
# Use auditors
# ------------------------------------------------------------

if doAuditors:
  # --- load AuditorSvc
  from AthenaCommon.ConfigurableDb import getConfigurable
  # --- write out summary of the memory usage
  #   | number of events to be skip to detect memory leak
  #   | 20 is default. May need to be made larger for complete jobs.
  ServiceMgr.AuditorSvc += getConfigurable("ChronoAuditor")()
  # --- memory auditors
  MemStatAuditor = getConfigurable("MemStatAuditor")()
  MemStatAuditor.OutputLevel = WARNING
  ServiceMgr.AuditorSvc += MemStatAuditor
  # --- write out a short message upon entering or leaving each algorithm
  if doNameAuditor:
    ServiceMgr.AuditorSvc += getConfigurable("NameAuditor")()
  #
  theApp.AuditAlgorithms = True
  theApp.AuditServices   = True
  #  
  # --- Display detailed size and timing statistics for writing and reading
  if doWriteESD or doWriteAOD or ('doCopyRDO' in dir() and doCopyRDO):
    ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

# ------------------------------------------------------------
# write BS 
# ------------------------------------------------------------

if 'doWriteBS' in dir() and doWriteBS:
  # --- load writing BS file
  include ("InDetRawDataByteStreamCnv/PixelWriteBS_jobOptions.py")
  
  
# ------------------------------------------------------------
# persistency
# ------------------------------------------------------------

if doWriteESD or doWriteAOD or ('doCopyRDO' in dir() and doCopyRDO):
  # --- load setup
  from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
  # --- check dictionary
  ServiceMgr.AthenaSealSvc.CheckDictionary = True
  
  if doWriteESD:
    # --- create stream
    StreamESD            = AthenaPoolOutputStream ( "StreamESD" )
    StreamESD.OutputFile = "InDetRecESD.root"
    # --- save MC collection if truth turned on
    if InDetFlags.doTruth():
      StreamESD.ItemList += ["McEventCollection#*"]
    # ---- load list of objects
    include ( "InDetRecExample/WriteInDetESD.py" )
      
  if doWriteAOD:
    # --- create stream
    StreamAOD            = AthenaPoolOutputStream ( "StreamAOD" )
    StreamAOD.OutputFile = "InDetRecAOD.root"
    # --- save MC collection if truth turned on
    if InDetFlags.doTruth():
      StreamAOD.ItemList += ["McEventCollection#*"]
    # --- load list of objects
    include ( "InDetRecExample/WriteInDetAOD.py" )
  
  if 'doCopyRDO' in dir() and doCopyRDO:
    # --- create stream
    StreamRDO            = AthenaPoolOutputStream ( "StreamRDO" )
    StreamRDO.OutputFile = "InDetRecRDO.root"
    # --- add output to list
    StreamRDO.ItemList   +=  ['TRT_RDO_Container#*','SCT_RDO_Container#*','PixelRDO_Container#*']
    StreamRDO.ItemList   +=  ['InDetSimDataCollection#*','McEventCollection#*']
    # --- Force read
