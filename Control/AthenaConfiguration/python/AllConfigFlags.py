# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.SystemOfUnits import TeV
from AthenaConfiguration.AutoConfigFlags import GetFileMD


def _createCfgFlags():

    acf=AthConfigFlags()

    acf.addFlag('Input.Files', ["_ATHENA_GENERIC_INPUTFILE_NAME_",] ) # fromer global.InputFiles
    acf.addFlag('Input.isMC', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("isMC",None)) # former global.isMC
    acf.addFlag('Input.RunNumber', lambda prevFlags : list(GetFileMD(prevFlags.Input.Files).get("RunNumber",None))) # former global.RunNumber
    acf.addFlag('Input.ProjectName', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("Project","data17_13TeV") ) # former global.ProjectName

    acf.addFlag('Concurrency.NumProcs', 0)
    acf.addFlag('Concurrency.NumThreads', 0)
    acf.addFlag('Concurrency.NumConcurrentEvents', 0)

    acf.addFlag('Scheduler.CheckDependencies', True)
    acf.addFlag('Scheduler.ShowDataDeps', True)
    acf.addFlag('Scheduler.ShowDataFlow', True)
    acf.addFlag('Scheduler.ShowControlFlow', True)

    acf.addFlag('Common.isOnline', False ) #  Job runs in an online environment (access only to resources available at P1) # former global.isOnline

    def _checkProject():
        import os
        if "AthSimulation_DIR" in os.environ:
            return "AthSimulation"
        #TODO expand this method.
        return "Athena"
    acf.addFlag('Common.Project', _checkProject())

    # replace global.Beam*
    acf.addFlag('Beam.BunchSpacing', 25) # former global.BunchSpacing
    acf.addFlag('Beam.Type', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('beam_type','collisions') )# former global.BeamType
    acf.addFlag("Beam.NumberOfCollisions", lambda prevFlags : (GetFileMD(prevFlags.Input.Files)["/Digitization/Parameters"]["numberOfCollisions"] if prevFlags.Input.isMC \
                                                                   else (2. if prevFlags.Beam.Type=='collisions' else 0.))) # former global.NumberOfCollisions
    acf.addFlag('Beam.Energy', lambda prevFlags : GetFileMD(prevFlags.Input.Files).get('BeamEnergy',7*TeV)) # former global.BeamEnergy
    acf.addFlag('Beam.estimatedLuminosity', lambda prevFlags : ( 1E33*(prevFlags.Beam.NumberOfCollisions)/2.3 ) *\
        (25./prevFlags.Beam.BunchSpacing)) # former flobal.estimatedLuminosity


    acf.addFlag('Output.doESD', False) # produce ESD containers

    acf.addFlag('Output.EVNTFileName','myEVNT.pool.root')
    acf.addFlag('Output.HITSFileName','myHITS.pool.root')
    acf.addFlag('Output.RDOFileName','myROD.pool.root')
    acf.addFlag('Output.ESDFileName','myESD.pool.root')
    acf.addFlag('Output.AODFileName','myAOD.pool.root')
    acf.addFlag('Output.HISTFileName','myHIST.root')

#Detector Flags:
    def __detector():
        from AthenaConfiguration.DetectorConfigFlags import createDetectorConfigFlags
        return createDetectorConfigFlags()
    acf.addFlagsCategory( "Detector", __detector )

#Simulation Flags:
    def __simulation():
        from G4AtlasApps.SimConfigFlags import createSimConfigFlags
        return createSimConfigFlags()
    acf.addFlagsCategory( "Sim", __simulation )

#Geo Model Flags:
    acf.addFlag('GeoModel.Layout', 'atlas') # replaces global.GeoLayout
    acf.addFlag("GeoModel.AtlasVersion", lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("Geometry","ATLAS-R2-2016-01-00-01")) #
    acf.addFlag("GeoModel.Align.Dynamic", lambda prevFlags : (not prevFlags.Detector.Simulate))
    acf.addFlag("GeoModel.StripGeoType", "GMX") # Based on CommonGeometryFlags.StripGeoType
    acf.addFlag("GeoModel.Run","RUN2") # Based on CommonGeometryFlags.Run (InDetGeometryFlags.isSLHC replaced by GeoModel.Run=="RUN4")

#IOVDbSvc Flags:
    acf.addFlag("IOVDb.GlobalTag",lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("ConditionsTag","CONDBR2-BLKPA-2017-05"))
    from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)


    def __lar():
        from LArConfiguration.LArConfigFlags import createLArConfigFlags
        return createLArConfigFlags()
    acf.addFlagsCategory( "LAr", __lar ) 

#CaloNoise Flags
    acf.addFlag("Calo.Noise.fixedLumiForNoise",-1)
    acf.addFlag("Calo.Noise.useCaloNoiseLumi",True)
                

#TopoCluster Flags:
    acf.addFlag("Calo.TopoCluster.doTwoGaussianNoise",True)
    acf.addFlag("Calo.TopoCluster.doTreatEnergyCutAsAbsolute",False)
    acf.addFlag("Calo.TopoCluster.doTopoClusterLocalCalib",True)


    def __trigger():
        from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags
        return createTriggerFlags()
    acf.addFlagsCategory( "Trigger", __trigger )

    def __muon():
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        return createMuonConfigFlags()
    acf.addFlagsCategory( "Muon", __muon )

    def __egamma():
        from egammaConfig.egammaConfigFlags import createEgammaConfigFlags
        return createEgammaConfigFlags()
    acf.addFlagsCategory( "Egamma", __egamma )

    def __dq():
        from AthenaMonitoring.DQConfigFlags import createDQConfigFlags, createComplexDQConfigFlags
        dqf = createDQConfigFlags()
        dqf.join( createComplexDQConfigFlags() )
        return dqf
    acf.addFlagsCategory("DQ", __dq )

    return acf


ConfigFlags=_createCfgFlags()

del _createCfgFlags

if __name__=="__main__":
    import sys
    if len(sys.argv)>1:
        ConfigFlags.Input.Files = sys.argv[1:]
    else:
        ConfigFlags.Input.Files = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1",]
    
    ConfigFlags.loadAllDynamicFlags()
    ConfigFlags.initAll()
    ConfigFlags.dump()
    
