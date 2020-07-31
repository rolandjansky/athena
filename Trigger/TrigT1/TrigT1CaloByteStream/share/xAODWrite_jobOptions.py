
# ==============================================================================
# Change the input file
InputFiles = [
    "/afs/cern.ch/work/v/vkousk/public/L1Calo/data15_cos.00255313.physics_CosmicCalo.merge.RAW._lb0700._SFO-ALL._0001.1"
#    "/afs/cern.ch/user/t/tcorneli/public/PROC/extractedEvents8.data",
#    "/afs/cern.ch/work/g/geisler/public/misc/l1caloruns/data15_calib.00252372.calibration_L1CaloPprPhos4ScanPars.daq.RAW._lb0000._SFO-1._0001.data",
#    "/afs/cern.ch/work/h/hmeyerzt/public/data/data15_cos.00252223.physics_CosmicCalo.merge.RAW._lb0025._SFO-ALL._0001.1",
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
# ==============================================================================

from ByteStreamCnvSvc import ReadByteStream


svcMgr.EventSelector.Input = InputFiles
include("TrigT1CaloByteStream/ReadLVL1CaloBSRun2_jobOptions.py")
svcMgr.MessageSvc.defaultLimit = 1000000


class PyTriggerTowerRef(PyAthena.Alg):

    def __init__(self, name='PyTriggerTowerRef', **kw):
        # init base class
        kw['name'] = name
        super(PyTriggerTowerRef, self).__init__(**kw)

    def initialize(self):
        self.event_store = PyAthena.py_svc("StoreGateSvc")
        return PyAthena.StatusCode.Success

    def execute(self):
        #tt =  self.event_store["xAODTriggerTowers"]
        cpm =  self.event_store["CPMTowers"]
        # je = self.event_store["JetElements"]
        # for j in je:
        #     vv = j.hadEnergyVec()
        #     for v in vv:
        #         print v,
        #     print " "
        self.setFilterPassed(True)
        return PyAthena.StatusCode.Success

    def finalize(self):
        return PyAthena.StatusCode.Success
    pass

from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()
topSequence += PyTriggerTowerRef()


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
   
#MyFirstXAODStream = MSMgr.NewPoolRootStream( "StreamxAOD", "TileCIS.root" )
#MyByteStream = MSMgr.NewByteStream( "StreamBS", "TileCIS.raw" )
#MyFirstXAODStream.AddItem(["xAOD::CPMTowerContainer#xAODCPMTowers", "xAOD::CPMTowerAuxContainer#xAODCPMTowersAux."])
#MyFirstXAODStream.AddItem(["xAOD::TriggerTowerContainer#xAODTriggerTowers", "xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux."])
#MyByteStream.AddItem(["xAOD::TriggerTowerContainer#xAODTriggerTowers"])


# svcMgr.StoreGateSvc.Dump = True
#svcMgr.EventSelector.SkipEvents = 2 
theApp.EvtMax = 10
# ==============================================================================
