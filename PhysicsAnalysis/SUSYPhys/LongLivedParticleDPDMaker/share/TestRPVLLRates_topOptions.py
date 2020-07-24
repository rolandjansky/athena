
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
jp.AthenaCommonFlags.FilesInput = [ '/home/nbarlow/ESD/data10_7TeV.00161948.physics_Muons.recon.DESDM_RPVLL.f286_p260_tid171439_00/DESDM_RPVLL.171439._000100.pool.root.1' ]
#/tmp/nbarlow/myTestDESD.root' ]

from RecExConfig.RecFlags import rec

# get inputFileSummary - will use it to extract info for MC/DATA
from RecExConfig.InputFilePeeker import inputFileSummary

# get a handle on the ServiceManager which holds all the services 
from AthenaCommon.AppMgr import ServiceMgr

include ("RecExCond/RecExCommon_flags.py")
include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# The input file already specified near the top of this file
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
topSequence=AlgSequence()

# list of the algorithms to be executed at run time
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import RPVLLTestRates
topSequence += RPVLLTestRates() 

print(topSequence.RPVLLTestRates)

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='testRates.root' OPT='RECREATE'" ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'testRates.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO

# Number of Events to process
theApp.EvtMax = -1

       
