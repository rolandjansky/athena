# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @brief Module with standard reconstruction transform options and substeps

import argparse

import logging
msg = logging.getLogger(__name__)

import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfExe import athenaExecutor, DQMergeExecutor, reductionFrameworkExecutor, reductionFrameworkExecutorNTUP
from PyJobTransforms.trfArgs import addD3PDArguments, addPrimaryDPDArguments, addExtraDPDTypes, addReductionArguments


def addCommonRecTrfArgs(parser):
    parser.defineArgGroup('Common Reco', 'Common Reconstruction Options')
    parser.add_argument('--autoConfiguration', group='Common Reco', type=trfArgClasses.argFactory(trfArgClasses.argList), 
                        help='Autoconfiguration settings (whitespace or comma separated)', nargs='+', metavar='AUTOCONFKEY')
    parser.add_argument('--trigStream', group='Common Reco', type=trfArgClasses.argFactory(trfArgClasses.argList), 
                        help='Trigger stream setting')
    parser.add_argument('--topOptions', group='Common Reco', type=trfArgClasses.argFactory(trfArgClasses.argSubstep), 
                        nargs="+", help='Alternative top options file for reconstruction (can be substep specific)', 
                        metavar="substep:TOPOPTIONS")
    parser.add_argument('--valid', group='Common Reco', type=trfArgClasses.argFactory(trfArgClasses.argBool), 
                        help='Enable decorations for AOD that allow for enhanced physics validation', metavar='BOOL')


def addStandardRecoFiles(parser):
    parser.defineArgGroup('Reco Files', 'Reconstruction file options')
    parser.add_argument('--inputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input bytestream file', group='Reco Files')
    parser.add_argument('--inputDRAW_ZMUMUFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input skimmed Z->mumu bytestream', group='Reco Files')
    parser.add_argument('--inputDRAW_ZEEFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input skimmed Z->ee bytestream', group='Reco Files')
    parser.add_argument('--inputDRAW_EMUFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input'),
                        help='Input skimmed e+mu bytestream', group='Reco Files')
    parser.add_argument('--outputBSFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output'),
                        help='Output bytestream file', group='Reco Files')
    parser.add_argument('--inputRDOFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
                        help='Input RDO file', group='Reco Files')
    parser.add_argument('--inputESDFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input ESD file', group='Reco Files')
    parser.add_argument('--outputESDFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output ESD file', group='Reco Files')
    parser.add_argument('--outputRDO_TRIGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='output'),
                        help='Output RDO_TRIG file', group='Reco Files')
    parser.add_argument('--inputRDO_TRIGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
                        help='Input RDO_TRIG file', group='Reco Files')
    parser.add_argument('--inputRDO_FTKFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argRDOFile, io='input'),
                        help='Input RDO_FTK file', group='Reco Files')
    parser.add_argument('--inputAODFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input AOD file', group='Reco Files')
    parser.add_argument('--outputAODFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output AOD file', group='Reco Files')
    parser.add_argument('--outputAOD_RPRFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output AOD (reprocessed) file', group='Reco Files')
    parser.add_argument('--outputAOD_SKIMFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output'),
                        help='Output skimmed AOD file', group='Reco Files')
    parser.add_argument('--outputHISTFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output'), 
                        help='Output DQ monitoring file', group='Reco Files')
    parser.add_argument('--outputHIST_AODFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', countable=False), 
                        help='Output DQ monitoring file', group='Reco Files')
    parser.add_argument('--outputTAGFile', 
                        type=trfArgClasses.argFactory(trfArgClasses.argTAGFile, io='output'), 
                        help='Output TAG file', group='Reco Files')
    parser.add_argument('--outputNTUP_HTTIPFile',
                        type=trfArgClasses.argFactory(trfArgClasses.argHTTIPFile, io='output', multipleOK=True), help='Output HTT IP file', group='Reco Files')

    parser.add_argument('--inputEVNTFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input'),
                        help='Input EVNT file for NTUP_TRUTH making', group='Reco Files')
    parser.add_argument('--outputTXT_JIVEXMLTGZFile',
                        type = trfArgClasses.argFactory(trfArgClasses.argFile, io = 'output'),
                        help = 'Output JiveXML.tgz file', group = 'Reco Files')


