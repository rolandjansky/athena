# steering file for RDO->ESD step without trigger
# see myTopOptions.py for more info

doCBNT=False
doTrigger=False
doWriteRDO=False
doWriteESD=False
if not 'doWriteESD' in dir():
    doWriteESD=True
doWriteAOD=False
doAOD=False 
doWriteTAG=False 

#doPerfMon=False
#doHist=False

#include.block("RecExCommon/RecoUtils.py")

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

#theAuditorSvc = svcMgr.AuditorSvc
#theApp.AuditAlgorithms=False
#theApp.AuditServices=False
#theApp.AuditTools=False

#VGCOMMENT: no CBNT, Trigger, Muons, ESD
