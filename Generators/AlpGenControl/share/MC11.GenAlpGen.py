# Set up for generic alpgen generation
#  This is the main job option to call

import sys

UseHerwig=True
rand_seed=1
ecm = 3500.

from AthenaCommon import Logging

log = Logging.logging.getLogger('AlpGenGenericGeneration')

if not hasattr(runArgs,'runNumber'):
    log.fatal('No run number found.  Generation will fail.  No idea how to decode this.')
    raise RunTimeError('No run number found.')

if not hasattr(runArgs,'randomSeed'):
    log.error('Random seed not set.  Will use 1.')
else:
    rand_seed=runArgs.randomSeed

if not hasattr(runArgs,'skipEvents'):
    log.info('No skip events argument found.  Good!')
else:
    log.fatal('Skip events argument found.  No idea how to deal with that yet.  Bailing out.')
    raise RunTimeError('Skip events argument found.')

if not hasattr(runArgs,'ecmEnergy'):
    log.warning('No center of mass energy found.  Will use the default of 7 TeV.')
else:
    ecm = runArgs.ecmEnergy / 2.

# Set up run based on run number
run_num=int(runArgs.runNumber)
process=''
special1=None
special2=None
nwarm = [ 4 , 1000000 ]


# Run numbers currently reserved: 147001 - 147151

# W+jets samples obo L. Mijovic
if run_num>=147001 and run_num<=147048:
    log.info('Recognized W+jets run number.  Will generate for run '+str(run_num))
    process = 'wjet'
    jets = [(run_num-147001)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-147001)>=25: UseHerwig=False # Move to Pythia for half the samples

    #               10 min  30 min  55 min  9 hrs    10 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ 4 , ncwarm[jets[0]] ]
    events_alpgen = event_numbers[jets[0]]
    
    # 0:5000  1:5000  2:5000  3:5000  4:5000 

    events_athena = 5000
    if jets[0]==4: events_athena=750 
    if jets[0]==5: events_athena=200 # Should work
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100
    
    # Ok, just kidding, we aren't doing 7 partons, we're doing 5 inclusive
    if jets[0]==7: jets[0]=5

    W_mode = int((run_num-147001)/8)%3+1
    special1 = 'iwdecmode %i          ! W decay mode (1: el, 2: mu, 3: tau)\n '%(W_mode)
    special2 = special1

# TTbar samples obo J. Ferrando
if run_num>=147049 and run_num<=147072:
    log.info('Recognized ttbar run number.  Will generate for run '+str(run_num))
    process = '2Q'
    jets = [(run_num-147049)%6,1] # Exclusive matching for Np0-Np4
    if jets[0]==5: jets[1]=0 # Inclusive matching for Np5

    if run_num-147049>=12: UseHerwig=False # Move to Pythia for half the samples
    if (run_num-147049)%12<6: # Use fully leptonic top decays
        special1 = """ihvy      6
itdecmod 5
itdec  1
"""
        special2 = special1
    else: # Use semi-leptonic top decays
        special1 = """ihvy     6
itdec      1
itdecmod   4
"""
        special2=special1

    #    Pythia terminating early due to VERY low efficiency
    #                 16 min   32 min   ~20hrs    60 hrs    XXXX    XXXX 
    #                 30 min   2 hrs    92 hrs  XXXX     XXXX    XXXX  
    event_numbers = [2000000,4000000,27000000,50000000,50000000,50000000,
                     4000000,8000000,200000000,200000000,200000000,50000000]
    events_alpgen = event_numbers[ (run_num-147049)%12 ]
    events_athena = 500
    if jets[1]<2: events_athena=5000 # Np0/Np1 can produce 5k events
    if jets[1]==2: events_athena=2500

# Z+jets samples - thanks to O. Rosenthal
if run_num>=147073 and run_num<=147136:
    log.info('Recognized Z+jets run number.  Will generate for run '+str(run_num))
    process = 'zjet'
    jets = [(run_num-147001)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-147001)>=32: UseHerwig=False # Move to Pythia for half the samples

    event_numbers=[1000000,5000000,5000000,2500000,2500000,10000000,10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ ncwarm[jets[0]] , event_numbers[jets[0]] ]
    events_alpgen = event_numbers[jets[0]]

    events_athena = 5000
    if jets[0]==2: events_athena=2500
    if jets[0]==3: events_athena=1000
    if jets[0]==4: events_athena=500
    if jets[0]==5: events_athena=100
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100

    # Ok, just kidding, we aren't doing 7 partons, we're doing 5 inclusive
    if jets[0]==7: jets[0]=5

    Z_mode = int((run_num-147001)/8)%4+1
    special1 = """mllmin  60           ! Minimum M_ll
mllmax  2000          ! Maximum M_ll 
"""
    if Z_mode<3: special1 += """ilep 0        ! Use leptons in the final state (not neutrinos)
izdecmode %i          ! Z decay mode (1: ee, 2: mumu, 3: tautau)\n """%(Z_mode)
    else: special1 += """ilep 1       ! Use neutrinos in the final state (not leptons) \n """
    special2 = special1


if process=='':
    log.fatal('Unknown run number!  Bailing out!')
    raise RunTimeError('Unknown run number.')

if not UseHerwig:  
        special1 += """xlclu   0.26          ! lambda for alpha_s in CKKW scale evaluation
lpclu 1         ! nloop for alphas in CKKW scale evaluation
""" 
        special2 += """xlclu   0.26          ! lambda for alpha_s in CKKW scale evaluation
lpclu 1         ! nloop for alphas in CKKW scale evaluation
""" 

from AlpGenControl.AlpGenUtils import SimpleRun

log.info('Running AlpGen on process '+process+' with ECM='+str(ecm)+' with '+str(nwarm)+' warm up rounds')
log.info('Will generate '+str(events_alpgen)+' events with jet params '+str(jets)+' and random seed '+str(rand_seed))
if special1 is not None:
    log.info('Special settings for mode 1:')
    log.info(special1)
if special2 is not None:
    log.info('Special settings for mode 2:')
    log.info(special2)

SimpleRun( proc_name=process , events=events_alpgen , nwarm=nwarm , energy=ecm , jets=jets , seed = rand_seed , special1=special1 , special2=special2 )

if hasattr(runArgs,'inputGeneratorFile'):
    log.info('About to clobber old input generator file argument, '+str(runArgs.inputGeneratorFile))
runArgs.inputGeneratorFile='alpgen.XXXXXX.TXT.v1'

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

from AthenaCommon.AlgSequence import AlgSequence
topAlg = AlgSequence("TopAlg") 

if not UseHerwig:
    include ( "MC11JobOptions/MC11_PythiaPerugia2011C_Common.py" )
    
    Pythia.PythiaCommand += ["pyinit user alpgen",
                             "pydat1 parj 90 20000.",
                             "pydat3 mdcy 15 1 0",
                             "pypars mstp 143 1",
                             "pydat1 parj 90 20000",   # prevent photon emission in parton showers
                             "pydat1 mstj 26 0"        # turn off B mixing (done by EvtGen)
                             ]
else:
    include ( "MC11JobOptions/MC11_AlpgenJimmy_Common_7TeV.py" )
    
    Herwig.HerwigCommand += [ "iproc alpgen",
                              "taudec TAUOLA",
                              "mixing 0",  # Switch off B-mixing
                            ]

include ( "MC11JobOptions/MC11_Tauola_Fragment.py" )
include ( "MC11JobOptions/MC11_Photos_Fragment.py" )

from MC11JobOptions.AlpgenEvgenConfig import evgenConfig
evgenConfig.inputfilebase = 'alpgen.XXXXXX.TXT.v1'
if not UseHerwig:
       evgenConfig.generators = ["Alpgen", "Pythia"]
evgenConfig.efficiency = 0.90000
evgenConfig.minevents = events_athena

