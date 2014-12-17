# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Specialist reconstruction and bytestream transforms
#  @author atlas-comp-jt-dev@cern.ch
#  @version $Id: overlayTransformUtils.py 636536 2014-12-17 16:49:33Z jchapman $

import os
import re
import subprocess
import sys
import tarfile

import logging
# Logging needs to be in the PyJobTransforms "namespace"
msg = logging.getLogger('PyJobTransforms.'+__name__.split('.')[-1])

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses
import PyJobTransforms.trfValidation as trfValidation

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.trfExe import athenaExecutor

## @brief write me
#  numbers
class BSJobSplitterExecutor(athenaExecutor):

    def preExecute(self, input = set(), output = set()):
        msg.debug('Preparing for execution of {0} with inputs {1} and outputs {2}'.format(self.name, input, output))

        # There are two ways to configure this transform:
        # - Give an inputBSFile argument directly
        # - Give a overlayConfigFile and jobNumber argument
        # Check now that we have a configuration that works

        if 'inputBSFile' in self.conf.argdict and 'overlayConfigFile' in self.conf.argdict:
            raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), 'Both inputBSFile and overlayConfigFile have been specified - please use only one.')

        if 'overlayConfigFile' in self.conf.argdict:
            if 'jobNumber' not in self.conf.argdict:
                raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), 'overlayConfigFile is specified, but no jobNumber was given.')
            # Job number has to wrap around from 500, dropping back to 1
            wrappedJobNumber = (self.conf.argdict['jobNumber'].value-1)%500 + 1

            self._inputFilelist = 'filelist_{0}.txt'.format(wrappedJobNumber)
            self._lblList = 'lbn_anal_map_{0}.txt'.format(wrappedJobNumber)

            try:
                f=tarfile.open(name=self.conf.argdict['overlayConfigFile'].value)
                f.extract('filelist_{0}.txt'.format(wrappedJobNumber))
                f.extract('lbn_anal_map_{0}.txt'.format(wrappedJobNumber))
                f.close()

                bsInputs = open(self._inputFilelist).readline().rstrip().split(',')
                self.conf.addToArgdict('inputBSFile', trfArgClasses.argBSFile(bsInputs, io='input', type='BS', subtype='BS'))
                self.conf.addToDataDictionary('BS', self.conf.argdict['inputBSFile'])
                input.add('BS')
                msg.info('Validating resolved input bytestream files')
                trfValidation.performStandardFileValidation({'BS': self.conf.argdict['inputBSFile']}, io='input')
            except Exception, e:
                raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), 'Error while unpacking and extracting input files for transform: {0}'.format(e))

            # Now setup correct input arguments
            self.conf.argdict['InputLbnMapFile'] = trfArgClasses.argString(self._lblList)
            self.conf.argdict['InputFileMapFile'] = trfArgClasses.argString(self._inputFilelist)

        else:
            #if 'lumiBlockMapFile' not in self.conf.argdict:
            #    raise trfExceptions.TransformSetupException(trfExit.nameToCode('TRF_EXEC_SETUP_FAIL'), 'inputBSFile is specified, but no lumiBlockMapFile was given.')
            #self.conf.argdict['InputLbnMapFile'] = self.conf.argdict['lumiBlockMapFile']
            if 'lumiBlockMapFile' in self.conf.argdict:
                self.conf.argdict['InputLbnMapFile'] = self.conf.argdict['lumiBlockMapFile']
        super(BSJobSplitterExecutor, self).preExecute(input=input, output=output)


    def execute(self):
        if 'maxFilesPerSubjob' in self.conf.argdict:
            self._hasExecuted = True
            msg.info('Activating job splitting with {0} files per subjob'.format(self.conf.argdict['maxFilesPerSubjob'].value))

            tmpFiles = self.conf.argdict['inputBSFile'].value
            self._subJobInputs = []
            while len(tmpFiles) > 0:
                self._subJobInputs.append(tmpFiles[0:self.conf.argdict['maxFilesPerSubjob'].value-1])
                del tmpFiles[0:self.conf.argdict['maxFilesPerSubjob'].value-1]

            #### TESTING TESTING!
            # self._subJobInputs = self._subJobInputs[0:2]
            ####

            msg.info('Job will be split into {0} pieces: {1}'.format(len(self._subJobInputs), self._subJobInputs))

            # Now loop over each subjob
            from PyJobTransforms.trfExe import athenaExecutor, executorConfig, bsMergeExecutor
            jobCounter = 0
            outputBSFileList = []
            for sjInputs in self._subJobInputs:
                sjobArgdict = {'triggerBit': self.conf.argdict['triggerBit'],
                              'InputLbnMapFile': self.conf.argdict['InputLbnMapFile'],
                              'checkEventCount': trfArgClasses.argSubstepBool('False', runarg=False)}

                sjOutput = self.conf.argdict['outputBS_SKIMFile'].value[0] + '_sub{0:03d}.tmp'.format(jobCounter)
                outputBSFileList.append(sjOutput)
                sjDataDictionary = {'BS': trfArgClasses.argBSFile(sjInputs, io='input', type='BS', subtype='BS'),
                                    'BS_SKIM':  trfArgClasses.argBSFile(sjOutput, io='output', type='BS', subtype='BS_SKIM')}

                sjconf = executorConfig(sjobArgdict, sjDataDictionary)
                sjexe = athenaExecutor(name='BSOverlayFilter_{0:03d}'.format(jobCounter), skeletonFile = 'EventOverlayJobTransforms/skeleton.BSOverlayFilter_tf.py',
                                       conf=sjconf, inData=set(['BS']), outData=set(['BS_SKIM']))
                sjexe.doAll(input=set(['BS']), output=set(['BS_SKIM']))
                jobCounter += 1

            # Now do the merging
            mrgArgdict = {'maskEmptyInputs': trfArgClasses.argBool(True, runarg=False),
                          'allowRename': trfArgClasses.argBool(True, runarg=False)}
            mrgDataDictionary = {'BS': trfArgClasses.argBSFile(outputBSFileList, io='input', type='BS', subtype='BS_SKIM'),
                                 'BS_MRG': self.conf.argdict['outputBS_SKIMFile']}
            mrgConf = executorConfig(mrgArgdict, mrgDataDictionary)
            mrgExe = bsMergeExecutor(name = 'RAWFileMerge', exe = 'file_merging', conf=mrgConf,
                                     inData = set(['BS']), outData = set(['BS_MRG']))
            mrgExe.doAll(input=set(['BS']), output=set(['BS_MRG']))

            # If we got here then all must be well, so we have executed successfully
            self._rc = 0

        else:
            # Ordinary run
            super(BSJobSplitterExecutor, self).execute()



