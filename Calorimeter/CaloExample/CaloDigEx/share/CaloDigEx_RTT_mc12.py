# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=30

#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['Sim_mc12.pool.root'])

jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['root://eosatlas.cern.ch//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc12a/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.simul.HITS.e1193_s1469/HITS.782686._012893.pool.root.1'])
jobproperties.AthenaCommonFlags.PoolRDOOutput.set_Value_and_Lock('CaloDigEx_mc12a.pool.root')

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
# jobproperties.Digitization.doMinimumBias=True
# jobproperties.Digitization.numberOfCollisions=2.3
# jobproperties.Digitization.minBiasInputCols=["", "", "" ]
# jobproperties.Digitization.doCavern=True
# jobproperties.Digitization.cavernInputCols=["",""]

 
#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags

globalflags.DetDescrVersion='ATLAS-R1-2012-02-01-00'
globalflags.ConditionsTag='OFLCOND-MC12-SDR-06'

#from G4AtlasApps.SimFlags import SimFlags 
#SimFlags.SimLayout='ATLAS-GEO-20-00-01'

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

svcMgr.GeoModelSvc.IgnoreTagDifference = True




