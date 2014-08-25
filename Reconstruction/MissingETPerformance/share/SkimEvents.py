#leave all these False
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
primDPD.MakePrimaryDPD = False
primDPD.UseMCTruth=False
primDPD.WriteLargeMetStream       = False
primDPD.WritePrescaledESDStream   = False
primDPD.ApplyJetSlimming = False
primDPD.ApplySkimming = False
primDPD.ApplySlimming = False
primDPD.ApplyThinning = False
primDPD.WriteSkimDecisions = False
primDPD.WritePrimaryDPDTrigger = False

#Global setup
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#pick the correct flag for ESD or AOD input
athenaCommonFlags.PoolESDInput=["/scratch/ayurkewi/data08_cosmag.00091361.physics_L1Calo.recon.ESD.o4_r602_tid030702/ESD.030702._00211.pool.root.2"]
#athenaCommonFlags.PoolAODInput=["/scratch/ayurkewi/Wenu_files/mc08.106020.PythiaWenu_1Lepton.recon.AOD.e352_s462_r604_tid032279/AOD.032279._01127.pool.root"]

from AthenaCommon.GlobalFlags import globalflags
#set correct geometry version
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-03-00-00")
from AthenaCommon.GlobalFlags import GlobalFlags
#need this set_data() uncommented for real data, commented for monte carlo
GlobalFlags.DataSource.set_data()
include( "RecExCond/AllDet_detDescr.py")
#athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)
#set max number of events to process here
athenaCommonFlags.EvtMax=10

#use the DPD maker to skim events
#pick the correct include for ESD or AOD input
include("PrimaryDPDMaker/esdtodpd.py")
#include("PrimaryDPDMaker/aodtodpd.py")

#define my filter here
#need to modify some things there as well
include("MyFilter.py")
