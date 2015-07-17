#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Trig_tf_reco.py - based on PyJobTransforms/Reco_tf.py now stored here:
## https://svnweb.cern.ch/trac/atlasoff/browser/Reconstruction/RecJobTransforms/trunk/scripts

import sys
import time

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor, DQMergeExecutor, reductionFrameworkExecutor, reductionFrameworkExecutorNTUP
from PyJobTransforms.trfArgs import addAthenaArguments, addD3PDArguments, addPrimaryDPDArguments, addExtraDPDTypes, addDetectorArguments, addReductionArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace
from RecJobTransforms.recTransformUtils import addCommonRecTrfArgs, addStandardRecoFiles

import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigTransform.trigRecoExe import trigRecoExecutor
from TrigTransform.trigCostExe import trigCostExecutor

@stdTrfExceptionHandler
@sigUsrStackTrace
def main():
    
    msg.info('This is %s' % sys.argv[0])

    trf = getTransform()
    trf.parseCmdLineArgs(sys.argv[1:])
    trf.execute()
    trf.generateReport()

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trf.exitCode))
    sys.exit(trf.exitCode)

def getTransform():
    executorSet = set()
        
    #BSRDOtoRAW is new option for trigger transform
    #now setup to run athenaHLT, so TODO is to remove:
    #writeBS BSRDOInput EvtMax from AthenaCommon...  athenaCommonFlags....
    executorSet.add(trigRecoExecutor(name = 'BSRDOtoRAW', skeletonFile = 'TriggerRelease/runHLT_standalone.py',
                                     exe = 'athenaHLT.py',
                                     substep = 'b2r', tryDropAndReload = False,
                                     inData = ['BS_RDO', 'RDO'], outData = ['BS', 'HIST_HLTMON','HIST_DEBUGSTREAMMON'], 
                                     perfMonFile = 'ntuple_BSRDOtoRAW.pmon.gz',
                                     literalRunargs = ['writeBS = runArgs.writeBS',
                                                       'BSRDOInput = runArgs.inputBS_RDOFile',
                                                       'EvtMax = runArgs.maxEvents',
                                                       'if hasattr(runArgs,\'outputBSFile\'):',
                                                       '   from AthenaCommon.AthenaCommonFlags import athenaCommonFlags',
                                                       '   athenaCommonFlags.BSRDOOutput.set_Value_and_Lock( runArgs.outputBSFile )']))

    #RAWtoCOST is new option for trigger transform
    #runs in athena and will succeed if input BS file has costmon enabled
    executorSet.add(trigCostExecutor(name = 'RAWtoCOST', skeletonFile = 'TrigCostMonitor/readTrigCost.py',
                                     substep = 'r2c',
                                     inData = ['BS'], outData = ['NTUP_TRIGCOST','NTUP_TRIGRATE','NTUP_TRIGEBWGHT'],
                                     perfMonFile = 'ntuple_RAWtoCOST.pmon.gz',
                                     literalRunargs = ['BSRDOInput = runArgs.inputBSFile',
                                                       'EvtMax = runArgs.maxEvents']))

    #add default reconstruction steps
    # eventually to be replaced by:
    #from RecJobTransforms.recTransformUtils import addRecoSubsteps
    #addRecoSubsteps(executorSet)
       
    # one difference to reco_tf is that NTUP_TRIG has to be added - needs pyjobtf update before can use above    
        #NTUP_TRIG is added as is not available in ATLASP1HLT, but is available in the reco release
        #hence can be used later in a ATLASP1HLT job if switch releases

    executorSet.add(athenaExecutor(name = 'RAWtoESD', skeletonFile = 'RecJobTransforms/skeleton.RAWtoESD_tf.py',
                                   substep = 'r2e', inData = ['BS', 'RDO'], outData = ['ESD', 'HIST_ESD_INT'], 
                                   perfMonFile = 'ntuple_RAWtoESD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'ESDtoAOD', skeletonFile = 'RecJobTransforms/skeleton.ESDtoAOD_tf.py',
                                   substep = 'e2a', inData = ['ESD'], outData = ['AOD', 'HIST_AOD_INT'], 
                                   perfMonFile = 'ntuple_ESDtoAOD.pmon.gz'))
    executorSet.add(DQMergeExecutor(name = 'DQHistogramMerge', inData = [('HIST_ESD_INT', 'HIST_AOD_INT')], outData = ['HIST']))
    executorSet.add(athenaExecutor(name = 'ESDtoDPD', skeletonFile = 'PATJobTransforms/skeleton.ESDtoDPD_tf.py',
                                   substep = 'e2d', inData = ['ESD'], outData = ['NTUP_TRIG'],
                                   perfMonFile = 'ntuple_ESDtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoDPD', skeletonFile = 'PATJobTransforms/skeleton.AODtoDPD_tf.py',
                                   substep = 'a2d', inData = ['AOD', 'EVNT'], outData = ['NTUP_TRIG'],
                                   perfMonFile = 'ntuple_AODtoDPD.pmon.gz'))
    executorSet.add(athenaExecutor(name = 'AODtoTAG', skeletonFile = 'RecJobTransforms/skeleton.AODtoTAG_tf.py',
                                   inData = ['AOD'], outData = ['TAG'],))
