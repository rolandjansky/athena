# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties

#for RTT test only : the input file is in the chain store or in the fallback directory 
jobproperties.AthenaCommonFlags.PoolHitsInput=jobproperties.AthenaCommonFlags.FilesInput.get_Value()

#for specified input file
#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['Hits.pool.root'])

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties
# jobproperties.Digitization.doMinimumBias=True
# jobproperties.Digitization.numberOfCollisions=2.3
# jobproperties.Digitization.minBiasInputCols=["", "", "" ]
# jobproperties.Digitization.doCavern=True
# jobproperties.Digitization.cavernInputCols=["",""]
# jobproperties.Digitization.physicsList = 'QGSP_BERT'

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







