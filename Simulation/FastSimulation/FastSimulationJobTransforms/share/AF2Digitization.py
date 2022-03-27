#--------------------------------------------------------------
#  Run the fast sim 
#--------------------------------------------------------------

print " Now do the FastSim ....."

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


from AthenaCommon.DetFlags import DetFlags

if DetFlags.pileup.LAr_on() or DetFlags.pileup.Tile_on():
    job.theFastHitConv.doPileup = True
else:
    job.theFastHitConv.doPileup = False
    


print topSequence

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSim = True
jobproperties.CaloCellFlags.doFastCaloSimNoise = False
jobproperties.CaloCellFlags.doLArCellEmMisCalib = False

from FastCaloSimHit.FastCaloCellGetter import CaloCellGetter
theCaloCellGetter=CaloCellGetter()                          # CaloCellGetter then goes in front of FastHitConv

print "Set the TileHitVectors and LArHitContainers"

topSequence.TileHitVecToCnt.TileHitVectors += ["TileHitVec_Fast"]

topSequence.digitmaker1.EmBarrelHitContainerName += ["LArHitEMB_Fast"]
topSequence.digitmaker1.EmEndCapHitContainerName += ["LArHitEMEC_Fast"]
topSequence.digitmaker1.HecHitContainerName += ["LArHitHEC_Fast"]
topSequence.digitmaker1.ForWardHitContainerName += ["LArHitFCAL_Fast"] # It really is spelt with a "W" in the middle!


# Print out job sequence
print " The jobsequence is now:"
print job
