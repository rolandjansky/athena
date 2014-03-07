# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: Nicolas Berger, based on Reco_trf auto-generated jobOptions

from LArCafJobs.CafRunArgs import CafRunArgs

runArgs = CafRunArgs(CafJobInputs)

##================================================
## Below: For HI running only!!
##===============================================
#runArgs.preInclude += ['RecJobTransforms/HeavyIonDataRecConfig.py']
#runArgs.preExec += ['InDetFlags.useBeamConstraint.set_Value_and_Lock(True)']
##===============================================

runArgs.preInclude += ['LArSamplesMon/samples_tweaks.py']
runArgs.postInclude += ['LArSamplesMon/samplesNtuple.py']

include("RecJobTransforms/skeleton.RAWtoESD.py")


