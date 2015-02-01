# main switch
# default set in HIGlobalFlags.py
# it can be changed in your top jobOption


print 'IN HIGLOBAL'
from HIRecExample.HIRecExampleFlags import jobproperties


from CaloRec.CaloTowerCmbGetter import CaloTowerCmbGetter
CaloTowerCmbGetter()

from HIGlobal.HIGlobalConf import HIEventShapeMaker
theAlg=HIEventShapeMaker()
from HIGlobal.HIGlobalFlags import jobproperties
theAlg.OutputContainerKey=jobproperties.HIGlobalFlags.EventShapeKey.get_Value()
#theAlg.NumFlowHarmonics=jobproperties.HIGlobalFlags.NumFlowHarmonics.get_Value()
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += theAlg

rootStreamName = "MYSTREAM"
rootFileName   = "myHistosAth.root"
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )
