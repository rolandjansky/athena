# This comes after all Simplified Model setup files
from MadGraphControl.MadGraphUtils import SUSY_SM_Generation
from MadGraphControl.MadGraphUtils import build_param_card
try:
    from MadGraphControl.MadGraphUtils import update_lhe_file
except ImportError:
    print "Will not be able to update LHE file using this release"
try:
    from MadGraphControl.MadGraphUtils import run_madspin
except ImportError:
    print "Will not be able to run madspin on existing LHE file using this release"

# Set maximum number of events if the event multiplier has been modified
if evt_multiplier>0:
    if runArgs.maxEvents>0:
        nevts=runArgs.maxEvents*evt_multiplier
    else:
        nevts=5000*evt_multiplier

if njets<0:
    evgenLog.fatal('njets is not set')
    raise RuntimeError('njet is not set')

# systematic variation
if 'scup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[0]
elif 'scdw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[1]
elif 'alup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[2]
elif 'aldw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[3]
elif 'qcup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[6]
elif 'qcdw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[7]

# Set up for grid pack running
gridpackDirName=None
if hasattr(runArgs, "inputGenConfFile"):
    gridpackDirName='madevent/'

if 'writeGridpack' not in dir():
    writeGridpack=False

# Pass arguments as a dictionary: the "decays" argument is not accepted in older versions of MadGraphControl
argdict = {'runArgs'  : runArgs,
           'process'  : process,
           'gentype'  : gentype,
           'decaytype': decaytype,
           'masses'   : masses,
           'decays'   : decays,
           'fixEventWeightsForBridgeMode': fixEventWeightsForBridgeMode,
           'madspin_card' : madspin_card,
           }

if not SLHAonly and (not hasattr(runArgs,'inputGeneratorFile') or runArgs.inputGeneratorFile is None):
    # If it has not been set explicitly, set it now
    if not 'ickkw' in extras: extras['ickkw'] = 0

    argdict.update({'nevts'          : nevts,
                    'syst_mod'       : syst_mod,
                    'xqcut'          : xqcut,
                    'writeGridpack'  : writeGridpack,
                    'gridpackDirName': gridpackDirName,
                    'keepOutput'     : keepOutput,
                    'pdlabel'        : "'nn23lo1'",
                    'lhaid'          : 247000,
                    'extras'         : extras,
                    })
    try:
        [qcut,outputDS] = SUSY_SM_Generation(**argdict)

    except TypeError:
        # Older version of MadGraphControl
        try:
            del argdict['extras']
            [qcut,outputDS] = SUSY_SM_Generation(**argdict)
        except TypeError:
            del argdict['decays']
            [qcut,outputDS] = SUSY_SM_Generation(**argdict)

elif not hasattr(runArgs,'inputGeneratorFile') or runArgs.inputGeneratorFile is None:
    argdict.update({'nevts'     : 10000,
                    'syst_mod'  : None,
                    'xqcut'     : xqcut,
                    'SLHAonly'  : True,
                    'keepOutput': keepOutput,
                    'extras'    : extras,
                    })
    try:
        [qcut,outputDS] = SUSY_SM_Generation(**argdict)
    except TypeError:
        # Older version of MadGraphControl
        try:
            del argdict['extras']
            [qcut,outputDS] = SUSY_SM_Generation(**argdict)
        except TypeError:
            del argdict['decays']
            [qcut,outputDS] = SUSY_SM_Generation(**argdict)

else:
    build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='param_card_updated.dat',decays=decays)
    param_card_old = 'param_card_updated.dat'
    #param_card_old = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)
    qcut = -1
    import tarfile
    myTarball = tarfile.open(runArgs.inputGeneratorFile)
    myEvents = None
    for afile in myTarball.getnames():
        if afile.endswith('.events'): myEvents = afile
    if myEvents is None:
        print 'No input events file found!'
    else:
        events_file = myTarball.extractfile( myEvents )
        try:
            lhe_file_new_tmp = update_lhe_file(myEvents,param_card_old=param_card_old,lhe_file_new=None,masses=masses)
            print lhe_file_new_tmp
        except NameError:
            print "WARNING: Will not overwrite LHE file - make sure the LHE file you're using is specific to the model you're generating!"

        for aline in events_file:
            if 'ktdurham' in aline and "=" in aline:
                qcut = float(aline.split('=')[0].strip())
                break

    myTarball.close()

    if madspin_card is not None: run_madspin(madspin_card,runArgs,keepOutput)

    outputDS = runArgs.inputGeneratorFile


from __main__ import opts
if ((qcut<0 and njets>0) or outputDS is None or ''==outputDS) and not opts.config_only:
    evgenLog.warning('Looks like something went wrong with the MadGraph generation - bailing out!')
    raise RuntimeError('Error in MadGraph generation')

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts

runArgs.qcut = qcut
runArgs.inputGeneratorFile = outputDS
if 'syst_mod' in dir():
    runArgs.syst_mod = syst_mod
runArgs.decaytype = decaytype
runArgs.gentype = gentype

# Pythia8 setup
genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            ]
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on",
                                "Merging:nJetMax = "+str(njets),
                                "Merging:doKTMerging = on",
                                "Merging:TMS = "+str(qcut),
                                "Merging:ktType = 1",
                                "Merging:Dparameter = 0.4",
                                "Merging:nQuarksMerge = 4"]



# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.generators += ["EvtGen"]

if not hasattr(runArgs,'inputGeneratorFile'):
    print 'ERROR: something wasnt write in file name transfer from the fragment.'
    runArgs.inputGeneratorFile='madgraph.*._events.tar.gz'
evgenConfig.keywords += ["SUSY"]
if not opts.config_only:
    evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]
