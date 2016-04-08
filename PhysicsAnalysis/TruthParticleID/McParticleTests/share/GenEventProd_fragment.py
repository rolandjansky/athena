#######################################################
# @file: McParticleTests/GenEventProd_fragment.py
# @purpose: jobO fragment to generate a process among a list of
#           available processes (Pythia, Herwig, ttbar, Z+jets)
# @author: Sebastien Binet <binet@cern.ch>
# @date: August 2006
#######################################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from McParticleTests.tests import makeGenEvents
job += makeGenEvents( genName    = GENERATOR,
                      genProcess = PROCESS,
                      cfgGenName = "EvGen" )

