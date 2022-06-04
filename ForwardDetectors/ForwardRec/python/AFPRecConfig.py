#!/bin/env python

# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# file   AFPRecConfig.py
# author Petr Balek <petr.balek@cern.ch>
# date   2022-03-31

# brief  A script that provides setup for AFP reconstruction included in RecJobTransforms/RecoSteering.py. 
#        It also allows to run standalone AFP reconstruction. To use it:
#           0. setup athena enviroment
#           1a. run this script:
#               $ python AFPRecConfig.py
#           1b. feel free to change the input file:
#               $ python AFPRecConfig.py --filesInput=/afs/cern.ch/user/p/pbalek/workspace/public/data17_13TeV.00338480.physics_Main.daq.RAW/data17_13TeV.00338480.physics_Main.daq.RAW._lb0275._SFO-7._0007.data
#           1c. run the whole reconstruction with:
#               $ RecoSteeringTest.py --filesInput=/afs/cern.ch/user/p/pbalek/workspace/public/data17_13TeV.00338480.physics_Main.daq.RAW/data17_13TeV.00338480.physics_Main.daq.RAW._lb0275._SFO-7._0007.data --RAW

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import Format

def AFPRecOutputCfg(flags):
    """defines outputs for ESD and AOD; provides the same information as in ForwardRec/AFP_Rec_OutputItemList_jobOptions.py"""
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc = ComponentAccumulator()
    
    AFP_ItemList=[]

    if flags.Input.Format is Format.BS:
        # AFP Silicon hits containers
        AFP_ItemList.append("xAOD::AFPSiHitContainer#AFPSiHitContainer")
        AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#AFPSiHitContainerAux.")
        AFP_ItemList.append("xAOD::AFPSiHitsClusterContainer#AFPSiHitsClusterContainer")
        AFP_ItemList.append("xAOD::AFPSiHitsClusterAuxContainer#AFPSiHitsClusterContainerAux.")

        # for runs with more BCX ids i.e. before timing in the detectors
        for bcIDshift in range (1, 6):
            name = "AFPSiHitContainerBcIDplus" + str(bcIDshift)
            AFP_ItemList.append("xAOD::AFPSiHitContainer#" + name)
            AFP_ItemList.append("xAOD::AFPSiHitAuxContainer#" + name + "Aux.")

    AFP_ItemList.append("xAOD::AFPTrackContainer#AFPTrackContainer")
    AFP_ItemList.append("xAOD::AFPTrackAuxContainer#AFPTrackContainerAux.")
    AFP_ItemList.append("xAOD::AFPToFHitContainer#AFPToFHitContainer")
    AFP_ItemList.append("xAOD::AFPToFHitAuxContainer#AFPToFHitContainerAux.")
    AFP_ItemList.append("xAOD::AFPProtonContainer#AFPProtonContainer")
    AFP_ItemList.append("xAOD::AFPProtonAuxContainer#AFPProtonContainerAux.")
    AFP_ItemList.append("xAOD::AFPToFTrackContainer#AFPToFTrackContainer")
    AFP_ItemList.append("xAOD::AFPToFTrackAuxContainer#AFPToFTrackContainerAux.")
    AFP_ItemList.append("xAOD::AFPVertexContainer#AFPVertexContainer")
    AFP_ItemList.append("xAOD::AFPVertexAuxContainer#AFPVertexContainerAux.")


    if flags.Output.doWriteESD:
        acc.merge(OutputStreamCfg(flags, "ESD", AFP_ItemList))
    if flags.Output.doWriteAOD:
        acc.merge(OutputStreamCfg(flags, "AOD", AFP_ItemList))
    return acc


def AFPRecCfg(flags):
    """defines AFP reconstruction; provides the same setup as used to be in ForwardRec/ForwardRec_jobOptions.py"""
    acc = ComponentAccumulator()
    
    if flags.Input.Format is Format.BS:
        from AthenaConfiguration.ComponentFactory import CompFactory
        
        acc.addEventAlgo(CompFactory.AFP_RawDataProvider("AFP_RawDataProvider"))
        acc.addEventAlgo(CompFactory.AFP_Raw2Digi("AFP_Raw2Digi"))
    
    #cluster reconstruction
    from AFP_SiClusterTools.AFP_SiClusterTools import AFP_SiClusterTools_Cfg
    acc.merge(AFP_SiClusterTools_Cfg(flags))
  
    # tracks reconstruction
    from AFP_LocReco.AFP_LocReco import AFP_LocReco_SiD_Cfg, AFP_LocReco_TD_Cfg
    acc.merge(AFP_LocReco_SiD_Cfg(flags))
    acc.merge(AFP_LocReco_TD_Cfg(flags))
   
    # protons reconstruction
    from AFP_GlobReco.AFP_GlobReco import AFP_GlobReco_Cfg
    acc.merge(AFP_GlobReco_Cfg(flags))
  
    # vertex reconstruction
    from AFP_VertexReco.AFP_VertexReco import AFP_VertexReco_Cfg
    acc.merge(AFP_VertexReco_Cfg(flags))
    
    # Setup output
    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        acc.merge(AFPRecOutputCfg(flags))
    
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    
    ConfigFlags.Scheduler.CheckDependencies = True
    ConfigFlags.Scheduler.ShowDataDeps = True
    ConfigFlags.Scheduler.ShowDataFlow = True
    ConfigFlags.Scheduler.ShowControlFlow = True
    ConfigFlags.Scheduler.EnableVerboseViews = True
    
    ConfigFlags.Input.Files = ["/afs/cern.ch/work/p/pbalek/public/data17_13TeV.00338480.physics_Main.daq.RAW/data17_13TeV.00338480.physics_Main.daq.RAW._lb0275._SFO-7._0007.data"]
    
    ConfigFlags.Exec.MaxEvents=500
    ConfigFlags.Concurrency.NumThreads=4
 
    ConfigFlags.fillFromArgs() # enable unit tests to switch only parts of reco: python -m HIRecConfig.HIRecConfig HeavyIon.doGlobal = 0 and so on
    ConfigFlags.lock()
    ConfigFlags.dump()
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(ConfigFlags)
    acc.getEventAlgo("SGInputLoader").FailIfNoProxy = True # enforce no missing data
    
    
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc.merge(ByteStreamReadCfg(ConfigFlags))

    acc.merge(AFPRecCfg(ConfigFlags))
    
    from AthenaCommon.Constants import DEBUG
    acc.foreach_component("*AFP*").OutputLevel=DEBUG
    
    acc.printConfig(withDetails=True, summariseProps=True)
    
    status = acc.run()
    if status.isFailure():
        import sys
        sys.exit(-1)


