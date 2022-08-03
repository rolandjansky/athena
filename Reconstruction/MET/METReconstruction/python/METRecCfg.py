# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

# Configure all MET algorithms for standard reconstruction
def METCfg(inputFlags):
    result=ComponentAccumulator()
    outputList = []

    # Calo MET collections (for monitoring)
    from METReconstruction.METCalo_Cfg import METCalo_Cfg
    result.merge(METCalo_Cfg(inputFlags))
    metDefs = ['EMTopo', 'EMTopoRegions', 'LocHadTopo', 'LocHadTopoRegions', 'Calo']
    for metDef in metDefs:
        outputList.append('xAOD::MissingETContainer#MET_'+metDef)
        outputList.append('xAOD::MissingETAuxContainer#MET_'+metDef+'Aux.')

    # Track MET
    from METReconstruction.METTrack_Cfg import METTrack_Cfg
    result.merge(METTrack_Cfg(inputFlags))
    outputList.append("xAOD::MissingETContainer#MET_Track")
    outputList.append("xAOD::MissingETAuxContainer#MET_TrackAux.")

    # Truth MET
    if inputFlags.Input.isMC:
        from METReconstruction.METTruth_Cfg import METTruth_Cfg
        result.merge(METTruth_Cfg(inputFlags))
        outputList.append("xAOD::MissingETContainer#MET_Truth")
        outputList.append("xAOD::MissingETAuxContainer#MET_TruthAux.")
        outputList.append("xAOD::MissingETContainer#MET_TruthRegions")
        outputList.append("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")
        outputList.append('xAOD::MissingETComponentMap#METMap_Truth')
        outputList.append('xAOD::MissingETAuxComponentMap#METMap_TruthAux.')

    # "Normal" MET collections
    from METReconstruction.METAssociatorCfg import METAssociatorCfg
    from METUtilities.METMakerConfig import getMETMakerAlg
    metDefs = ['AntiKt4EMTopo','AntiKt4LCTopo']
    if inputFlags.MET.DoPFlow:
       metDefs.append('AntiKt4EMPFlow')
    for metDef in metDefs:
        # Build association maps and core soft terms
        result.merge(METAssociatorCfg(inputFlags, metDef))
        outputList.append('xAOD::MissingETAssociationMap#METAssoc_'+metDef)
        outputList.append('xAOD::MissingETAuxAssociationMap#METAssoc_'+metDef+'Aux.')
        outputList.append('xAOD::MissingETContainer#MET_Core_'+metDef)
        outputList.append('xAOD::MissingETAuxContainer#MET_Core_'+metDef+'Aux.')
        # Build reference MET
        result.addEventAlgo(getMETMakerAlg(metDef), sequenceName='METAssoc_'+metDef)
        outputList.append('xAOD::MissingETContainer#MET_Reference_'+metDef)
        outputList.append('xAOD::MissingETAuxContainer#MET_Reference_'+metDef+'Aux.-ConstitObjectLinks.-ConstitObjectWeights')

    # Add the collections to the output stream
    from OutputStreamAthenaPool.OutputStreamConfig import addToAOD, addToESD
    result.merge(addToESD(inputFlags, outputList))
    if inputFlags.MET.WritetoAOD:
        result.merge(addToAOD(inputFlags, outputList))

    return result


