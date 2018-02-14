#This file is to run standard reconstruction + pflow + jet finding on an ESD file

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/data/hodgkinson/dataFiles/mc16_13TeV/ESDFiles/mc16_valid.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.ESD.e3698_s2995_r8905/ESD.10230993._000008.pool.root.1"]

doDumpProperties=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

#change some calo flags
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Turn of TAG
rec.doWriteTAG.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=1
#Run pflopw jet finding - this cannot be enabled via reconstruction flags currently! (without enabling other things we don't want)
UserAlgs = ["jetAlgs.py"]
include ("RecExCommon/RecExCommon_topOptions.py")