#     executorSet.add(athenaExecutor(name = 'AODtoHIST', skeletonFile = 'RecJobTransforms/skeleton.FROM_PETER.py',
#                                    inData = ['AOD'], outData = ['HIST_AOD'],))
    executorSet.add(reductionFrameworkExecutor(name = 'AODtoRED', skeletonFile = 'PATJobTransforms/skeleton.AODtoRED_tf.py',
                                   substep = 'a2r', inData = ['AOD'], outData = ['DAOD_RED']))
    executorSet.add(reductionFrameworkExecutorNTUP(name = 'NTUPtoRED', skeletonFile = 'PATJobTransforms/skeleton.NTUPtoRED_tf.py',
                                   substep = 'n2n', inData = ['NTUP_COMMON'], outData = ['NTUP_RED']))
    
    trf = transform(executor = executorSet, description = 'Trigger transform to run HLT_standalone, followed by'
                    ' general purpose ATLAS reconstruction transform. Input to HLT_Standalone is inputBS_RDOFile'
                    ' with outputs of RDO, ESD, AOD or DPDs. For more details on reco_tf, see:'
                    ' https://twiki.cern.ch/twiki/bin/viewauth/Atlas/RecoTf')
    
    #add arguments as donw in reco_tf
    addAthenaArguments(trf.parser)
    addDetectorArguments(trf.parser)
    addCommonRecTrfArgs(trf.parser)
    addStandardRecoFiles(trf.parser)
    addPrimaryDPDArguments(trf.parser, transform = trf)
    addD3PDArguments(trf.parser, transform = trf)
    addExtraDPDTypes(trf.parser, transform = trf)
    addReductionArguments(trf.parser, transform = trf)

    #currently ignoring simulation arguments as can't find the package when have below in cmt/requirements:
    #use SimuJobTransforms SimuJobTransforms-* Simulation
    
    # For digi step - make sure we can add the digitisation/simulation arguments
    # before we add this substep
#    try:
#        from SimuJobTransforms.simTrfArgs import addForwardDetTrfArgs, addBasicDigiArgs, addPileUpTrfArgs, addCommonSimDigTrfArgs
#        addBasicDigiArgs(trf.parser)
#        addForwardDetTrfArgs(trf.parser)
#        addPileUpTrfArgs(trf.parser)
#        #addCommonSimDigTrfArgs(trf.parser)
#        executorSet.add(athenaExecutor(name = 'HITtoRDO', skeletonFile = 'SimuJobTransforms/skeleton.HITtoRDO.py',
#                                       substep = 'h2r', tryDropAndReload = False, perfMonFile = 'ntuple.pmon.gz',
#                                       inData = ['Hits'], outData = ['RDO'],
#                                       runtimeRunargs = {'LowPtMinbiasHitsFile' : 'runArgs.inputLowPtMinbiasHitsFile',
#                                                         'HighPtMinbiasHitsFile' : 'runArgs.inputHighPtMinbiasHitsFile',}))
#        
#    except ImportError:
#        msg.warning('Failed to import simulation/digitisation arguments. These substeps will not be available.')

    #now add specific trigger arguments
    #  putting this last makes them appear last in the help so easier to find
    addTriggerArgs(trf.parser)
    addTriggerDBArgs(trf.parser)
    addDebugArgs(trf.parser)

    return trf


def addTriggerArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Trigger', 'Specific options related to the trigger configuration used for reprocessing')
    
    #new for trigger transform
    #now setup to run athenaHLT, so TODO is to remove:testPhysicsV4 and writeBS
    #TODO: testPhysicsV4 needs deleting as causes double menu loading but left in for now to not add conflicts to the panda tag page
    parser.add_argument('--testPhysicsV4', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Please do not use this command, to be deleted', group='Trigger')
    parser.add_argument('--writeBS', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Needed if running BSRDO to BS step in athena (default: True)', group='Trigger', default=trfArgClasses.argBool(True, runarg=True))
    parser.add_argument('--inputBS_RDOFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input', runarg=True, type='bs'),
                        help='Input bytestream file', group='Trigger')
    #without an outputBSFile name specified then any further steps will know to use tmp.BS
    parser.add_argument('--outputBSFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', runarg=True, type='bs'),
                        help='Output bytestream file', group='Trigger')
    parser.add_argument('--outputHIST_HLTMONFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='Output HLTMON file', group='Trigger')    
    #NTUP_TRIG is added as is not available in ATLASP1HLT, but is available in the reco release
    #hence can be used later in a ATLASP1HLT job if switch releases
    parser.add_argument('--outputNTUP_TRIGFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False), 
                        help='D3PD output NTUP_TRIG file (can be made in substeps e2d,a2d)', group='Trigger')
    #NTUP_COST is added as is not available in ATLASP1HLT, but is available in the reco release
    #hence can be used later in a ATLASP1HLT job if switch releases
    parser.add_argument('--outputNTUP_TRIGCOSTFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False), 
                        help='D3PD output NTUP_TRIGCOST file', group='Trigger')
    #NTUP_RATE is added as is not available in ATLASP1HLT, but is available in the reco release
    #hence can be used later in a ATLASP1HLT job if switch releases
    parser.add_argument('--outputNTUP_TRIGRATEFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False), 
                        help='D3PD output NTUP_TRIGRATE file', group='Trigger')
    #NTUP_TRIGEBWGHT is added as is not available in ATLASP1HLT, but is available in the reco release
    #hence can be used later in a ATLASP1HLT job if switch releases
    parser.add_argument('--outputNTUP_TRIGEBWGHTFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False), 
                        help='D3PD output NTUP_TRIGEBWGHT file', group='Trigger')
    parser.add_argument('--triggerConfig', nargs='+', metavar='substep=TRIGGERCONFIG',
                        type=trfArgClasses.argFactory(trfArgClasses.argSubstep, runarg=True, separator='='),
                        help='Trigger Configuration String. '
			'N.B. This argument uses EQUALS (=) to separate the substep name from the value.', group='Trigger')
    parser.add_argument('--dumpOptions', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Only for testing: '
                        'dump to stdout the options athenaHLT has received. '
                        'N.B. option consistency is not checked.', group='Trigger')
    parser.add_argument('--precommand', nargs='+', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='precommand for athenaHLT aka -c', group='Trigger')
    parser.add_argument('--postcommand', nargs='+', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='postcommand for athenaHLT aka -C', group='Trigger')
    parser.add_argument('--eventmodifier', nargs='+', type=trfArgClasses.argFactory(trfArgClasses.argList, runarg=True),
                        help='event-modifier for athenaHLT aka -Z', group='Trigger')
    parser.add_argument('--prodSysBSRDO', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='For prodsys to make sure uses inputBS_RDOFile rather than inputBSFile when running the b2r step', group='Trigger')


def addTriggerDBArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('TriggerDB', 'Specific options related to the trigger DB')
    
    #TODO add short forms and help messages
        
    parser.add_argument('--jobOptionSvcType', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='jobOptionSvcType aka -J', group='TriggerDB')
    parser.add_argument('--useDB', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='useDB', group='TriggerDB')
    parser.add_argument('--DBtype', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBtype', group='TriggerDB')
    parser.add_argument('--DBserver', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBserver', group='TriggerDB')
    parser.add_argument('--DBsmkey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBsmkey', group='TriggerDB')
    parser.add_argument('--DBhltpskey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBhltpskey', group='TriggerDB')
    parser.add_argument('--DBlvl1pskey', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBlvl1pskey', group='TriggerDB')
    parser.add_argument('--DBextra', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
                        help='DBextra', group='TriggerDB')


def addDebugArgs(parser):
    # Use arggroup to get these arguments in their own sub-section (of --help)
    parser.defineArgGroup('Debug', 'Specific options related to the trigger debug recovery')

    parser.add_argument('--debug_stream', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                        help='Runs debug_stream analysis', group='Debug')
    parser.add_argument('--outputHIST_DEBUGSTREAMMONFile', nargs='+', 
                        type=trfArgClasses.argFactory(trfArgClasses.argHISTFile, io='output', runarg=True, countable=False),
                        help='Output DEBUGSTREAMMON file', group='Debug')    



    
#below commented out after cleanup to latest reco_tf as don't believe needed
#    
# This is copied from Digi_tf, but should be in a loadable module
#def addDigiArgs(parser):
#    # Use arggroup to get these arguments in their own sub-section (of --help)
#    parser.defineArgGroup('Digi', 'Digitisation specific options')
#    parser.add_argument('--inputHitsFile', nargs='+',
#                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='hits'),
#                        help='Input HITS file', group='Digi')
#    parser.add_argument('--outputRDOFile', nargs='+',
#                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='output', runarg=True, type='rdo'),
#                        help='Output RDO file', group='Digi')
#    parser.add_argument('--digiSeedOffset1',
#                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
#                        help='Offset for first random seed', group='Digi')
#    parser.add_argument('--digiSeedOffset2',                                                             
#                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
#                        help='Offset for second random seed', group='Digi')
#    parser.add_argument('--digiRndmSvc',
#                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), ##STRING CHOICES ARG,
#                        help='Random Number Service to use.', group='Digi' )
#    parser.add_argument('--samplingFractionDbTag',
#                        type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
#                        help='This argument can be used to override the PhysicsList retrieved from the Simulation metadata. This information is used in the LAr sampling fraction data base tag used by LArfSamplG4Phys.', group='Digi')
#    parser.add_argument('--doAllNoise',
#                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
#                        help='Overall control of noise simulation during digitization - useful for overlay jobs, will set doCaloNoise, doMuonNoise, doInDetNoise digitizationFlags.', group='Digi')
#    parser.add_argument('--AddCaloDigi',
#                        type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
#                        help='Save Calo Digits too, not just RawChannels.', group='Digi')
#    parser.add_argument('--triggerConfig',
#                        type=trfArgClasses.argFactory(trfArgClasses.argSubstep, runarg=True),
#                        help='Trigger Configuration String.', group='Digi')
#    parser.add_argument('--DataRunNumber',
#                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
#                        help='Override existing run number with this value - deprecated?', group='Digi')
#    parser.add_argument('--jobNumber',
#                        type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True),
#                        help='The number of this job in the current RunDependentSimulation task.', group='Digi')
#    parser.add_argument('--LowPtMinbiasHitsFile',
#                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='hits', executor=['HITtoRDO']),
#                        help='Files with low Pt hits', group='Digi')
#    parser.add_argument('--HighPtMinbiasHitsFile',
#                        type=trfArgClasses.argFactory(trfArgClasses.argPOOLFile, io='input', runarg=True, type='hits', executor=['HITtoRDO']),
#                        help='Files with high Pt hits', group='Digi')
#    parser.add_argument('--numberOfLowPtMinBias',
#                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
#                        help='Number of low Pt minbias events', group='Digi')
#    parser.add_argument('--numberOfHighPtMinBias',
#                        type=trfArgClasses.argFactory(trfArgClasses.argFloat, runarg=True),
#                        help='Number of high Pt minbias events', group='Digi')

if __name__ == '__main__':
    main()
