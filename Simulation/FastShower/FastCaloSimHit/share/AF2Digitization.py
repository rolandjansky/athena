# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#--------------------------------------------------------------
#  Run the fast sim 
#--------------------------------------------------------------

printfunc (" Now do the FastSim .....")

#--------------------------------------------------------------
# FastHitConv
#--------------------------------------------------------------

from AthenaCommon.AlgSequence import AlgSequence
job=AlgSequence()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ServiceMgr

# Add top algorithms to be run
from FastCaloSimHit.FastCaloSimHitConf import FastHitConv
theFastHitConv=FastHitConv("theFastHitConv")

from AthenaCommon.DetFlags import DetFlags

list = topSequence.getSequence()
topSequence.insert(list.index('LArHitEMapMaker/digitmaker1'), theFastHitConv) # put FastHitConv right before
                                                                            # CaloDigi in topSequence

job.theFastHitConv.OutputLevel = ALL
job.OutputLevel = INFO


if DetFlags.pileup.LAr_on() or DetFlags.pileup.Tile_on():
    job.theFastHitConv.doPileup = True
else:
    job.theFastHitConv.doPileup = False
    


from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSim = True
jobproperties.CaloCellFlags.doFastCaloSimNoise = False
jobproperties.CaloCellFlags.doLArCellEmMisCalib = False

from FastCaloSimHit.FastCaloCellGetter import CaloCellGetter
theCaloCellGetter=CaloCellGetter()                          # CaloCellGetter then goes in front of FastHitConv

printfunc ("Set the TileHitVectors and LArHitContainers")

topSequence.TileHitVecToCnt.TileHitVectors += ["TileHitVec_Fast"]

topSequence.digitmaker1.LArPileUpTool.EmBarrelHitContainerName += ["LArHitEMB_Fast"]
topSequence.digitmaker1.LArPileUpTool.EmEndCapHitContainerName += ["LArHitEMEC_Fast"]
topSequence.digitmaker1.LArPileUpTool.HecHitContainerName += ["LArHitHEC_Fast"]
topSequence.digitmaker1.LArPileUpTool.ForWardHitContainerName += ["LArHitFCAL_Fast"] # It really is spelt with a "W" in the middle!


# Print out job sequence
#printfunc (" The jobsequence is now:")
#printfunc (job)
