from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/tmp/limbach/mc12_8TeV.147916.Pythia8_AU2CT10_jetjet_JZ6W.recon.ESD.e1126_s1599_s1600_r4156_tid01090382_00/ESD.01090382._000092.pool.root.1"]

athenaCommonFlags.EvtMax=20



from RecExConfig.RecFlags import rec#,Rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(True)
rec.doESD.set_Value_and_Lock(True)
rec.doAOD.set_Value_and_Lock(True)



import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

#don't need to set Geometry tags anymore. They are read from the file itself.
from RecExConfig.RecFlags import rec
# get inputFileSummary - will use it to extract info for MC/DATA
from RecExConfig.InputFilePeeker import inputFileSummary
# import the data types
import EventKernel.ParticleDataType
include ("RecExCond/RecExCommon_flags.py")
include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags import recAlgs

rec.doCBNT.set_Value_and_Lock( False )
rec.doTrigger.set_Value_and_Lock( False )
recAlgs.doAtlfast.set_Value_and_Lock( False )
rec.UserAlgs.set_Value_and_Lock("PanTauExample/PanTau_jobOptions.py")
#-----------------------------
# Configure eflowRec
#-_---------------------------
recAlgs.doEFlow = True
recAlgs.doEFlowJet = False
#Recovery:
from eflowRec.eflowRecFlags import jobproperties
jobproperties.eflowRecFlags.runTauMode.set_Value_and_Lock(False)
jobproperties.eflowRecFlags.useSplitShowers.set_Value_and_Lock(True)
jobproperties.eflowRecFlags.recoverIsolatedTracks.set_Value_and_Lock(True)
jobproperties.eflowRecFlags.UseElectronHadronID.set_Value_and_Lock(False)
#-----------------------------------------------------------------------------------------
# When running on input ESDs:
# rebuild topoclusters before eflowRec runs, because only (calibrated) CaloCalTopoCluster
# and not (uncalibrated) CaloTopoCluster are stored in the ESD:
#-----------------------------------------------------------------------------------------
#change some calo flags
from CaloRec.CaloRecFlags import jobproperties
jobproperties.CaloRecFlags.Enabled.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloCluster.set_Value_and_Lock(True)
jobproperties.CaloRecFlags.doCaloTopoCluster.set_Value_and_Lock(True)


PanTauAOD_ItemList=["JetCollection#EflowKt06Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowKt04Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowKt02Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowCone7Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowCone4Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowCone2Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowSISCone2Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowSISCone4Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowAntiKt02Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowAntiKt04Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowCamKt02Jets"]
PanTauAOD_ItemList+=["JetCollection#EflowCamKt04Jets"]
 
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user modifier should come here
import AthenaCommon.PropertiesManip as manip
manip.appendItemsToList(StreamAOD,'ItemList','eflowObjectContainer#eflowObjects01Prime')
manip.appendItemsToList(StreamAOD,'ItemList','CaloClusterContainer#eflowClusters01Prime')
manip.appendItemsToList(StreamAOD,'ItemList','CaloClusterContainer#CaloTopoCluster')
manip.appendItemsToList(StreamAOD,'ItemList','JetCollection#EflowConeJets')
manip.appendItemsToList(StreamAOD,'ItemList','JetKeyDescriptor#JetKeyMap')

StreamAOD.ItemList += PanTauAOD_ItemList
print "\n * * * * * * * * * * * * * * * * *"
print "modified StreamAOD Itemlist:"
print StreamAOD.ItemList

