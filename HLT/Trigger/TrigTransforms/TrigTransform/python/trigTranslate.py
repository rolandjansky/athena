#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger translator to setup arguments for athenaHLT
# @details: to be used with Trig_tf_reco.py and trigRecoExe.py 
# @author: Mark Stockton

import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

import os
import ast

import PyJobTransforms.trfArgClasses as trfArgClasses
import PyJobTransforms.trfExceptions as trfExceptions
from PyJobTransforms.trfExitCodes import trfExit

#create option dict needed by athenaHLT from runargs
def getOption(runArgs,name, substep, first, output):

    # Dictionary to be filled to run athenaHLT from
    option = {}

    # Dictionary defining args: key=transform value=athenaHLT
    tfToAthenaHLT = {}
    tfToAthenaHLT['inputBS_RDOFile'] = 'file'
    tfToAthenaHLT['maxEvents'] = 'number-of-events'
    tfToAthenaHLT['skipEvents'] = 'skip-events'
    tfToAthenaHLT['runNumber'] = 'run-number'
    tfToAthenaHLT['precommand'] = 'precommand'
    tfToAthenaHLT['postcommand'] = 'postcommand'
    tfToAthenaHLT['eventmodifier'] = 'event-modifier'
    tfToAthenaHLT['jobOptionSvcType'] = 'joboptionsvc-type'
    tfToAthenaHLT['useDB'] = 'use-database'
    tfToAthenaHLT['DBtype'] = 'db-type'
    tfToAthenaHLT['DBserver'] = 'db-server'
    tfToAthenaHLT['DBsmkey'] = 'db-smkey'
    tfToAthenaHLT['DBhltpskey'] = 'db-hltpskey'
    ## Some arguments need specific code so aren't included here
    # save-output = outputBSFile (or filled by default in multi step tf)
    # db-extra = DBextra
    
    # Output needs the string not a list 
    # (as in PyJobTransforms/python/trfJobOptions.py)
    # For a multi step tf it can be defined by the transform itself rather than on command line
    if 'outputBSFile' in runArgs:
        option['save-output'] = runArgs['outputBSFile'].value[0]
    elif 'BS' in output:
        option['save-output'] = output['BS'].value[0]
    else:
        msg.warning('No BS filename defined, athenaHLT will not save the output')
    
    #DBextra needs to be a dictionary not a string
    if 'DBextra' in runArgs:
        option['db-extra'] = ast.literal_eval(runArgs['DBextra'].value)
        if 'DBlvl1pskey' in runArgs:
            if 'lvl1key' in option['db-extra']:
                raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'), 'Multiple definition of lvl1key as both --DBlvl1pskey %s and --DBextra %s' % (runArgs['DBlvl1pskey'],runArgs['DBextra'] ))
            else:
                option['db-extra']['lvl1key'] = runArgs['DBlvl1pskey']
    if 'DBlvl1pskey' in runArgs and 'db-extra' not in option:
        option['db-extra'] = {'lvl1key': runArgs['DBlvl1pskey']}
    
    # below based on PyJobTransforms/python/trfJobOptions.py
    for k in set(tfToAthenaHLT) & set(runArgs):
        v = runArgs[k]
        ## @note Substep type arguments are rather special, they apply to only named
        #  executors or substeps. We use the returnMyValue() method to sort out what
        #  specific value applies to us
        if isinstance(v, trfArgClasses.argSubstep):
            myValue = v.returnMyValue(name, substep, first)
            if myValue is not None:
                option[tfToAthenaHLT[k]]=myValue
        else:
            option[tfToAthenaHLT[k]]=v
    
    # Now make sure that if we did not add maxEvents then we set this to -1, which
    # avoids some strange defaults that only allow 5 events to be processed
    if tfToAthenaHLT['maxEvents'] not in option:
        option[tfToAthenaHLT['maxEvents']] = -1
        msg.info('maxEvents not defined, explicitly set to -1')
    
    # TODO - currently skips all the extra, literal, etc options
    # as these are not saved in options no point in being saved to file
    
    return option
    
#write option dict to runTranslate file
def writeTranslate(runTranslate,runArgs,name,substep,first,output):
    msg.info('Writing options to file \"%s\"' % runTranslate)
        
    option = getOption(runArgs, name, substep, first, output)  
        
    msg.info('Options set to: \"%s\":' % option)
    
    with open(runTranslate, 'w') as runTranslateFile:
        try:
            print >>runTranslateFile, os.linesep, "option = ", option
        except (IOError, OSError) as e:
            errMsg = 'Got an error when writing JO template {0}: {1}'.format(runTranslateFile, e)
            msg.error(errMsg)
            raise trfExceptions.TransformExecutionException(trfExit.nameToCode('TRF_EXEC_RUNARGS_ERROR'), errMsg)