if __name__=="__main__":
    # Set message levels
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    # Config flags steer the job at various levels
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC  = True
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc20e_13TeV/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.ESD.e4993_s3227_r12689/myESD.pool.root"]

    # Flags relating to multithreaded execution
    nthreads = 1
    ConfigFlags.Concurrency.NumThreads = nthreads
    if nthreads > 0:
        ConfigFlags.Concurrency.NumThreads = 1
        ConfigFlags.Concurrency.NumConcurrentEvents = 1
    ConfigFlags.MET.UseTracks = True
    ConfigFlags.MET.DoPFlow = True
    if ConfigFlags.Beam.Type in [BeamType.Cosmics, BeamType.SingleBeam]: # used to have " or not rec.doInDet()" on the end
        ConfigFlags.MET.UseTracks = False
        ConfigFlags.MET.DoPFlow = False
        print("METReconstruction_jobOptions: detected cosmics/single-beam configuration -- switch off track-based MET reco")

    ConfigFlags.lock()

    # Get a ComponentAccumulator setting up the fundamental Athena job
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(ConfigFlags)

    # Add the components for reading in pool files
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc("DetectorStore"))

    #Setup up general geometry
    modelConfig=ComponentAccumulator()
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    modelConfig=GeoModelCfg(ConfigFlags)
    cfg.merge(modelConfig)

    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    cfg.merge(AtlasFieldCacheCondAlgCfg(ConfigFlags))

    #Configure topocluster algorithmsm, and associated conditions
    from CaloRec.CaloTopoClusterConfig import CaloTopoClusterCfg
    cfg.merge(CaloTopoClusterCfg(ConfigFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(ConfigFlags,"totalNoise"))
    cfg.merge(CaloNoiseCondAlgCfg(ConfigFlags,"electronicNoise"))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    cfg.merge(TrackingGeometrySvcCfg(ConfigFlags))

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    cfg.merge(MuonGeoModelCfg(ConfigFlags))

    # Nowadays the jet calibration tool requires the EventInfo
    # to be decorated with lumi info, which is not in Run 2 AODs
    from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
    cfg.merge(LuminosityCondAlgCfg(ConfigFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    muWriter = CompFactory.LumiBlockMuWriter("LumiBlockMuWriter",LumiDataKey="LuminosityCondData")
    cfg.addEventAlgo(muWriter,"AthAlgSeq")

    # Get Jet Inputs
    from JetRecConfig.StandardJetConstits import stdConstitDic as cst
    from JetRecConfig import JetRecConfig
    for jetdef in [cst.EMTopoOrigin,cst.LCTopoOrigin,cst.EMPFlow]:
        cfg.merge(JetRecConfig.JetInputCfg(ConfigFlags,jetdef))

    # Need to rename the collections in the xAOD in order to avoid conflicts
    from SGComps.AddressRemappingConfig import InputRenameCfg
    cfg.merge(InputRenameCfg('xAOD::MissingETContainer','MET_Track','MET_Track_Old'))
    cfg.merge(InputRenameCfg('xAOD::MissingETAuxContainer','MET_TrackAux.','MET_Track_OldAux.'))
    cfg.merge(InputRenameCfg('xAOD::MissingETContainer','MET_EMTopo','MET_EMTopo_Old'))
    cfg.merge(InputRenameCfg('xAOD::MissingETAuxContainer','MET_EMTopoAux.','MET_EMTopo_OldAux.'))
    cfg.merge(InputRenameCfg('xAOD::MissingETContainer','MET_LocHadTopo','MET_LocHadTopo_Old'))
    cfg.merge(InputRenameCfg('xAOD::MissingETAuxContainer','MET_LocHadTopoAux.','MET_LocHadTopo_OldAux.'))

    cfg.merge(METCfg(ConfigFlags))

    outputlist = ["EventInfo#*"]
    outputlist+=["xAOD::MissingETContainer#"+"MET_Track","xAOD::MissingETAuxContainer#"+"MET_Track"+"Aux."]
    outputlist+=["xAOD::MissingETContainer#"+"MET_Track_Old","xAOD::MissingETAuxContainer#"+"MET_Track_Old"+"Aux."]
    outputlist+=["xAOD::MissingETContainer#"+"MET_EMTopo","xAOD::MissingETAuxContainer#"+"MET_EMTopo"+"Aux."]
    outputlist+=["xAOD::MissingETContainer#"+"MET_EMTopo_Old","xAOD::MissingETAuxContainer#"+"MET_EMTopo_Old"+"Aux."]
    outputlist+=["xAOD::MissingETContainer#"+"MET_AntiKt4EMPFlow","xAOD::MissingETAuxContainer#"+"MET_AntiKt4EMPFlow"+"Aux."]
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    cfg.merge(OutputStreamCfg(ConfigFlags,"xAOD",ItemList=outputlist))

    cfg.run(maxEvents=20)
