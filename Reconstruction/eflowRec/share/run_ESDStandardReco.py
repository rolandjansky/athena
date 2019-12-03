#This file is to run standard reconstruction + pflow + jet finding on an ESD file

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"]

doDumpProperties=True

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)

from RecExConfig.RecFlags import rec
rec.doTrigger.set_Value_and_Lock(False)

#change some calo flags
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doEmCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)

#Turn of TAG
rec.doWriteTAG.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=10
#Run pflopw jet finding - this cannot be enabled via reconstruction flags currently! (without enabling other things we don't want)
UserAlgs = ["eflowRec/jetAlgs.py"]
include ("RecExCommon/RecExCommon_topOptions.py")
