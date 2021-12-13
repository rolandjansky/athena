from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/input/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.ESD.e4993_s3227_r12847/ESD.26288534._000001.pool.root.1", "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/tauRec/input/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.ESD.e4993_s3227_r12847/ESD.26288534._000002.pool.root.1"]
athenaCommonFlags.PoolAODOutput = "EMTopo_AOD.pool.root"

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("OFLCOND-MC16-SDR-RUN2-08")

from RecExConfig.RecFlags import rec
rec.doEgamma.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)

from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(False)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(False)

#this turns off CaloCluster2xAOD
rec.doWritexAOD.set_Value_and_Lock(False)
#nothing to say on these
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(True)
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
#Turns off xAODRingSetConfWriter
rec.doCaloRinger.set_Value_and_Lock(False)

#disables VertexCnvAlg
from InDetRecExample.InDetJobProperties import jobproperties
jobproperties.InDetJobProperties.doxAOD.set_Value_and_Lock(False)
#Disables AllExecutedEvents
rec.doFileMetaData.set_Value_and_Lock(False)

athenaCommonFlags.EvtMax=10

# enable standalone tau reconstruction
rec.doTau.set_Value_and_Lock(True)
from tauRec.tauRecFlags import tauFlags
tauFlags.isStandalone.set_Value_and_Lock(True)
# override RecExCommon_flags behaviour
tauFlags.Enabled.set_Value_and_Lock(True)
# set seed jets
tauFlags.tauRecSeedJetCollection.set_Value_and_Lock("AntiKt4EMTopoJets")

# ditau reconstruction setup
from DiTauRec.DiTauRecFlags import diTauFlags
diTauFlags.doVtxFinding.set_Value_and_Lock(True)

include ("RecExCommon/RecExCommon_topOptions.py")

# FlowElements are not rebuilt in standalone tau reconstruction from ESD
# the FE->tau links from the ESD will be messed up by tau thinning if we write out FE containers to AOD
# therefore, we must remove FE containers
import re
StreamAOD.ItemList = [x for x in StreamAOD.ItemList if not re.search("JetETMiss.*ParticleFlowObjects",x)]
# the other option is to deactivate tau thinning, but this wouldn't give a realistic tau AOD content
#from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.ThinTaus.set_Value_and_Lock(False)

condSeq = AthSequencer("AthCondSeq")
if not hasattr( condSeq, "LumiBlockMuWriter" ):
  include ("LumiBlockComps/LumiBlockMuWriter_jobOptions.py")
