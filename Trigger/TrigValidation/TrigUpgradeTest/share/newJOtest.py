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

flags.Input.isMC = False
flags.Input.Files= ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"] 

flags.Trigger.LVL1ConfigFile = "LVL1config_Physics_pp_v7.xml" 
flags.Trigger.L1Decoder.forceEnableAllChains = True


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
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import HLTMenuAccumulator
    menuAcc = HLTMenuAccumulator()

    # here menu generation starts


    from TrigUpgradeTest.ElectronMenuConfig import generateElectronsCfg
    accElectrons, steps = generateElectronsCfg( flags ) 
    if len( steps ) != 0:
        menuAcc.setupSteps( steps )         
        menuAcc.merge( accElectrons )


    from TrigUpgradeTest.PhotonMenuConfig import generatePhotonsCfg
    accPhotons, steps = generatePhotonsCfg( flags ) 
    if len( steps ) != 0:
        menuAcc.setupSteps( steps )         
        menuAcc.merge( accPhotons )




    # here setting of the Summary + top level Monitoring algs should be done
    menuAcc.printConfig()    
    
    return menuAcc, menuAcc.steps()


from TriggerJobOpts.TriggerConfig import triggerRunCfg
acc.merge( triggerRunCfg( flags, menu ) )


from EventInfoMgt.EventInfoMgtConf import TagInfoMgr
tagInfoMgr = TagInfoMgr()
tagInfoMgr.ExtraTagValuePairs    = ['AtlasRelease', 'Athena-22.0.1'] # this has to come from somewhere else
acc.addService( tagInfoMgr )

acc.getService("EventPersistencySvc").CnvServices += [ tagInfoMgr.getName() ]
acc.getService("ProxyProviderSvc").ProviderNames  += [ tagInfoMgr.getName() ]
acc.getService("IOVDbSvc").Folders += ['/TagInfo<metaOnly/>']


# setup algorithm sequences here, need few additional components
from TrigUpgradeTest.RegSelConfig import RegSelConfig
acc.merge( RegSelConfig( flags ) )

acc.getEventAlgo( "TrigSignatureMoniMT" ).OutputLevel=DEBUG
print acc.getEventAlgo( "TrigSignatureMoniMT" )



# from TrigUpgradeTest.TestUtils import applyMenu
# applyMenu( acc.getEventAlgo( "L1Decoder" ) )
acc.getEventAlgo( "L1Decoder" ).OutputLevel=DEBUG
acc.getEventAlgo( "L2ElectronCaloHypo" ).OutputLevel=DEBUG
acc.getEventAlgo( "FastEMCaloAlgo" ).OutputLevel=DEBUG


acc.printConfig()


fname = "newJOtest.pkl"
print "Storing config in the config", fname
with file(fname, "w") as p:
    acc.store( p )
    p.close()


