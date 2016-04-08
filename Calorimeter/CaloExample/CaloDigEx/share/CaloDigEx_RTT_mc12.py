# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=12

#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['Sim_mc12.pool.root'])

jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloDigEx_RTT_infiles/mc12/mc12_8TeV.105200.McAtNloJimmy_CT10_ttbar_LeptonFilter.merge.HITS.e1513_s1499_s1504_tid01012667_00/HITS.01012667._014280.pool.root.1'])
jobproperties.AthenaCommonFlags.PoolRDOOutput.set_Value_and_Lock('CaloDigEx.pool.root')

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
#from AthenaCommon.GlobalFlags import jobproperties

jobproperties.Global.DetDescrVersion='ATLAS-GEO-20-00-01'
jobproperties.Global.ConditionsTag='OFLCOND-MC12-SDR-06'

from G4AtlasApps.SimFlags import SimFlags 
SimFlags.SimLayout='ATLAS-GEO-20-00-01'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOn()


MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

include("Digitization/Digitization.py")





