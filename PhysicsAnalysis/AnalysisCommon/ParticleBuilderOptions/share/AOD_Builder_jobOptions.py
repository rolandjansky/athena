from AthenaCommon.Resilience import treatException,protectedInclude
from ParticleBuilderOptions.AODFlags import AODFlags

from AthenaCommon.GlobalFlags import globalflags

if AODFlags.JetTag:
    protectedInclude( "BTagging/BTagging_jobOptions.py" )

#if AODFlags.SpclMC:
#    protectedInclude( "McParticleAlgs/TruthParticleBuilder_jobOptions.py" )

# Fast Simulation AOD
if AODFlags.FastSimulation:
   try:
      include( "ParticleBuilderOptions/runFastSim.py" )
      from AtlfastAlgs.GlobalEventDataGetter import Atlfast__GlobalEventDataGetter
      GlobalEventDataMaker = Atlfast__GlobalEventDataGetter().AlgHandle()
      GlobalEventDataMaker.McLocation = AODFlags.McEventKeyStr()
      include( "AtlfastConversionAlgs/AtlfastConversion_jobOptions.py" )
      if AODFlags.MissingEtTruth or AODFlags.FastTrackParticle:
         protectedInclude( "RecAthenaPool/RecAthenaPool_joboptions.py" )
      if AODFlags.MissingEtTruth:
         protectedInclude( "ParticleBuilderOptions/MissingEtTruth_jobOptions.py" )
   except Exception:
      treatException("Could not load FastSim" )

   from FastSimulationConfig.FastSimulationFlags import jobproperties
   if jobproperties.FastSimulation.doAtlfastICorrection == True:
       include ( "AtlfastCorrectionAlgs/AtlfastCDivide.py" )

from CaloRec.CaloCellFlags import jobproperties
if jobproperties.CaloCellFlags.doFastCaloSim.statusOn:
    protectedInclude ("AtlfastAlgs/Atlfast_FastCaloSim_MuonFragment.py")

