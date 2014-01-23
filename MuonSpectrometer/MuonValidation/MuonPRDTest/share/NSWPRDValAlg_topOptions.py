#-----------------------------------------------------------------------------
# Athena imports
#-----------------------------------------------------------------------------
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-GEO-20-00-01'
# USE same version as jobOptions.NSW_Sim.py
globalflags.ConditionsTag = 'OFLCOND-SDR-BS7T-05-14'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()

DetFlags.Muon_setOn()
DetFlags.MDT_setOn()
DetFlags.CSC_setOn()
DetFlags.TGC_setOn()
DetFlags.RPC_setOn()
DetFlags.sTGC_setOn()
DetFlags.Micromegas_setOn()


DetFlags.digitize.MDT_setOn()
DetFlags.digitize.TGC_setOn()
DetFlags.digitize.RPC_setOn()
DetFlags.digitize.CSC_setOn()
DetFlags.digitize.Micromegas_setOff()
DetFlags.digitize.sTGC_setOff()

#DetFlags.Lucid_setOff()
DetFlags.Truth_setOn()
#DetFlags.LVL1_setOff()

include('MuonGeoModelTest/NSWGeoSetup.py')


#-----------------------------------------------------------------------------
# Message Service
#-----------------------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 9999999

#-----------------------------------------------------------------------------
# Input Datasets
#-----------------------------------------------------------------------------
ServiceMgr.EventSelector.InputCollections = [
     'atlasG4.hits.pool.root'
    ]
theApp.EvtMax = -1 # -1 means all events

#-----------------------------------------------------------------------------
# Algorithms
#-----------------------------------------------------------------------------
   #if DetFlags.Micromegas_on() and DetFlags.digitize.Micromegas_on():    
from MuonFastDigitization.MuonFastDigitizationConf import MM_FastDigitizer
job += MM_FastDigitizer("MM_FastDigitizer")

   #if DetFlags.sTGC_on() and DetFlags.digitize.sTGC_on():    
from MuonFastDigitization.MuonFastDigitizationConf import sTgcFastDigitizer
job += sTgcFastDigitizer("sTgcFastDigitizer")


from MuonPRDTest.MuonPRDTestConf import *

job += NSWPRDValAlg('NSWPRDValAlg', OutputLevel = WARNING)
NSWPRDValAlg.OutputLevel = DEBUG
NSWPRDValAlg.doTruth = True
NSWPRDValAlg.doMMDigit = False
#NSWPRDValAlg.doMM = False

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "NSWPRDValAlg DATAFILE='NSWPRDValAlg_allDet.root' OPT='RECREATE'" ]
