#-----------------------------------------------------------------------------
#  Set Container names for calo digi (has to be done after calo digi setup) 
#-----------------------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

if jobproperties.Digitization.doXingByXingPileUp():
    puAlg = PileUpToolsAlg()
    
    puAlg.PileUpTools["TileHitVecToCntTool"].TileHitVectors += ["TileHitVec_Fast"]
    puAlg.PileUpTools["LArPileUpTool"].EmBarrelHitContainerName += ["LArHitEMB_Fast"]
    puAlg.PileUpTools["LArPileUpTool"].EmEndCapHitContainerName += ["LArHitEMEC_Fast"]
    puAlg.PileUpTools["LArPileUpTool"].HecHitContainerName += ["LArHitHEC_Fast"]
    puAlg.PileUpTools["LArPileUpTool"].ForWardHitContainerName += ["LArHitFCAL_Fast"]

    #print puAlg.PileUpTools
else:
    topSequence.TileHitVecToCnt.TileHitVectors += ["TileHitVec_Fast"]
    topSequence.digitmaker1.LArPileUpTool.EmBarrelHitContainerName += ["LArHitEMB_Fast"]
    topSequence.digitmaker1.LArPileUpTool.EmEndCapHitContainerName += ["LArHitEMEC_Fast"]
    topSequence.digitmaker1.LArPileUpTool.HecHitContainerName += ["LArHitHEC_Fast"]
    topSequence.digitmaker1.LArPileUpTool.ForWardHitContainerName += ["LArHitFCAL_Fast"] # It really is spelt with a "W" in the middle!

    #topSequence.digitmaker1.EmBarrelHitContainerName += ["LArHitEMB_Fast"]
    #topSequence.digitmaker1.EmEndCapHitContainerName += ["LArHitEMEC_Fast"]
    #topSequence.digitmaker1.HecHitContainerName += ["LArHitHEC_Fast"]
    #topSequence.digitmaker1.ForWardHitContainerName += ["LArHitFCAL_Fast"] # It really is spelt with a "W" in the middle!

# setup the LAr hit container remapping if necessary
from AthenaCommon.AppMgr import ServiceMgr
if hasattr(ServiceMgr, 'AddressRemappingSvc'):
  ServiceMgr.AddressRemappingSvc.TypeKeyOverwriteMaps += [
      "LArHitContainer#LArHitEMB_Fast->LArHitFloatContainer#LArHitEMB_Fast",
      "LArHitContainer#LArHitEMEC_Fast->LArHitFloatContainer#LArHitEMEC_Fast",
      "LArHitContainer#LArHitHEC_Fast->LArHitFloatContainer#LArHitHEC_Fast",
      "LArHitContainer#LArHitFCAL_Fast->LArHitFloatContainer#LArHitFCAL_Fast" ]

