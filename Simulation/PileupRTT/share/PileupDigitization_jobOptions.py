
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
# Setup Input and Output Files
#--------------------------------------------------------------

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 50
athenaCommonFlags.PoolHitsInput = ["root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/digitization/RTT/mc10/mc10_7TeV.105200.T1_McAtNlo_Jimmy.simul.HITS.e598_s933_tid168076_00/HITS.168076._008421.pool.root.1"]
                                              
athenaCommonFlags.PoolRDOOutput = "PileupDigiRTTOutput.RDO.pool.root"

#--------------------------------------------------------------
# Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags

# Minimum Bias
#===============
digitizationFlags.doLowPtMinBias       = True
digitizationFlags.numberOfLowPtMinBias = 2.3
include("Digitization/mc10minbiasfiles.py")

# Cavern Background
#====================
digitizationFlags.doCavern       = True
digitizationFlags.numberOfCavern = 2.0
include("Digitization/mc10cavernfiles.py")

#--------------------------------------------------------------
# Set some of the global flags. Like eg the DD version:
#--------------------------------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = 'ATLAS-GEO-16-00-00'

# Flags that are defined in python are best set here
# switch off ID and muons

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
DetFlags.Truth_setOn()
DetFlags.LVL1_setOff()

include( "Digitization/Digitization.py" )
ServiceMgr.PoolSvc.AttemptCatalogPatch = True
