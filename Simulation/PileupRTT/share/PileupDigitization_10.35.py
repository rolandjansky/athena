
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
athenaCommonFlags.EvtMax = 5
athenaCommonFlags.PoolHitsInput=["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1"]
                                              
athenaCommonFlags.PoolRDOOutput="PileupDigiRTTOutput.RDO.pool.root"

#--------------------------------------------------------------
# Pileup bunch spacing and integration configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-SDR-BS7T-04-13'
digitizationFlags.bunchSpacing         = 25
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing   = 32

#--------------------------------------------------------------
# Minimum bias configuration
#--------------------------------------------------------------
digitizationFlags.doLowPtMinBias        = True
digitizationFlags.numberOfLowPtMinBias   = 230.0
include("Digitization/mc10minbiasfiles.py")

#--------------------------------------------------------------
# Cavern background configuration
#--------------------------------------------------------------                                              
digitizationFlags.doCavern             = True
digitizationFlags.numberOfCavern       = 230                                              
include("Digitization/mc10cavernfiles.py")

#--------------------------------------------------------------
# Beam gas and beam halo configuration
#--------------------------------------------------------------                                              
digitizationFlags.doBeamGas            = False
digitizationFlags.numberOfBeamGas      = 0.003
digitizationFlags.beamGasInputCols = ["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108863.Hijing_beamgas.merge.HITS.e4_s950_s952_tid170553_00/HITS.170552._000087.pool.root.1",
                                               "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108863.Hijing_beamgas.merge.HITS.e4_s950_s952_tid170553_00/HITS.170552._000091.pool.root.1"]

digitizationFlags.doBeamHalo           = False
digitizationFlags.numberOfBeamHalo     = 0.05                                              
digitizationFlags.beamHaloInputCols = ["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108852.BeamHaloInputs.merge.HITS.e4_e567_s949_s952_tid170552_00/HITS.170552._000001.pool.root.1",
                                                "root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.108852.BeamHaloInputs.merge.HITS.e4_e567_s949_s952_tid170552_00/HITS.170552._000020.pool.root.1"]

#--------------------------------------------------------------
# Set some of the global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-GEO-16-00-00'

# Flags that are defined in python are best set here
# switch off ID and muons

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )

