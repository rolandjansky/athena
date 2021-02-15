#!/usr/bin/env python

# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger translator to setup arguments for athenaHLT
# @details: to be used with Trig_tf_reco.py and trigRecoExe.py
# @author: Mark Stockton

import PyJobTransforms.trfArgClasses as trfArgClasses
# TODO for check of prescale keys
# import PyJobTransforms.trfExceptions as trfExceptions
# from PyJobTransforms.trfExitCodes import trfExit

# Setup logging here
import logging
msg = logging.getLogger("PyJobTransforms." + __name__)

# create option dict needed by athenaHLT from runargs
def getOption(runArgs, name, substep, first, output):

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
    tfToAthenaHLT['useDB'] = 'use-database'
    tfToAthenaHLT['DBserver'] = 'db-server'
    tfToAthenaHLT['DBsmkey'] = 'smk'
    tfToAthenaHLT['DBhltpskey'] = 'hltpsk'
    tfToAthenaHLT['DBl1pskey'] = 'l1psk'
    ## Some arguments need specific code so aren't included here
    # save-output = outputBSFile (or filled by default in multi step tf)

    # Output needs the string not a list
    # (as in PyJobTransforms/python/trfJobOptions.py)
    # For a multi step tf it can be defined by the transform itself rather than on command line
    if 'outputBSFile' in runArgs:
        option['save-output'] = runArgs['outputBSFile'].value[0]
    elif 'BS' in output:
        option['save-output'] = output['BS'].value[0]
    else:
        msg.warning('No BS filename defined, athenaHLT will not save the output')

    # TODO (ATR-11854) l1psk, hltpsk, smk should be compared to triggerConfig
    # example below based on old comparison but needs work to retrieve keys and do comparisons of all three keys
    # if 'triggerConfig' in runArgs:
    #     retrieve keys from triggerConfig string
    #     if 'lvl1key' in triggerConfig:
    #         if 'DBlvl1pskey' in runArgs:
    #         add check to compare DBlvl1pskey to lvl1key from triggerConfig
    #             raise trfExceptions.TransformArgException(trfExit.nameToCode('TRF_ARG_ERROR'), 'Inconsistent definition of lvl1key in --DBlvl1pskey {0} and --triggerConfig {1}'.format(runArgs['DBlvl1pskey'], runArgs['triggerConfig']))
    #         else:
    #             set lvl1key in triggerConfig
    # if 'DBlvl1pskey' in runArgs and 'triggerConfig' not in option:
    #     set lvl1key in triggerConfig

    # below based on PyJobTransforms/python/trfJobOptions.py
    for k in set(tfToAthenaHLT) & set(runArgs):
        v = runArgs[k]
        ## @note Substep type arguments are rather special, they apply to only named
        #  executors or substeps. We use the returnMyValue() method to sort out what
        #  specific value applies to us
        if isinstance(v, trfArgClasses.argSubstep):
            myValue = v.returnMyValue(name, substep, first)
            if myValue is not None:
                option[tfToAthenaHLT[k]] = myValue
        else:
            # return just the value to avoid returning all the properties (e.g. isRunArg=True)
            option[tfToAthenaHLT[k]] = v.value

    # Now make sure that if we did not add maxEvents then we set this to -1, which
    # avoids some strange defaults that only allow 5 events to be processed
    if tfToAthenaHLT['maxEvents'] not in option:
        option[tfToAthenaHLT['maxEvents']] = -1
        msg.info('maxEvents not defined, explicitly set to -1')

    # Skips all the other runArgs (extra, literal, etc)
    # as these are for running with athena not athenaHLT

    return option

# return option list to be used as command line for athenaHLT jobs
# In Run2 this was handled by producing runTranslate file which is no longer needed
def getTranslated(runArgs, name, substep, first, output):
    option = getOption(runArgs, name, substep, first, output)
    msg.info('Options set to: \"%s\":', option)
    optionList = list()
    for k, v in option.items():
        item = "--{0}={1}"
        if(type(v) == list):
            v = ''.join(v)
        optionList.append(item.format(k, v))

    # Replace --use-database=True with no argument version
    if '--use-database=True' in optionList:
        optionList.remove('--use-database=True')
        optionList.append('--use-database')

    return optionList
