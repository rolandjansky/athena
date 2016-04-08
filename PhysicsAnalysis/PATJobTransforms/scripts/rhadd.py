#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# $Id: rhadd.py 677431 2015-06-23 08:09:12Z graemes $
#
# Recursive historgam adder, wrapping around hadd
# Author: Graeme A Stewart <graeme.andrew.stewart@cern.ch>
#
# usage: rhadd.py [-h] [-n BUNCHNUMBER] [-p PARALLELMERGE]
#                 outputFile inputFiles [inputFiles ...]
#
# Recursive wrapper around the ROOT hadd script.
#
# positional arguments:
#   outputFile            Single merged output file
#   inputFiles            Input files to merge
#
# optional arguments:
#   -h, --help            show this help message and exit
#   -n BUNCHNUMBER, --bunchNumber BUNCHNUMBER
#                         File batching number for single hadds
#   -p PARALLELMERGE, --parallelMerge PARALLELMERGE
#                         Number of merges to do in parallel (experimental,
#                         please do not use in production)


import argparse
import logging
import os
import sys

from multiprocessing import Pool
from subprocess import Popen, STDOUT, PIPE
from tempfile import mkstemp

logging.basicConfig(level=logging.INFO)

class haddJob(object):
    '''A single hadd job'''
    def __init__(self, inputFiles, outputFile):
        self._inputFiles = inputFiles
        self._outputFile = outputFile
        self._exitCode = None
        
    @property
    def outputFile(self):
        return(self._outputFile)
    
    @property
    def exitCode(self):
        return self._exitCode
    
    @exitCode.setter
    # This is needed to paste the parallel executed exitCode back to the 'parent' instance
    def exitCode(self, value):
        self._exitCode = value
        
    def exe(self):
        '''Execute the hadd command'''
        mergeCmd = ['hadd', '-f']  # -f because our output file was touched to be safely created
        mergeCmd.append(self._outputFile)
        mergeCmd.extend(self._inputFiles)
        
        logging.info('Will now execute merge: %s' % ' '.join(mergeCmd))
        output = []
        job = Popen(mergeCmd, stdout=PIPE, stderr=STDOUT, bufsize=1, close_fds=True)
        while job.poll() == None:
            output.append(job.stdout.readline().strip())
        self._exitCode = job.returncode
        if self._exitCode != 0:
            logging.warning('Non zero return code from hadd. STDOUT/ERR follows:\n%s' % os.linesep.join(output))

    def __str__(self):
        return str(self._inputFiles) + ' -> ' + str(self._outputFile)
    
    def __call__(self):
        '''Wrapper to call my own exe function and return the exit code of hadd'''
        self.exe()
        return self._exitCode
            
        
