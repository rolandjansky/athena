#######################################################
# @file: McParticleAlgs/McAodValidation_jobOptions.py
# @purpose: jobO fragment to configure the validation
#           algorithm (and its tools) for the McAOD
# @author: Sebastien Binet <binet@cern.ch>
# @date: March 2006
#######################################################

# get a handle on the job main sequence
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from McParticleAlgs.tests import test_McAodValidation
job += test_McAodValidation()
job.McAodValidation.OutputLevel = INFO
