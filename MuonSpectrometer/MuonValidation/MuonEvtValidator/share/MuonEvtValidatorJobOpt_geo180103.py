#==============================================================
# Job options file for MuonEvtValidatior
# ATLAS-GEO-02-00-00 Layout
# muminus_pt_100GeV
# Nov. 2008
# Nektarios Chr. Benekos - Illinois
#==============================================================
from AthenaCommon.DetFlags import DetFlags 
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ServiceMgr
 
#--------------------------------------------------------------
# Control - standard options (as in jobOptions.py)
#--------------------------------------------------------------
 
# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = INFO
# --- produce an atlantis data file
doJiveXML       = False
# --- run the Virtual Point 1 event visualisation
doVP1           = False
doAuditors      = True
doNameAuditor   = True
doTimeLimit     = True
#--------------------------------------------------------------
import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False 
#-------------------------------------------------------------- 
# Private Application Configuration options
#--------------------------------------------------------------
#------------------------------------------------------------
# GeoModel Initialization
#--- GeoModel stuff -----------------------------------------
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
from AtlasGeoModel import GeoModelInit
#Set geometry tag
from AtlasGeoModel import SetGeometryVersion, GeoModelInit
GeoModelSvc = ServiceMgr.GeoModelSvc
GeoModelSvc.IgnoreTagDifference = True 

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MuonEvtValidator.MuonEvtValidatorConf import MVCombined
MuonEvtVal = MVCombined("MuonEvtVal")
#MuonEvtVal.HistogramNameVersion1 = "RLayout"
#MuonEvtVal.HistogramNameVersion2 = "RppLayout"
MuonEvtVal.HistogramNameVersion1 = "ATLAS-GEO-18-01-03_1"  
MuonEvtVal.HistogramNameVersion2 = "ATLAS-GEO-18-01-03_2"

#--------------------------------------------------------------
# Declare here the first File
# Specify which type is it: Digits or Hits
MuonEvtVal.FileTyp1 = "Hits" 			
# Specify the the Inputname of the First File
MuonEvtVal.InputVersion1File = "MuonHitTest_GEO-18-01-03.Hits.aan.root"	
#-------------------------------------------------------------- 
# Declare here the second File
# Specify which type is it: Digits or Hits
MuonEvtVal.FileTyp2 = "Hits"				
# Specify the the Inputname of the second File 
MuonEvtVal.InputVersion2File = "MuonHitTest_GEO-18-01-03.Hits.aan.root"	
#--------------------------------------------------------------
MuonEvtVal.ExcludeDoubleCounting = True
MuonEvtVal.MinimalDistanceForHitIdentification = 0

# Igonre this for a while
#MuonEvtVal.SpecialStation_NameID = 1
#MuonEvtVal.SpecialStation_Eta = 1
#MuonEvtVal.SpecialStation_Phi = 1
#--------------------------------------------------------------
MuonEvtVal.OutputFile = "MuonEvtValidator_geo180103_muminus_pt10-50.root"
#--------------------------------------------------------------
topSequence += MuonEvtVal
#-------------------------------------------------------------- 
###############################################################
# End of user settings
###############################################################
#--------------------------------------------------------------
#'ATLAS-GEO-02-00-00'    --> No solenoid field, corrected EIL chamber positions in Muon. Ideal layout, no misalignments
globalflags.DetDescrVersion = "ATLAS-GEO-18-01-03" 
###############################################################
# End of user settings
###############################################################
 
# for backwards compatibility
DetDescrVersion = globalflags.DetDescrVersion()
GeoModelSvc.AtlasVersion = DetDescrVersion  
#--------------------------------------------------------------
# load Global Flags and set defaults (import the new jobProperty globalflags)
#--------------------------------------------------------------
 
# --- default is atlas geometry
globalflags.DetGeo = 'atlas'
print 'DetDescrVersion = %r' % globalflags.DetDescrVersion()
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
if doAuditors:
  theApp.Dlls += [ "GaudiAud" ]
  # load AuditorSvc
  # write out a summary of the time spent
  from GaudiSvc.GaudiSvcConf import AuditorSvc
  ServiceMgr += AuditorSvc()
  theAuditorSvc = ServiceMgr.AuditorSvc
  #theAuditorSvc = AuditorSvc()
  # write out summary of the memory usage
  #   number of events to be skip to detect memory leak
  #   20 is default. May need to be made larger for complete jobs.
  theAuditorSvc.Auditors += [ "ChronoAuditor"]
  ServiceMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 5
  # memory auditors
  theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
  MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
  MemStatAuditor.OutputLevel = WARNING
  
# write out a short message upon entering or leaving each algorithm
if doNameAuditor:
  theAuditorSvc.Auditors     += [ "NameAuditor" ]
  theApp.AuditAlgorithms = True
  theApp.AuditServices   = True

from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
ServiceMgr += AthenaPoolCnvSvc()
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True
#-------------------------------------------------------------- 
ServiceMgr.StoreGateSvc.Dump = False  #true will dump data store contents
#-------------------------------------------------------------- 
#-------------------------------------------------------------- 
# Number of events to be processed (default is 10)
theApp.EvtMax = 1
#theApp.EvtSel = "EventSelector"
#-------------------------------------------------------------- 
# Root Ntuple output file and name
#--------------------------------------------------------------
# Histograms
#
theApp.Dlls += ["HbookCnv"]
theApp.HistogramPersistency = "ROOT" 
from GaudiSvc.GaudiSvcConf import HistogramPersistencySvc
ServiceMgr += HistogramPersistencySvc()
ServiceMgr.HistogramPersistencySvc.OutputFile  = MuonEvtVal.OutputFile
#--------------------------------------------------------------------------- 
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------------------- 
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.defaultLimit = 9999999
ServiceMgr.MessageSvc.useColors = True
#--------------------------------------------------------------
# Application: load TagInfoMgr
#--------------------------------------------------------------
import EventInfoMgt.EventInfoMgtInit
#--------------------------------------------------------------
# Dump configuration to file
#--------------------------------------------------------------
from AthenaCommon.ConfigurationShelve import saveToAscii
saveToAscii("config.txt") 
#--------------------------------------------------------------
print "List of all top algorithms"
print theApp.TopAlg
#================================================================


#--- End MuonEvtValidatorJobOpt.py file  ----------------------
 
