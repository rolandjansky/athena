#
#  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaCommon.CFElements import parOR, seqOR, seqAND, stepSeq
from AthenaCommon.AlgSequence import dumpMasterSequence
from AthenaCommon.AppMgr import theApp

from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

#theApp.setup()

flags = ConfigFlags

flags.set( "global.isMC", False )
flags.set( "global.InputFiles",
           ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] )

flags.set( "Trigger.LVL1ConfigFile", "LVL1config_Physics_pp_v7.xml" )
flags.set( "Trigger.L1Decoder.forceEnableAllChains", True)

flags.lock()

from AthenaCommon.Constants import INFO,DEBUG
acc = ComponentAccumulator()

# make sure we run the right scheduler
# need to move elsewhere

nThreads=1

from StoreGate.StoreGateConf import SG__HiveMgrSvc
eventDataSvc = SG__HiveMgrSvc("EventDataSvc")
eventDataSvc.NSlots = nThreads
eventDataSvc.OutputLevel = DEBUG
acc.addService( eventDataSvc )

from SGComps.SGCompsConf import SGInputLoader
inputLoader = SGInputLoader(DetStore = 'StoreGateSvc/DetectorStore',
                            EvtStore = 'StoreGateSvc',
                            ExtraInputs = [],
                            ExtraOutputs = [],
                            FailIfNoProxy = False,
                            Load = [],
                            NeededResources = [])

acc.addEventAlgo( inputLoader)

from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
acc.merge(TrigBSReadCfg(flags ))

#from AtlasGeoModel.GeoModelConfig import GeoModelCfg
#acc.merge(GeoModelCfg(flags ))

from TrigUpgradeTest.TriggerHistSvcConfig import TriggerHistSvcConfig
acc.merge(TriggerHistSvcConfig(flags ))

def menu( mf ):
    menuAcc = ComponentAccumulator()
    HLTSteps =  seqAND( "HLTSteps")
    menuAcc.addSequence( HLTSteps )
    menuAcc.addSequence( parOR("HLTStep_1_filters"), parentName="HLTSteps" )
    menuAcc.addSequence( parOR("HLTStep_1"), parentName="HLTSteps" )
    menuAcc.addSequence( parOR("HLTStep_2_filters"), parentName="HLTSteps" )
    menuAcc.addSequence( parOR("HLTStep_2"), parentName="HLTSteps" )

    from TrigUpgradeTest.EgammaCaloMod import EgammaCaloMod
    accECM,seqECM=EgammaCaloMod(flags)
    menuAcc.merge(accECM)
    menuAcc.addSequence(seqECM, parentName="HLTStep_1" )
    return menuAcc, HLTSteps


from TriggerJobOpts.TriggerConfig import triggerRunCfg
acc.merge( triggerRunCfg(flags, menu) )


from EventInfoMgt.EventInfoMgtConf import TagInfoMgr
tagInfoMgr = TagInfoMgr()
tagInfoMgr.ExtraTagValuePairs    = ['AtlasRelease', 'Athena-22.0.1'] # this has to come from somewhere else
acc.addService( tagInfoMgr )

acc.getService("EventPersistencySvc").CnvServices += [ tagInfoMgr.getName() ]
acc.getService("ProxyProviderSvc").ProviderNames  += [ tagInfoMgr.getName() ]
acc.getService("IOVDbSvc").Folders += ['/TagInfo<metaOnly/>']

# we need to setup it because of conditions data
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
athenaPoolSvcSvc = AthenaPoolCnvSvc()
athenaPoolSvcSvc.PoolAttributes = ["DEFAULT_SPLITLEVEL ='0'", "STREAM_MEMBER_WISE = '1'", "DEFAULT_BUFFERSIZE = '32000'", "ContainerName = 'POOLContainer(DataHeader)'; BRANCH_BASKET_SIZE = '256000'", "ContainerName = 'POOLContainerForm(DataHeaderForm)'; BRANCH_BASKET_SIZE = '1024000'", "ContainerName = 'TTree=POOLContainerForm(DataHeaderForm)'; CONTAINER_SPLITLEVEL = '99'"]
acc.addService( athenaPoolSvcSvc )
acc.getService( "EventPersistencySvc" ).CnvServices += [ athenaPoolSvcSvc.getName() ]


# setup algorithm sequences here, need few additional components
from TrigUpgradeTest.RegSelConfig import RegSelConfig
acc.merge( RegSelConfig( flags ) )

acc.getEventAlgo( "TrigSignatureMoniMT" ).OutputLevel=DEBUG
print acc.getEventAlgo( "TrigSignatureMoniMT" )

from TrigUpgradeTest.TestUtils import applyMenu
applyMenu( acc.getEventAlgo( "L1Decoder" ) )
acc.getEventAlgo( "L1Decoder" ).OutputLevel=DEBUG
acc.getEventAlgo( "L2CaloHypo" ).OutputLevel=DEBUG


acc.printConfig()


fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p )
    p.close()


