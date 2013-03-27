##############################################
# author: Andrzej Olszewski
# 01 April 2010 
##############################################
# calibration condition from db
###############################

# Detector description
DetDescrVersion = "ATLAS-GEO-08-00-02"

from RecExConfig.RecFlags import rec

rec.doESD = True
rec.doWriteESD = True
rec.doAOD = True
rec.doWriteAOD = True
rec.doCBNT = True

rec.doHeavyIon = True 

# Heavy Ion specific modifications of reconstruction flags are in
# heavyion_flagsESD.py (heavyion_flagsAOD.py)
# They can be redefined here, e.g.
#include ("HIRecExample/heavyion_flagsESD.py")
#rec.doRestrictedESD = True

# Other heavy ion specific reconstruction flags are in HIRecExamples/python
# HIRecExampleFlags.py. They can be redefined here, e.g.
#from HIRecExample.HIRecExampleFlags import jobproperties
#jobproperties.HIRecExampleFlags.doValidation = False
#jobproperties.HIRecExampleFlags.doHIJetRec = False
#jobproperties.HIRecExampleFlags.doHIJetAnalysis = False
#from HIGlobal.HIGlobalFlags import jobproperties
#jobproperties.HIGlobalFlags.doHIPixelTracklet = False
               
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =5

#number of Event to process
EvtMax = 5

# INPUT data files
PoolRDOInput = [
    "root://castoratlas//castor/cern.ch/user/o/olszewsk/HeavyIon/MC/RDO/user10.AndrzejOlszewski.208802.Hijing_PbPb_2p75TeV_MinBias_Flow_JJ.simul.HITS.e511_d15.6.3.6_test1/user10.AndrzejOlszewski.208802.Hijing_PbPb_2p75TeV_MinBias_Flow_JJ.simul.HITS.e511_d15.6.3.6_test1.EXT0._00001.pool.root"
    ]

# Output names
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.AthenaCommonFlags.PoolAODOutput = "hijing.aod.pool.root"
jobproperties.AthenaCommonFlags.PoolESDOutput = "hijing.esd.pool.root"
jobproperties.Rec.RootNtupleOutput            = "hijing.cbnt.root"

from IOVDbSvc.CondDB import conddb
# from sqlite dbase file
#conddb.addFolder("","<dbConnection>sqlite://X;schema=mycool.db;dbname=OFLP200</dbConnection> /GLOBAL/HeavyIon/CentralityCalib <tag>CentralityCalib_tag1</tag>")
# from published to central oracle dbase
conddb.addFolder('GLOBAL_OFL','/GLOBAL/HeavyIon/CentralityCalib <tag>CentralityCalib_tag1</tag>')

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from DetDescrCondTools.DetDescrCondToolsConf import CoolHistSvc
svcMgr+=CoolHistSvc(OutputLevel=2)

include( "HIRecExample/HeavyIonFlagsConfig.py" )

include ("RecExCommon/RecExCommon_topOptions.py")

include( "HIRecExample/HeavyIonPostOptionsConfig.py" )
