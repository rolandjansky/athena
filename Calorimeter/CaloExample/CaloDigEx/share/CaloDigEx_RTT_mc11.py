# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=11

#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['Sim_mc11.pool.root'])

jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['/afs/cern.ch/work/t/tothj/public/CaloDigEx_RTT_infiles/mc11/mc11_7TeV.105200.T1_McAtNlo_Jimmy.merge.HITS.e835_s1272_s1274_tid475222_00/HITS.475222._001501.pool.root.1'])
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

jobproperties.Global.DetDescrVersion='ATLAS-GEO-18-01-00'
jobproperties.Global.ConditionsTag='OFLCOND-SDR-BS7T-05-06'

from G4AtlasApps.SimFlags import SimFlags 
SimFlags.SimLayout='ATLAS-GEO-18-01-00'

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





