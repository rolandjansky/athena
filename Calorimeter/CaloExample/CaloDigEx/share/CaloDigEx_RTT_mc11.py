# This is the configuration file to run ATLAS Digitization
 
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.EvtMax=20

#jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['Sim_mc11.pool.root'])

jobproperties.AthenaCommonFlags.PoolHitsInput.set_Value_and_Lock(['root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc11b/mc11_7TeV.108118.Pythia8_minbias_Inelastic_low.merge.HITS.e816_s1354_s1360/HITS.580376._000962.pool.root.1'])
jobproperties.AthenaCommonFlags.PoolRDOOutput.set_Value_and_Lock('CaloDigEx_mc11b.pool.root')

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import jobproperties

 
#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags

globalglags.DetDescrVersion='ATLAS-R1-2011-02-00-00'
globalflags.ConditionsTag='OFLCOND-SDR-BS7T-05-06'

#from G4AtlasApps.SimFlags import SimFlags 
#SimFlags.SimLayout='ATLAS-GEO-18-01-00'

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




