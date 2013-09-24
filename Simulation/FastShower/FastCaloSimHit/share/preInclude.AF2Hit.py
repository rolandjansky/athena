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

topSequence += theFastHitConv

job.theFastHitConv.OutputLevel = ALL
job.OutputLevel = INFO


from AthenaCommon.DetFlags import DetFlags

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
