# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags
from AthenaCommon.SystemOfUnits import TeV
from AthenaConfiguration.AutoConfigFlags import GetFileMD


def _createCfgFlags():

    acf=AthConfigFlags()
#Global Flags:

    acf.addFlag('global.InputFiles',["_ATHENA_GENERIC_INPUTFILE_NAME_",])
    acf.addFlag('global.isOnline',False)
    acf.addFlag('global.GeoLayout',"atlas")
    acf.addFlag('global.ProjectName',lambda prevFlags : GetFileMD(prevFlags.get("global.InputFiles")).get("Project","data17_13TeV"))
    acf.addFlag('global.isMC',True)
    acf.addFlag('global.BunchSpacing',25) # Bunch spacing in ns
    acf.addFlag("global.NumberOfCollisions",0) #"Number of collisions per beam crossing. Should be 2.3*(L/10**33)*(bunchSpacing/25 ns)
    acf.addFlag("global.BeamType",'collisions') # "Specify data taking type: 'collisions' (default), 'singlebeam','cosmics'")
    acf.addFlag("global.BeamEnergy",lambda prevFlags : GetFileMD(prevFlags.get("global.InputFiles")).get("BeamEnergy",7*TeV))
    acf.addFlag("global.estimatedLuminosity",lambda prevFlags : 1E33*(prevFlags.get("global.NumberOfCollisions")/2.3)* \
                    (25./prevFlags.get("global.BunchSpacing")))
    
#Geo Model Flags:
    acf.addFlag("GeoModel.AtlasVersion",lambda prevFlags : GetFileMD(prevFlags.get("global.InputFiles")).get("Geometry","ATLAS-R2-2016-01-00-01"))


#IOVDbSvc Flags:
    acf.addFlag("IOVDb.GlobalTag",lambda prevFlags : GetFileMD(prevFlags.get("global.InputFiles")).get("ConditionsTag","CONDBR2-BLKPA-2017-05"))
    from IOVDbSvc.IOVDbAutoCfgFlags import getDatabaseInstanceDefault
    acf.addFlag("IOVDb.DatabaseInstance",getDatabaseInstanceDefault)


#LAr Flags:
    acf.addFlag("LAr.doAlign",lambda prevFlags : prevFlags.get("global.GeoLayout")=="atlas")
    acf.addFlag("LAr.doHVCorr",lambda prevFlags : not prevFlags.get("global.isMC"))
    acf.addFlag("LAr.doCellEmMisCalib",lambda prevFlags : prevFlags.get("global.isMC"))
    

#CaloNoise Flags
    acf.addFlag("Calo.Noise.fixedLumiForNoise",-1)
    acf.addFlag("Calo.Noise.useCaloNoiseLumi",True)
                

#TopoCluster Flags:
    acf.addFlag("Calo.TopoCluster.doTwoGaussianNoise",True)
    acf.addFlag("Calo.TopoCluster.doTreatEnergyCutAsAbsolute",False)


# Trigger
    acf.addFlag("Trigger.inputLVL1ConfigFile", "UNSPECIFIED")
    acf.addFlag("Trigger.L1Decoder.doMuon", True) # should have dependency on detector flag doCalo
    acf.addFlag("Trigger.L1Decoder.doCalo", True) 
    acf.addFlag("Trigger.L1Decoder.forceEnableAllChains", True)


    return acf


ConfigFlags=_createCfgFlags()

del _createCfgFlags

if __name__=="__main__":
    import sys
    if len(sys.argv)>1:
        ConfigFlags.set("global.InputFiles",sys.argv[1:])
    else:
        ConfigFlags.set("global.InputFiles",[ "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/CommonInputs/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1",])
    ConfigFlags.initAll()
    ConfigFlags.dump()
    
