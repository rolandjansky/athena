# ===========================================================================================================================
#
# Example jobOptions to perform LAr pileup starting from merged bunch crossing
#   Pileup is done with one "super event" per bunch crossing (fixed) (produced from merging of n subevets)
#   jobproperties.Digitization.numberOfCollisions should still correspond to the real luminosity such that OFC optimization is done
#     correctly
#
# ==========================================================================================================================
#
#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=10
jobproperties.AthenaCommonFlags.PoolHitsInput=["rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/proj-sit/digitization/RTT/mc08/mc08.105200.T1_McAtNlo_Jimmy.simul.HITS.e357_s462/HITS.025431._02899.pool.root.1"]
jobproperties.AthenaCommonFlags.PoolRDOOutput="DigitizationOutput_newMerge_23.pool.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
jobproperties.Digitization.doMinimumBias=True
jobproperties.Digitization.doCavern=False
jobproperties.Digitization.doBeamGas=False
jobproperties.Digitization.doBeamHalo=False

jobproperties.Digitization.bunchSpacing=25
jobproperties.Digitization.numberOfCollisions=23.
jobproperties.Digitization.initialBunchCrossing=-32 
jobproperties.Digitization.finalBunchCrossing=5 

jobproperties.Digitization.minBiasInputCols=["mergedHits_23.pool.root"]

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-02-01-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Tile_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )

minBiasCache.CollDistribution="Fixed"
minBiasCache.CollPerXing=1
