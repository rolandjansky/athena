
#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax=50
athenaCommonFlags.PoolHitsInput=["/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005122.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005207.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005259.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005319.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005366.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005435.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005453.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005555.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005595.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005607.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005619.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005656.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005667.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005717.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005745.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005758.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005838.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005902.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005921.pool.root.1", "/afs/cern.ch/user/t/tsfiligo/user.tsfiligo.mc12_8TeV.107206.ParticleGenerator_mu_Pt5.merge.HITS.e1717_s1609_s1611_tid01138936_00_der1382111116/HITS.01138936._005976.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC12-SDR-06'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

DetFlags.TRT_setOn()

include("Digitization/Digitization.py")


StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
# Setup the algorithm and output sequences
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.TRTFastDigitization.TRTFastDigitizationTool.OutputLevel=VERBOSE
from AthenaCommon.AppMgr import theApp
outStream = theApp.getOutputStream( "StreamRDO" )
outStream.ItemList += [ "InDet::TRT_DriftCircleContainer#*" ]
