from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# #change the line below to your desired input file
athenaCommonFlags.FilesInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PFlowTests/mc15_14TeV/ESD/mc15_14TeV.900327.PG_single_piminus_E0p4to2_etaFlatnp0_43.recon.ESD.e8373_s3735_s3738_r13118/ESD.27046140._000012.pool.root.1"]

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(True)
recAlgs.doMissingET.set_Value_and_Lock(False)
 
from RecExConfig.RecFlags import rec
rec.doAODCaloCells.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
#The ITK setup files disable calo, so lets stop them from doing that
rec.doCalo.set_Value_and_Lock(True)
 
from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.eflowAlgType.set_Value_and_Lock("EOverP")
jobproperties.eflowRecFlags.CalType.set_Value_and_Lock("NoCal")
 
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)
#change below to required number of events:
athenaCommonFlags.EvtMax=10

include("eflowRec/setupITKPFlow.py")

include ("RecExCommon/RecExCommon_topOptions.py")
include('InDetSLHC_Example/postInclude.SLHC_Setup_ITK.py')

import AthenaCommon.PropertiesManip as manip
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterContainer#CaloTopoCluster')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterAuxContainer#CaloTopoClusterAux.')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterContainer#eflowCluster01')
manip.appendItemsToList(StreamAOD,'ItemList','xAOD::CaloClusterAuxContainer#eflowCluster01Aux.') 