### Add Argument Methods
def addOverlayBSFilterArguments(parser):
    from EventOverlayJobTransforms.overlayTrfArgs import addOverlayBSFilterArgs
    addOverlayBSFilterArgs(parser)

def addOverlay_PoolArguments(parser):
    from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs
    from EventOverlayJobTransforms.overlayTrfArgs import addOverlayTrfArgs, addOverlayPoolTrfArgs
    addBasicDigiArgs(parser)
    addForwardDetTrfArgs(parser)
    addOverlayTrfArgs(parser)
    addOverlayPoolTrfArgs(parser)

def addOverlay_BSArguments(parser):
    from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs
    from EventOverlayJobTransforms.overlayTrfArgs import addOverlayTrfArgs, addOverlayBSTrfArgs
    addBasicDigiArgs(parser)
    addForwardDetTrfArgs(parser)
    addOverlayTrfArgs(parser)
    addOverlayBSTrfArgs(parser)

### Add Sub-step Methods
def addOverlayBSFilterSubstep(executorSet):
    executorSet.add(BSJobSplitterExecutor(name = 'BSFilter', skeletonFile = 'EventOverlayJobTransforms/skeleton.BSOverlayFilter_tf.py', substep='overlayBSFilt',
                                          perfMonFile = 'ntuple.pmon.gz', inData = ['BS'], outData = ['BS_SKIM']))

def addOverlay_PoolSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'OverlayPool', skeletonFile = 'EventOverlayJobTransforms/skeleton.OverlayPool_tf.py',
                                   substep = 'overlayPOOL', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
                                   inData = ['HITS', 'RDO_BKG'], outData = ['RDO', 'RDO_SGNL']))

def addOverlay_BSSubstep(executorSet):
    executorSet.add(athenaExecutor(name = 'OverlayBS', skeletonFile = 'EventOverlayJobTransforms/skeleton.OverlayBS_tf.py',
                                   substep = 'overlayBS', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
                                   inData = [('HITS', 'BS')], outData = ['RDO', 'RDO_SGNL']))

### Append Sub-step Methods
def appendOverlayBSFilterSubstep(trf):
    executor = set()
    addOverlayBSFilterSubstep(executor)
    trf.appendToExecutorSet(executor)

def appendOverlay_PoolSubstep(trf):
    executor = set()
    addOverlay_PoolSubstep(executor)
    trf.appendToExecutorSet(executor)

def appendOverlay_BSSubstep(trf):
    executor = set()
    addOverlay_BSSubstep(executor)
    trf.appendToExecutorSet(executor)

