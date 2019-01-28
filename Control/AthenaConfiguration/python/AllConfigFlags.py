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
    acf.addFlag('Scheduler.ShowDataDeps', False)
    acf.addFlag('Scheduler.ShowDataFlow', False)
    acf.addFlag('Scheduler.ShowControlFlow', False)

    acf.addFlag('Common.isOnline', False ) #  Job runs in an online environment (access only to resources available at P1) # former global.isOnline

    # replace global.Beam*
    acf.addFlag('Beam.BunchSpacing', 25) # former global.BunchSpacing
    acf.addFlag("Beam.NumberOfCollisions",0) # former global.NumberOfCollisions
    acf.addFlag('Beam.Type', 'collisions') # former global.BeamType
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


#Geo Model Flags:
    acf.addFlag('GeoModel.Layout', 'atlas') # replaces global.GeoLayout
    acf.addFlag("GeoModel.AtlasVersion", lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("Geometry","ATLAS-R2-2016-01-00-01")) #


#IOVDbSvc Flags:
    acf.addFlag("IOVDb.GlobalTag",lambda prevFlags : GetFileMD(prevFlags.Input.Files).get("ConditionsTag","CONDBR2-BLKPA-2017-05"))
    from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)


#LAr Flags:
    try:
        import LArCellRec # Suppress flake8 unused import warning: # noqa: F401
        haveLArCellRec = True
    except ImportError:
        haveLArCellRec = False

    if haveLArCellRec:
        from LArCellRec.LArConfigFlags import createLArConfigFlags
        lcf=createLArConfigFlags()
        acf.join(lcf)

#CaloNoise Flags
    acf.addFlag("Calo.Noise.fixedLumiForNoise",-1)
    acf.addFlag("Calo.Noise.useCaloNoiseLumi",True)
                

#TopoCluster Flags:
    acf.addFlag("Calo.TopoCluster.doTwoGaussianNoise",True)
    acf.addFlag("Calo.TopoCluster.doTreatEnergyCutAsAbsolute",False)


# Trigger
    try:
        import TriggerJobOpts # Suppress flake8 unused import warning: # noqa: F401
        haveTriggerJobOpts = True
    except ImportError:
        haveTriggerJobOpts = False

    if haveTriggerJobOpts:
        from TriggerJobOpts.TriggerConfigFlags import createTriggerFlags
        acf.join( createTriggerFlags() )

# Muon 
    try:
        import MuonConfig # Suppress flake8 unused import warning: # noqa: F401
        haveMuonConfig = True
    except ImportError:
        haveMuonConfig = False

    if haveMuonConfig:
        from MuonConfig.MuonConfigFlags import createMuonConfigFlags
        acf.join( createMuonConfigFlags() )

# DQ
    try:
        import AthenaMonitoring # Suppress flake8 unused import warning: # noqa: F401
        haveDQConfig = True
    except ImportError:
        haveDQConfig = False

    if haveDQConfig:
        from AthenaMonitoring.DQConfigFlags import createDQConfigFlags, createComplexDQConfigFlags
        acf.join( createDQConfigFlags() )
        createComplexDQConfigFlags(acf)

    return acf


ConfigFlags=_createCfgFlags()

del _createCfgFlags

if __name__=="__main__":
    import sys
    if len(sys.argv)>1:
        ConfigFlags.Input.Files = sys.argv[1:]
    else:
        ConfigFlags.Input.Files = [ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1",]
    
    ConfigFlags.initAll()
    ConfigFlags.dump()
    
