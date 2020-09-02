import re

InFiles = [
    "/afs/cern.ch/user/l/landon/work/public/raw/data15_hi.00287843.physics_HardProbes.daq.RAW._lb0200._SFO-6._0004.data",
    #"/afs/cern.ch/work/h/hristova/public/M9/256147/data15_cos.00256147.physics_L1Calo.merge.RAW._lb0500._SFO-ALL._0001.1"
    #"/afs/cern.ch/work/v/vkousk/public/L1Calo/data15_cos.00255313.physics_CosmicCalo.merge.RAW._lb0700._SFO-ALL._0001.1"
    #    "/afs/cern.ch/user/t/tcorneli/public/PROC/extractedEvents8.data",
    #    "/afs/cern.ch/work/g/geisler/public/misc/l1caloruns/data15_calib.00252372.calibration_L1CaloPprPhos4ScanPars.daq.RAW._lb0000._SFO-1._0001.data",
 #   "/afs/cern.ch/work/h/hmeyerzt/public/data/data15_cos.00252223.physics_CosmicCalo.merge.RAW._lb0025._SFO-ALL._0001.1",
    #    "/afs/cern.ch/user/r/rhowen/ppmData/ppmBcMuxRamp.data",
    #     "/afs/cern.ch/user/r/rhowen/ppmData/ppmBcMuxRamp_Compressed.data",
    #    "/afs/cern.ch/user/r/rhowen/ppmData/ppmnMCMRamp2_Compressed.data",
    #    "/afs/cern.ch/user/r/rhowen/ppmData/ppmnMCMRamp2.data",
    #    "/afs/cern.ch/user/r/rhowen/ppmData/ppmRandomData_Compressed.data",
    #"/afs/cern.ch/user/r/rhowen/ppmData/ppmRandomData_take2.data",
    #"/afs/cern.ch/user/r/rhowen/ppmData/ppmConstant123_Compressed.data",
    #"/afs/cern.ch/user/r/rhowen/ppmData/ppmSparceRandom_compressed.data",
    #"/afs/cern.ch/user/r/rhowen/ppmData/ppmConstant32_compressed.data"
    #"/afs/cern.ch/work/j/juraj/public/2015/TileCIS/251619/data15_calib.00251619.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data",
    #    "/afs/cern.ch/work/g/geisler/public/misc/data09_1beam.00140370.physics_MinBias.merge.RAW._lb0236._0001.1",
    #"/afs/cern.ch/work/h/hmeyerzt/public/data/data14_cos.00247548.physics_CosmicCalo.merge.RAW._lb0097._SFO-ALL._0001.1",
    #"/afs/cern.ch/work/v/vscharf/public/splash/data09_1beam.00140370.physics_MinBias.merge.RAW._lb0002._0001.1",
    #"/afs/cern.ch/work/h/hristova/public/M7/248370/data14_cos.00248370.physics_L1Calo.merge.RAW._lb0007._SFO-ALL._0001.1",
    #"/afs/cern.ch/work/h/hristova/public/M7/248370/data14_cos.00248370.express_express.merge.RAW._lb0163._SFO-ALL._0001.1"
    #"/afs/cern.ch/work/j/juraj/public/2015/TileCIS/data15_calib.00249297.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data",
    #"/afs/cern.ch/work/j/juraj/public/2015/TileCIS/data15_calib.00249300.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data",
    #    "/afs/cern.ch/work/j/jfrost/public/R19testing/slice_rerun/data12_8TeV.00204158.express_express.merge.RAW._lb0958._SFO-ALL._0001.1",
    #"/afs/cern.ch/work/j/jfrost/public/M8_tests/HLT_mcV5_rerunLvl1-1._0001.data"
]

runNumberReg = re.compile('\d' * 8)
runNumber = ""
for i in InFiles:
    res = runNumberReg.search(i)
    if res:
        runNumber += res.group(0) + "."


# configuration of globalflags... do this FIRST so that anything else you
# do that might load conddb will be ready for it
from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'data'
globalflags.DatabaseInstance = 'CONDBR2'
globalflags.InputFormat = 'bytestream'
globalflags.DetDescrVersion = "ATLAS-R2-2015-02-00-00"
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag("CONDBR2-ES1PA-2015-01")


# Output file
OutFile = "xAOD.L1Calo." + runNumber + "pool.root"
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream("StreamXAOD", OutFile)

# Decorate trigger towers with Calo Cells?
# This should be setup better
#decorateTT = True

# Number of events (-1 == all events)


# Tell Athena about the input file(s)
from ByteStreamCnvSvc import ReadByteStream
svcMgr.EventSelector.Input = InFiles

topSequence = CfgMgr.AthSequencer("AthAlgSeq")

# Add L1Calo xAOD to output
include("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")

xaodStream.AddItem("xAOD::EventInfo#*")
xaodStream.AddItem("xAOD::EventAuxInfo#*")

xaod = [
    (True, "TriggerTower", "xAODTriggerTowers"),
    (True, "CPMTower", "CPMTowers"),
    (True, "CMXCPTob", "CMXCPTobs"),
    (True, "CMXCPHits", "CMXCPHits"),
    (True, "CMXCPHits", "CMXCPHits"),
    (True, "CMXJetTob", "CMXJetTobs"),
    (True, "CMXJetHits", "CMXJetHits"),
    (True, "CMXEtSums", "CMXEtSums"),
    (True, "JEMEtSums", "JEMEtSums"),
    (True, "CPMTobRoI", "CPMTobRoIs"),
    (True, "CPMTobRoI", "CPMTobRoIsRoIB"),
    (True, "JetElement", "JetElements"),
    (True, "JetElement", "JetElementsOverlap"),
    (True, "CMXRoI", "CMXRoIs"),
    (True, "CMXRoI", "CMXRoIsRoIB"),
    (True, "RODHeader", "RODHeaders"),
    (True, "L1TopoRawData", "L1TopoRawData"),
]

for enabled, prefix, key in xaod:
	if not enabled:
		continue
	xaodStream.AddItem( "xAOD::%sContainer#%s" % (prefix, key))
	xaodStream.AddItem( "xAOD::%sAuxContainer#%sAux." % (prefix, key))


# Do some additional tweaking:
theApp.EvtMax = 150
#ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
