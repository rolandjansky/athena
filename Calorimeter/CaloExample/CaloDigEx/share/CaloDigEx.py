# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=5
#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1']) 
jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc15a/mc15_valid.361034.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_low.merge.HITS.e3581_s2578_s2169_tid05098374_00/HITS.05098374._000241.pool.root.1']) 
jobproperties.AthenaCommonFlags.PoolRDOOutput.set_Value_and_Lock('CaloDigEx.pool.root')

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
# digitizationFlags.doMinimumBias=True
# digitizationFlags.numberOfCollisions=2.3
# digitizationFlags.minBiasInputCols=["", "", "" ]
# digitizationFlags.doCavern=True
# digitizationFlags.cavernInputCols=["",""]
#
digitizationFlags.overrideMetadata=['ALL']
digitizationFlags.IOVDbGlobalTag='OFLCOND-SDR-BS7T-05-14'
#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion='ATLAS-R2-2015-03-01-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

include("Digitization/Digitization.py")
