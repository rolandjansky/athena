# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
#from AthenaConfiguration.MainServicesConfig import MainServicesSerial


def BunchCrossingCondAlgCfg(configFlags):
    BunchCrossingCondAlg=CompFactory.BunchCrossingCondAlg
    from IOVDbSvc.IOVDbSvcConfig import addFolders

    result=ComponentAccumulator()

    run1=(configFlags.IOVDb.DatabaseInstance=='COMP200')

    if (configFlags.Input.isMC):
        folder = "/Digitization/Parameters"
        Mode = 1
        from AthenaConfiguration.Enums import ProductionStep
        if configFlags.Common.ProductionStep not in [ProductionStep.Digitization, ProductionStep.PileUpPresampling]:
            result.merge(addFolders(configFlags,folder,None,className="AthenaAttributeList",tag='HEAD'))
        else:
            # Here we are in a digitization job, so the
            # /Digitization/Parameters metadata is not present in the
            # input file and will be created during the job
            pass
    else: #data case
        folder = '/TDAQ/OLC/LHC/FILLPARAMS'
        Mode = 0
        result.merge(addFolders(configFlags,folder,'TDAQ',className = 'AthenaAttributeList',tag='HEAD'))


    alg = BunchCrossingCondAlg('BunchCrossingCondAlgDefault',
                               Run1=run1,
                               FillParamsFolderKey =folder,
                               Mode=Mode )

    result.addCondAlgo(alg)

    return result
    



if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags


    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    ConfigFlags.IOVDb.DatabaseInstance="CONDBR2"
    ConfigFlags.Input.isMC=False
    ConfigFlags.IOVDb.GlobalTag="CONDBR2-BLKPA-2017-05"
    ConfigFlags.lock()


    

    result=MainServicesCfg(ConfigFlags)


    McEventSelector=CompFactory.McEventSelector
    McCnvSvc=CompFactory.McCnvSvc
    EvtPersistencySvc=CompFactory.EvtPersistencySvc

    #event & time-stamp from the q431 test input 
    mcevtsel=McEventSelector(RunNumber=330470,
                             EventsPerRun=1,
                             FirstEvent=1183722158,
                             FirstLB=310,
                             EventsPerLB=1,
                             InitialTimeStamp=1500867637,
                             TimeStampInterval=1
                         )

    result.addService(mcevtsel)
    result.setAppProperty("EvtSel",mcevtsel.getFullJobOptName())

    mccnvsvc=McCnvSvc()
    result.addService(mccnvsvc)


    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[mccnvsvc.getFullJobOptName(),]))
    
    result.merge(BunchCrossingCondAlgCfg(ConfigFlags))
    
    
    BunchCrossingCondTest=CompFactory.BunchCrossingCondTest
    result.addEventAlgo(BunchCrossingCondTest(FileName="BCData1.txt"))

    result.run(1)
                       
    
    #f=open("test.pkl","wb")
    #result.store(f)
    #f.close()