class haddStep(object):
    '''An hadd iteration - takes a bunch of input files and produces a bunch of output files'''
    def __init__(self, inputFiles, bunchNumber, finalOutputFile, level = None):
        self._inputFiles = inputFiles
        self._bunchNumber = bunchNumber
        self._finalOutputFile = finalOutputFile
        self._haddJobArray = []
        self._level = level
        self._defineMergeJobs()
    
    
    def _defineMergeJobs(self):
        # How many merges to do in this step?
        nMerges = (len(self._inputFiles)-1) / self._bunchNumber + 1
        logging.debug('Need %d merges for level %d' % (nMerges, self._level))
        if nMerges == 1:
            logging.debug('Final merge job: %s -> %s' % (self._inputFiles, self._inputFiles))
            self._haddJobArray.append(haddJob(self._inputFiles, self._finalOutputFile))
            return
        
        # With >1 merge need temporary files
        nextFile = 0
        for job in range(nMerges):
            # Try to ensure we have ~equal numbers of files in each merge
            fileCounter = len(self._inputFiles) * float(job+1) / nMerges
            # Add 0.5 to ensure that rounding errors don't lose a file off the back... (very unlikely!)
            lastFile = int(fileCounter + 0.5)
            tempOutput = mkstemp(dir='.', prefix='tmp.')
            os.close(tempOutput[0])
            logging.debug('Intermediate merge job %d: %s -> %s' % (job, self._inputFiles[nextFile:lastFile], tempOutput[1]))
            self._haddJobArray.append(haddJob(self._inputFiles[nextFile:lastFile], tempOutput[1]))
            nextFile = lastFile
        
        
    def executeAll(self, parallel = 1):
        if parallel > 1:
            # Funky parallel processing
            logging.info('Starting merge using up to %d hadd processes in parallel' % parallel)
            logging.warning('Parallel merging is experimental')
            pool = Pool(processes = parallel)
            parallelResultsArray = []
            for job in self._haddJobArray:
                parallelResultsArray.append(pool.apply_async(job, ()))
            pool.close()
            # The next two lines will stick until all the worker processes are finished
            # Really one needs a progress loop monitor with a timeout... 
            pool.join()
            
            # Update our hadd exit codes to the parallel processed return code, because the copy of the 
            # instance held by the worker was the one where the exe method was actually called
            for i, job in enumerate(self._haddJobArray):
                job.exitCode = parallelResultsArray[i].get(timeout=0)
            
            for job in self._haddJobArray:
                if job.exitCode != 0:
                    logging.error('Merging job %s failed, exit code %s' % (job, job.exitCode))
                    sys.exit(1)
        else:
            # Safe and slow serial processing
            for job in self._haddJobArray:
                job.exe()
                if job.exitCode != 0:
                    logging.error('Merging job %s failed, exit code %s' % (job, job.exitCode))
                    sys.exit(1)


    @property
    def outputFiles(self):
        return [ job.outputFile for job in self._haddJobArray ]
    
    @property
    def numHadds(self):
        return len(self._haddJobArray)
    
    def __str__(self):
        return 'Merging level %s: %s' % (self._level, str([ str(job) for job in self._haddJobArray ]))


def main():
    parser = argparse.ArgumentParser(description='Recursive wrapper around the ROOT hadd script.',
                                     epilog='Return codes: 0 All OK; 1 Problem with hadd; 2 Invalid arguments')
    parser.add_argument('outputFile', help='Single merged output file')
    parser.add_argument('inputFiles', nargs='+', help='Input files to merge')
    parser.add_argument('-n', '--bunchNumber', type=int, help='File batching number for single hadds', default=10)
    parser.add_argument('-p', '--parallelMerge', type=int, 
                        help='Number of merges to do in parallel (experimental, please do not use in production)', default=1)
    
    args = vars(parser.parse_args(sys.argv[1:]))

    logging.debug(args)
    
    # Sanity checks
    if args['bunchNumber'] <= 1:
        logging.error('bunchNumber parameter must be greater than 1')
        sys.exit(2)

    if args['parallelMerge'] < 1:
        logging.error('parallelMerge parameter must be greater than 1')
        sys.exit(2)

        
    doRecursiveMerge(args)
    
    sys.exit(0)
    
    
def doRecursiveMerge(args):
    '''Setup the cascade of merge jobs and execute each level in turn'''
    # First setup the cascade of merge steps
    jobGraph = []
    level = 0
    jobGraph.append(haddStep(bunchNumber = args['bunchNumber'], 
                             inputFiles = args['inputFiles'], 
                             finalOutputFile = args['outputFile'], level = level))
    
    while jobGraph[-1].numHadds > 1:
        level += 1
        jobGraph.append(haddStep(bunchNumber = args['bunchNumber'], 
                                 inputFiles = jobGraph[-1].outputFiles, 
                                 finalOutputFile = args['outputFile'], level = level))
        logging.debug(jobGraph[-1])

    # Now execute each merge stage in turn
    for i, jobs in enumerate(jobGraph):
        logging.info('Executing merge interation step %d' % i)
        jobs.executeAll(args['parallelMerge'])
        
    logging.info('Final merge completed successfully.')


if __name__ == '__main__':
    main()
