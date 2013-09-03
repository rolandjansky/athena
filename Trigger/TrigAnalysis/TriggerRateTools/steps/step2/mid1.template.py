# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

AllAlgs=False
doWriteESD=False # uncomment if do not write ESD
doWriteAOD=False # uncomment if do not write AOD
doAOD=False # uncomment if do not run AOD making algorithms
doWriteTAG=False # uncomment if do not write TAG
doCBNT=False
readAOD=True
CBNTathenaAware=False
doTrigger=True
doTriggerConfigOnly=False
# add some minimal trigger stuff here...
include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
#DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()

TriggerFlags.doHypo=True
TriggerFlags.doFEX=False


# DetFlags.detdescr.all_setOff() - unavoidable from services cascade to run EF
DetFlags.readRIOPool.LVL1_setOn()
include( "RecExCommon/RecExCommon_topOptions.py" )

include( "BFieldAth/BFieldAth_jobOptions.py" )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = ERROR

