from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]

from RecExConfig.RecFlags import rec
rec.doEgamma.set_Value_and_Lock(False)
#Turn on flavor tagging
rec.doBTagging.set_Value_and_Lock(True)

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)

#this turns off CaloCluster2xAOD
rec.doWritexAOD.set_Value_and_Lock(False)
#nothing to say on these
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
#Turns off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#disables VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disables AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

#Disables PFO Thnning - these cotnainers do not exist, unless you run jet finding
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=10
include ("RecExCommon/RecExCommon_topOptions.py")
include ("RecExRecoTest/RemappingsFlavorTagging.py")
