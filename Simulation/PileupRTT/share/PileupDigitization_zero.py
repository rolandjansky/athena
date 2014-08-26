
#**************************************************************
#
# Job Options For Pileup RDO Digitization
#      Adapted from Sven Vahsen's Digitization RTT job
#
# AUTHOR      : David W. Miller
# EMAIL       : David.W.Miller@cern.ch
# DATE        : 16 April 2007
# INSTITUTION : SLAC
#
#==============================================================

#--------------------------------------------------------------
# POOL Catalog Patch
#--------------------------------------------------------------

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr

import AthenaPoolCnvSvc.ReadAthenaPool
from PoolSvc.PoolSvcConf import PoolSvc

ServiceMgr += PoolSvc()
ServiceMgr.PoolSvc.AttemptCatalogPatch = True

#--------------------------------------------------------------
# Setup Permon performance monitoring
#--------------------------------------------------------------
#from PerfMonComps.PerfMonFlags import jobproperties
#jobproperties.PerfMonFlags.doMonitoring            = True
#jobproperties.PerfMonFlags.doPersistencyMonitoring = True
#jobproperties.PerfMonFlags.OutputFile              = "perfmon.root"

#--------------------------------------------------------------
# Setup Input and Output Files
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 50
athenaCommonFlags.PoolHitsInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11b/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1354_s1360/HITS.580376._000962.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="PileupDigiRTTOutput.RDO.pool.root"

#--------------------------------------------------------------
# Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias   = False
digitizationFlags.doCavern        = False
digitizationFlags.IOVDbGlobalTag='OFLCOND-SDR-BS7T-05-18'
digitizationFlags.overrideMetadata+=["SimLayout"]

#--------------------------------------------------------------
# Set some of the global flags. Like eg the DD version:
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-18-01-00'

# Flags that are defined in python are best set here
# switch off ID and muons

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )
