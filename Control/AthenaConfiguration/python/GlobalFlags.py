from AthenaConfiguration.ConfigFlags import ConfigFlag,makeFlag
from AthenaCommon.SystemOfUnits import TeV

isMC=makeFlag('isMC',True)

isOnline=makeFlag('isOnline',False)

GeoLayout=makeFlag('GeoLayout',"atlas")

projectName=makeFlag("ProjectName","data17_13TeV")     #should autoconfigure! 

InputFiles=makeFlag("InputFiles",["_ATHENA_GENERIC_INPUTFILE_NAME_",])


#Beam-related flags:
bunchSpacing=makeFlag("BunchSpacing",25,"Bunch spacing in ns")

numberOfCollisions=makeFlag("numberOfCollisions",0, "Number of collisions per beam crossing. Should be 2.3*(L/10**33)*(bunchSpacing/25 ns)")

beamType=makeFlag("beamType",'collisions', "Specify data taking type: 'collisions' (default), 'singlebeam','cosmics'")

beamEnergy=makeFlag("beamEnergy",7*TeV)

class estimatedLuminosity(ConfigFlag):
    def getDefault(self,prevFlags):
        return 1E33*(prevFlags.get("AthenaConfiguration.GlobalFlags.numberOfCollisions")/2.3)* \
            (25./prevFlags.get("AthenaConfiguration.GlobalFlags.bunchSpacing"))
                     