## @brief Add reconstruction substeps to a set object
#  @note This is done in a separate function so that other transforms (full chain ones)
#  can import these steps easily
def addRecoSubsteps(executorSet):
    executorSet.add(athenaExecutor(name = 'RDOtoBS', skeletonFile = 'RecJobTransforms/skeleton.RDOtoBS_tf.py',
                                   substep = 'r2b', inData = ['RDO'], outData = ['BS']))
    executorSet.add(athenaExecutor(name = 'RDOtoRDOTrigger', skeletonFile = 'RecJobTransforms/skeleton.RDOtoRDOtrigger.py',
                                   substep = 'r2t', inData = ['RDO','RDO_FTK'], outData = ['RDO_TRIG']))
    # Note that the RAWtoALL substep is disabled by defaut (no inputs or outputs)
    # It will be enabled explicitly via --steering if required
    executorSet.add(athenaExecutor(name = 'RAWtoALL', skeletonFile = 'RecJobTransforms/skeleton.RAWtoALL_tf.py',
                                   substep = 'r2a', inData = [],
                                   outData = []))
    executorSet.add(athenaExecutor(name = 'RAWtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                   substep = 'r2e', inData = ['BS', 'RDO', 'DRAW_ZMUMU', 'DRAW_ZEE', 'DRAW_EMU', 'DRAW_RPVLL', 'RDO_FTK'], 
                                   outData = ['ESD', 'HIST_ESD_INT', 'TXT_JIVEXMLTGZ', 'NTUP_HTTIP'],))
    executorSet.add(athenaExecutor(name = 'ESDtoAOD', skeletonFile = 'RecJobTransforms/skeleton.ESDtoAOD_tf.py',
                                   substep = 'e2a', inData = ['ESD'], outData = ['AOD', 'HIST_AOD_INT']))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD_INT', 'HIST_AOD_INT'), 'HIST_R2A'], outData = ['HIST']))
    executorSet.add(athenaExecutor(name = 'ESDtoDPD', skeletonFile = 'PATJobTransforms/skeleton.ESDtoDPD_tf.py',
                                   substep = 'e2d', inData = ['ESD'], outData = []))
    executorSet.add(athenaExecutor(name = 'AODtoDPD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDPD_tf.py',
                                   substep = 'a2d', inData = ['AOD', 'EVNT'], outData = []))
    executorSet.add(athenaExecutor(name = 'AODtoAOD', skeletonFile = 'RecJobTransforms/skeleton.AODtoAOD_tf.py',
                                   substep = 'a2a', inData = ['AOD'], outData = ['AOD_RPR'],
                                   perfMonFile = None))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD', 'AOD_RPR'], outData = ['TAG'],))
    executorSet.add(athenaExecutor(name = 'AODtoHIST', skeletonFile = 'RecJobTransforms/skeleton.AODtoHIST_tf.py',
                                   inData = ['AOD'], outData = ['HIST_AOD'],))
    executorSet.add(reductionFrameworkExecutor(name = 'AODtoDAOD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDAOD_tf.py',
                                   substep = 'a2da', inData = ['AOD'], outData = ['DAOD']))
    executorSet.add(reductionFrameworkExecutorNTUP(name = 'NTUPtoRED', skeletonFile = 'PATJobTransforms/skeleton.NTUPtoRED_tf.py',
                                   substep = 'n2n', inData = ['NTUP_COMMON'], outData = ['DNTUP']))
    executorSet.add(reductionFrameworkExecutor(name = 'EVNTtoDAOD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDAOD_tf.py',
                                   substep = 'ev2da', inData = ['EVNT'], outData = ['DAOD']))



## @brief The standard suite of reconstruction specific arguments
#  @param trf The transform to which these arguments should be added
#  @param RAWtoALL Flag that will be passed to some of the automated argument type parsers
def addAllRecoArgs(trf, RAWtoALL=False):
    addCommonRecTrfArgs(trf.parser)
    addStandardRecoFiles(trf.parser)
    addPrimaryDPDArguments(trf.parser, transform = trf, RAWtoALL=RAWtoALL)
    addExtraDPDTypes(trf.parser, transform = trf)
    addReductionArguments(trf.parser, transform = trf)


## @brief Detect if the RAWtoALL steering argument is set
#  This is a very special case, requiring an unfortunately hacky solution... :-(
def detectRAWtoALL(argv):
    limitedParser = argparse.ArgumentParser(argument_default=argparse.SUPPRESS)
    limitedParser.add_argument("--steering", nargs="+")
    limitedParser.add_argument("--argJSON")
    limitedParser.add_argument("--AMIConfig", "--AMI")
    
    args, unknown = limitedParser.parse_known_args(argv)
    args=vars(args)
    
    # Note if we have steering and it does _not_ contain doRAWtoALL we
    # return false, as CLI overrides JSON and AMI
    if "steering" in args:
        if "doRAWtoALL" in args["steering"]:
            return True
        else:
            return False
    
    # JSON overrides AMI
    if "argJSON" in args:
        import json
        from PyJobTransforms.trfUtils import convertToStr
        argfile = open(args['argJSON'], 'r')
        jsonParams = json.load(argfile)
        jsonParams = convertToStr(jsonParams)
        if "steering" in jsonParams:
            if "doRAWtoALL" in jsonParams["steering"]:
                return True
            else:
                return False
        
    if "AMIConfig" in args:
        from PyJobTransforms.trfAMI import TagInfo
        tag=dict(TagInfo(args['AMIConfig']).trfs[0])
        if "steering" in tag and "doRAWtoALL" in tag["steering"]:
            return True
        
    return False
