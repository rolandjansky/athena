#--------------------------------------------------------------
# Control
#--------------------------------------------------------------

# --- Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
OutputLevel     = DEBUG
#OutputLevel     = INFO
#OutputLevel     = ERROR

# --- read or write BS
doReadBS        = True
#doReadBS        = False
doCopyRDO       = False
doWriteESD      = doReadBS
doWriteAOD      = False
doWriteBS       = not doReadBS
# --- do auditors ?
doAuditors      = False

import os
if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False




# --- setup InDetFlags according to InDetFlagsJobProperty FIXME do I need that?
include( "InDetRawDataByteStreamCnv/PixelFlags.py")
InDetFlags = ConfiguredInDetFlags()


#--------------------------------------------------------------
# detector description version
#--------------------------------------------------------------

DetDescrVersion = "ATLAS-CSC-02-00-00"

#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRawDataByteStreamCnv/Pixel_main.py")

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# Number of events to be processed (default is 10)
theApp.EvtMax = 1000
#ServiceMgr.EventSelector.SkipEvents = 2

if not doReadBS:
#  from PoolSvc.PoolSvcConf import PoolSvc
 # ServiceMgr += PoolSvc()
  
 # ServiceMgr.PoolSvc.AttemptCatalogPatch=True
  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-DC3-07")
  # ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d38/ID_ValidationData/testIdeal_07.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000201_tid002760._00002.pool.root.1"]
  # --- TP split version (local) - (DetDescrVersion = "ATLAS-DC3-07")
  #ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d89/InDetRecRDO.root"]
  # --- t tbar channel 5200 (DetDescrVersion = "ATLAS-CSC-02-00-00")

  
  
  ServiceMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/maxidisk/d158/CSC.005200.T1_McAtNlo_Jimmy.RDO.pool.root" ]
  # --- gamma 20 (ATLAS-CSC-01-00-00)
  #ServiceMgr.EventSelector.InputCollections = ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00001.pool.root"]
  #ServiceMgr.EventSelector.InputCollections += ["castor:/castor/cern.ch/grid/atlas/datafiles/egamma/DC3.007040.singlepart_gamma_Et20/digit/120031/ideal0_mc12.007040.singlepart_gamma_Et20.digit.RDO.v12003108_tid005022._00002.pool.root"]
if doReadBS:
  #ServiceMgr.EventSelector.Input = [ "rfio:/castor/cern.ch/user/w/wildauer/calib0.007003.singlepart_e_Et25.digit.RDO.v12003101_tid003425._00001.bs.data" ] 
  ServiceMgr.EventSelector.Input = [ "RawEvent.re" ] 
  ServiceMgr.ByteStreamInputSvc.NumFile      = [1] # this might be the same as SkipEvents?? useless if it just specifies the number of input files ...
