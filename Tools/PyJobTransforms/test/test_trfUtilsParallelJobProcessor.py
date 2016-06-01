#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_ParallelJobProcessor.py
#  @brief Unittests for trfUtils.py ParallelJobProcessor
#  @author william.dmitri.breaden.madden@cern.ch

import inspect
import fnmatch
import os
import re
import sys
import unittest
import logging

from PyUtils import RootUtils
from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfArgClasses import argFile
import PyJobTransforms.trfExceptions as trfExceptions

#import PyJobTransforms.trfUtils as trfUtils
from PyJobTransforms.trfUtils import *

from PyJobTransforms.trfLogger import msg
from PyJobTransforms.trfLogger import stdLogLevels, stdLogLevelsByCritcality

from PyJobTransforms.trfExitCodes import trfExit
import PyJobTransforms.trfExceptions as trfExceptions


## @brief a simple function that returns a string and sleeps a specified time
#  @param sleepTime sleep time specification
def helloWorld(
    sleepTime = 5
    ):
    time.sleep(sleepTime)
    return "hello world"


## @brief a simple function that returns a product of multiplication
#  @param multiplicand1 multiplicand
#  @param multiplicand1 multiplicand
def multiply(
    multiplicand1 = 0,
    multiplicand2 = 0
    ):
    return multiplicand1 * multiplicand2


## @brief a simple function that returns its arguments
#  @param **kwargs arguments
def returnArguments(**kwargs):
    time.sleep(5)
    return kwargs


## @brief a simple function sleeps a specified time
#  @param sleepTime sleep time specification
def timeout(
    sleepTime = 30
    ):
    time.sleep(sleepTime)
    return 0


## @brief a simple function that raises a RuntimeError exception
def exception():
    raise RuntimeError("FAIL")
    return 1


## @brief unit tests for the parallel job processor
class TestParallelJobProcessor(unittest.TestCase):
    # Disable for now
    pass
    
    
    ## @brief unit test for working functions
    #  @detail This method is a unit test of the parallel job processor
    #  testing the processing of two simple, working functions.
#     def test_working(self):
#         msg.info("\n\n\n\nPARALLEL JOB PROCESSOR WORKING TEST")
#         jobGroup1 = JobGroup(
#             name = "working functions test",
#             jobs = [
#                 Job(
#                     name = "hello world function",
#                     workFunction = helloWorld,
#                     workFunctionKeywordArguments = {
#                         'sleepTime': 1,
#                     },
#                     workFunctionTimeout = 10
#                 ),
#                 Job(
#                     name = "multiplication function",
#                     workFunction = multiply,
#                     workFunctionKeywordArguments = {
#                         'multiplicand1': 2,
#                         'multiplicand2': 3
#                     },
#                     workFunctionTimeout = 10
#                 )
#             ]
#         )
#         parallelJobProcessor1 = ParallelJobProcessor()
#         parallelJobProcessor1.submit(jobSubmission = jobGroup1)
#         results = parallelJobProcessor1.getResults()
#         self.assertEquals(results, ['hello world', 6])
#     ## @brief unit test for timeout function
#     #  @detail This method is a unit test of the parallel job processor
#     #  testing the processing of a simple function that is used to cause a
#     #  timeout.
#     def test_timeout(self):
#         msg.info("\n\n\n\nPARALLEL JOB PROCESSOR TIMEOUT TEST")
#         jobGroup1 = JobGroup(
#             name = "timeout test",
#             jobs=[
#                 Job(
#                     name = "timeout function",
#                     workFunction = timeout,
#                     workFunctionKeywordArguments = {
#                         'sleepTime': 30,
#                     },
#                     workFunctionTimeout = 1
#                 )
#             ]
#         )
#         parallelJobProcessor1 = ParallelJobProcessor()
#         parallelJobProcessor1.submit(jobSubmission = jobGroup1)
#         self.assertRaises(
#             trfExceptions.TransformTimeoutException,
#             parallelJobProcessor1.getResults
#         )
#     ## @brief unit test for fail function
#     #  @detail This method is a unit test of the parallel job processor
#     #  testing the processing of a simple function that raises an exception.
#     def test_fail(self):
#         msg.info("\n\n\n\nPARALLEL JOB PROCESSOR FAIL TEST")
#         jobGroup1 = JobGroup(
#             name = "fail test",
#             jobs=[
#                 Job(
#                     name = "fail function",
#                     workFunction = exception,
#                     workFunctionTimeout = 5
#                 )
#             ]
#         )
#         parallelJobProcessor1 = ParallelJobProcessor()
#         parallelJobProcessor1.submit(jobSubmission = jobGroup1)
#         self.assertRaises(
#             trfExceptions.TransformExecutionException,
#             parallelJobProcessor1.getResults
#         )
#     ## @brief unit test for job information
#     #  @detail This method is a unit test of the job object that raises an
#     #  exception when a work function is not specified.
#     def test_job_information(self):
#         msg.info("\n\n\n\nPARALLEL JOB PROCESSOR JOB INFORMATION TEST")
#         self.assertRaises(
#             trfExceptions.TransformInternalException,
#             Job
#         )


def main():
    msg.level = logging.DEBUG
    unittest.main()


if __name__ == '__main__':
    main()
