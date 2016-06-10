#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_ParallelJobProcessorData.py
#  @brief Unittests for output file validation using trfUtils.py
#  ParallelJobProcessor
#  @author william.dmitri.breaden.madden@cern.ch

import subprocess
import os
import os.path
import sys
import unittest
import logging

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfLogger import stdLogLevels, stdLogLevelsByCritcality

# data files for unit tests
fileAOD1  = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/AOD.01448536._003819.pool.root.1"
fileAOD2  = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/AOD.01448536._003819.pool.root.1"
fileAOD3  = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/AOD.01448536._003819.pool_corrupted.root.1" # (corrupted for test purposes)
fileESD1  = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/my.ESD.pool.root"
fileHIST1 = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/my.HIST.root"
fileTAG1  = "/afs/cern.ch/work/g/graemes/ddm/parallelValidate/my.TAG.pool.root"
fileBS1   = "/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201556.physics_JetTauEtmiss.merge.RAW._lb0423._SFO-1._0001.1"


## @brief unit tests for output data file validation using the parallel job
#  processor
class TestOutputFileValidationInParallel(unittest.TestCase):
    # Disable for now
    pass
    
# 
#     ## @brief unit test for AOD
#     #  @detail This method is a unit test for output file validation of AOD data
#     #  using the parallel job processor.
#     def test_AOD(self):
#         msg.info("\n\n\n\nAOD OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputAODFile',
#             fileAOD1,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for ESD
#     #  @detail This method is a unit test for output file validation of ESD data
#     #  using the parallel job processor.
#     def test_ESD(self):
#         msg.info("\n\n\n\nESD OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputESDFile',
#             fileESD1,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for HIST
#     #  @detail This method is a unit test for output file validation of HIST
#     #  data using the parallel job processor.
#     def test_HIST(self):
#         msg.info("\n\n\n\nHIST OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputHISTFile',
#             fileHIST1,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for TAG
#     #  @detail This method is a unit test for output file validation of TAG data
#     #  using the parallel job processor.
#     def test_TAG(self):
#         msg.info("\n\n\n\nTAG OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputTAGFile',
#             fileTAG1,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for BS
#     #  @detail This method is a unit test for output file validation of BS data
#     #  using the parallel job processor.
#     def test_BS(self):
#         msg.info("\n\n\n\nBS OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputBSFile',
#             fileBS1,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for multiple AOD files
#     #  @detail This method is a unit test for output file validation of AOD data
#     #  using the parallel job processor.
#     def test_AOD_multiple_file__parallel_validations(self):
#         msg.info("\n\n\n\nAOD MULTIPLE OUTPUT FILE VALIDATIONS USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputAODFile',
#             fileAOD1,
#             fileAOD2,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 0)
# 
#     ## @brief unit test for corrupted AOD
#     #  @detail This method is a unit test for output file validation of AOD data
#     #  using the parallel job processor.
#     def test_AOD_corrupted(self):
#         msg.info("\n\n\n\nCORRUPTED AOD OUTPUT FILE VALIDATION USING PARALLEL JOB PROCESSOR")
#         cmd = [
#             'ValidateFiles_tf.py',
#             '--outputAODFile',
#             fileAOD3,
#             '--parallelFileValidation',
#             'True',
#             '--verbose'
#         ]
#         p = subprocess.Popen(
#             cmd,
#             shell = False,
#             stdout = subprocess.PIPE,
#             stderr = subprocess.STDOUT,
#             bufsize = 1
#         )
#         while p.poll() is None:
#             line = p.stdout.readline()
#             sys.stdout.write(line)
#         # Clean remaining buffered output lines.
#         for line in p.stdout:
#             sys.stdout.write(line)
#         self.assertEqual(p.returncode, 75)


def main():
    msg.level = logging.DEBUG
    unittest.main()


if __name__ == '__main__':
    main()
