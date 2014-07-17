#--------------------------------------------------------------
# JobOptions to produce NN training samples for IBL. These are
# based on the standard IBL tracking jobOptions:
#      jobOptions_tracking_IBL.py 
#  - Tracking
#  - TrkValidationNtuple for NN training
#
# Author:   Tiago Perez <tperez@cern.ch>
# Date  :   2012-05-23
#--------------------------------------------------------------

#--------------------------------------------------------------
# Event related parameters and input files
#--------------------------------------------------------------

# --- read BS
doReadBS        = False

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

f = open('files.txt', 'r')
inputCollection=[]
for line in f:
    inputCollection.append(line)
inputCollection = [ l.strip() for l in inputCollection ]
#print "Input Collection : \n" + inputCollection

if not doReadBS:
  athenaCommonFlags.FilesInput = inputCollection

#  athenaCommonFlags.FilesInput = ['singleMuon_IBL_Digits.pool.root']

import AthenaPython.ConfigLib as apcl
cfg = apcl.AutoCfg(name = 'InDetIBLExampleAutoConfig', input_files=athenaCommonFlags.FilesInput())
cfg.configure_job()

if doReadBS:
  from AthenaCommon.GlobalFlags import globalflags
  if len(globalflags.ConditionsTag())!=0:
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

theApp.EvtMax = -1

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
doAuditors      = True

if os.environ['CMTCONFIG'].endswith('-dbg'):
  # --- do EDM monitor (debug mode only)
  doEdmMonitor    = True 
  # --- write out a short message upon entering or leaving each algorithm
  doNameAuditor   = True
else:
  doEdmMonitor    = False
  doNameAuditor   = False

#--------------------------------------------------------------
# Additional Detector setup
#--------------------------------------------------------------

# # --- select the geometry version to be IBL default 
# from AthenaCommon.GlobalFlags import globalflags
# globalflags.DetDescrVersion = 'ATLAS-IBL-01-00-00'
# 
# 
# from AthenaCommon.BeamFlags import jobproperties
# # --- default is high luminosity for IBL
# jobproperties.Beam.numberOfCollisions = 1.0

from AthenaCommon.DetFlags import DetFlags 
# --- switch on InnerDetector
DetFlags.ID_setOn()
# --- and switch off all the rest
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

#DetFlags.makeRIO.pixel_setOff()
#DetFlags.makeRIO.SCT_setOff()

#--------------------------------------------------------------
# Load InDet configuration
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags

# --- geoemetry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# --- setup InDetJobProperties
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doTruth = (globalflags.DataSource == 'geant4' and globalflags.InputFormat() == 'pool')
# --- Pixel cluster splitting does not work yet for IBL
if (hasattr(InDetFlags,"doPixelClusterSplitting")) :
  InDetFlags.doPixelClusterSplitting=False

InDetFlags.doTrkNtuple.set_Value_and_Lock(True)
InDetFlags.doTrkD3PD.set_Value_and_Lock(False) # Was True
# --- uncomment to change the default of one of the following options:
#OutputLevel          = VERBOSE

# --- Set output names such that they work with Reco_trf.py
from InDetRecExample.InDetKeys import InDetKeys
if athenaCommonFlags.PoolESDOutput(): InDetKeys.OutputESDFileName = athenaCommonFlags.PoolESDOutput()
if athenaCommonFlags.PoolAODOutput(): InDetKeys.OutputAODFileName = athenaCommonFlags.PoolAODOutput()
if InDetFlags.doTrkNtuple():          InDetKeys.trkValidationNtupleName = 'TrkValidation_IBL_J6_ROT_.root'
if InDetFlags.doTrkD3PD():            InDetKeys.trkD3PDFileName.set_Value_and_Lock('InDetTrackD3PD_IBL_J6.pool.root')


#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------
include("InDetIBL_Example/NewLayerMaterial.py")

include("InDetIBL_Example/InDetIBLRecoPreInclude.py")


#--------------------------------------------------------------
# load master joboptions file
#--------------------------------------------------------------

include("InDetRecExample/InDetRec_all.py")

#--------------------------------------------------------------
# try to access ntuple writer
#--------------------------------------------------------------
from InDetTrackValidation.InDetTrackValidationConf import InDet__PixelClusterValidationNtupleWriter
PixelNtupleWriter = InDet__PixelClusterValidationNtupleWriter(name                          = 'InDetPixelClusterValidationNtupleWriter',
                                                              NtupleFileName                = 'TRKVAL',
                                                              NtupleDirectoryName           = 'Validation',
#                                                              NtupleTreeName                = 'PixelROTs', ## not needed PixelRIOs is default. 
                                                              PixelClusterContainer         = InDetKeys.PixelClusters(),
                                                              WriteDetailedPixelInformation = True,
                                                              DoHits 		            = True,
                                                              OnlyClusterOnTrack            = True, ## For OnTrack ntuples
                                                              DoMC			    = True,
                                                              WriteNNTraining               = True,
                                                              # Extra flags ONLY ON PRIVATE InDetTrackValidation/PixelClusterValidationNtupleWriter
                                                              UseToT = True )

topSequence += PixelNtupleWriter
if (InDetFlags.doPrintConfigurables()):
  print PixelNtupleWriter 

#--------------------------------------------------------------
# configure IBL reco
#--------------------------------------------------------------

include("InDetIBL_Example/InDetIBLRecoPostInclude.py")

