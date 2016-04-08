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
special3=None
nwarm = [ 4 , 1000000 ]

# Run numbers currently reserved: 147001 - 147151
# W+jets samples obo L. Mijovic
if run_num>=147001 and run_num<=147048:
    log.info('Recognized W+jets run number.  Will generate for run '+str(run_num))
    process = 'wjet'
    jets = [(run_num-147001)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-147001)>=24: UseHerwig=False # Move to Pythia for half the samples

    #               10 min  30 min  55 min  9 hrs    10 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ ncwarm[jets[0]] , event_numbers[jets[0]] ]
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
                     2000000,4000000,27000000,50000000,50000000,50000000]
    events_alpgen = event_numbers[ (run_num-147049)%12 ]
    events_athena = 500
    nwarm[1] = events_alpgen
    if jets[0]<2: events_athena=5000 # Np0/Np1 can produce 5k events
    if jets[0]==2: events_athena=2500

# DY Filtered sample - thanks to B. Di Micco
UseDilFilter=False
ForceLeptonicTau=False

if run_num>=146860 and run_num<=146886:
    UseDilFilter=True
    run_min=146860
    event_numbers=[11000000,2000000,250000000,131000000,65000000,30000000,10000000,10000000,10000000]
 
    jets = [(run_num-run_min)%10,1]
    events_athena = 5000
 
    if jets[0]==2: events_athena=1000
    if jets[0]==3: events_athena=200
    if jets[0]==4: events_athena=10
    if jets[0]==5: events_athena=50
 
    if (run_num>=146880 and run_num<=146886):
       event_numbers=[22000000,60000000,430000000,210000000,70000000,30000000,10000000,10000000,10000000]
       if jets[0]==0: events_athena=10
       if jets[0]==1: events_athena=1000
       if jets[0]==2: events_athena=10
       if jets[0]==3: events_athena=20
 
    nwarm = [ jets[0]+2 , 1000000 ]
    run_num=(run_num-run_min)+146830

# DY Sample - thanks to B. Di Micco
ZJET=False
if run_num>=146830 and run_num<=146856:
    ZJET=True
    if not UseDilFilter:
        event_numbers=[120000,400000,20000000,90000000,70000000,20000000,10000000,10000000,10000000]

    run_min=146830
    jets = [(run_num-run_min)%10,1] # Exclusive matching to the last number of partons   
    events_athena = 5000
    if jets[0]==3: events_athena=500
    if jets[0]==4: events_athena=100
    if jets[0]==5: events_athena=20
    Z_mode = int((run_num-run_min)/10)%4+1
    special1 = """mllmin  10           ! Minimum M_ll
mllmax  60          ! Maximum M_ll
"""

# Z+jets samples - thanks to O. Rosenthal; modified by B. Di Micco
if run_num>=147073 and run_num<=147136:
    ZJET=True
    run_min=147073
    event_numbers=[80000,2000000,4500000,70000000,50000000,14000000,10000000,10000000,10000000]
    jets = [(run_num-run_min)%8,1] # Exclusive matching to the last number of partons

    events_athena = 5000
    if jets[0]==4: events_athena=2000
    if jets[0]==5: events_athena=20
    if (run_num-run_min)>=32: UseHerwig=False # Move to Pythia for half the samples
    Z_mode = int((run_num-147073)/8)%4+1

    special1 = """mllmin  60           ! Minimum M_ll
mllmax  2000          ! Maximum M_ll
"""

if ZJET:
    log.info('Recognized Z+jets run number.  Will generate for run '+str(run_num))
    process = 'zjet'
    if jets[0]==5: jets[1]=0

    nwarm = jets[0]+2
    events_alpgen = event_numbers[jets[0]]

    # Handle special cases for more than five partons.  Run # 6, Np5 excl.  Run # 7, Np6 incl.
    if jets[0]==6: 
        jets=[5,1]
    elif jets[0]==7:
        jets=[6,1]
    elif jets[0]>7:
        jets=[5,1]

    nwarm = [ jets[0]+2 , 1000000 ]
    if Z_mode<4: special1 += """ilep 0        ! Use leptons in the final state (not neutrinos)
izdecmode %i          ! Z decay mode (1: ee, 2: mumu, 3: tautau)\n """%(Z_mode)
    else: special1 += """ilep 1       ! Use neutrinos in the final state (not leptons) \n """
    if not UseHerwig: special1 += """xclu   0.26          ! ???
ipclu 1         ! ???
"""
    special2 = special1

# Wc+jets samples - thanks to O. Rosenthal
if run_num>=200000 and run_num<=200048:
    log.info('Recognized Wc+jets run number.  Will generate for run '+str(run_num))
    process = 'wcjet'
    jets = [(run_num-200000)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-200000)>=25: UseHerwig=False # Move to Pythia for half the samples

    #               10 min  30 min  55 min  9 hrs    10 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ ncwarm[jets[0]] , event_numbers[jets[0]] ]
    events_alpgen = event_numbers[jets[0]]

    # 0:5000  1:5000  2:5000  3:5000  4:5000 

    events_athena = 5000
    if jets[0]==4: events_athena=750
    if jets[0]==5: events_athena=200 # Should work
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100

    # Ok, just kidding, we aren't doing 7 partons, we're doing 5 inclusive
    if jets[0]==7: jets[0]=5

    W_mode = int((run_num-200000)/8)%3+1
    special1 = 'iwdecmode %i          ! W decay mode (1: el, 2: mu, 3: tau)\n '%(W_mode)
    special2 = special1

# Wcc+jets samples - thanks to O. Rosenthal
if run_num>=200100 and run_num<=200148:
    log.info('Recognized Wcc+jets run number.  Will generate for run '+str(run_num))
    process = 'wqq'
    jets = [(run_num-200100)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-200100)>=25: UseHerwig=False # Move to Pythia for half the samples

    #               10 min  30 min  55 min  9 hrs    10 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ ncwarm[jets[0]] , event_numbers[jets[0]] ]
    events_alpgen = event_numbers[jets[0]]

    # 0:5000  1:5000  2:5000  3:5000  4:5000 

    events_athena = 5000
    if jets[0]==4: events_athena=750
    if jets[0]==5: events_athena=200 # Should work
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100

    # Ok, just kidding, we aren't doing 7 partons, we're doing 5 inclusive
    if jets[0]==7: jets[0]=5

    W_mode = int((run_num-200100)/8)%3+1
    special1 = """iwdecmode %i          ! W decay mode (1: el, 2: mu, 3: tau)
ihvy 4        ! Select W+cc
 """%(W_mode)
    special2 = special1

# Wbb+jets samples - thanks to O. Rosenthal
if run_num>=200200 and run_num<=200248:
    log.info('Recognized Wbb+jets run number.  Will generate for run '+str(run_num))
    process = 'wqq'
    jets = [(run_num-200200)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-200200)>=25: UseHerwig=False # Move to Pythia for half the samples

    #               10 min  30 min  55 min  9 hrs    10 hrs
    #               20 hrs     ???      ???
    event_numbers=[1000000,5000000,5000000,30000000,12000000,
                   10000000,10000000,10000000]
    ncwarm = [2,2,3,3,4,4,4,4]
    nwarm = [ ncwarm[jets[0]] , event_numbers[jets[0]] ]
    events_alpgen = event_numbers[jets[0]]

    # 0:5000  1:5000  2:5000  3:5000  4:5000 

    events_athena = 5000
    if jets[0]==4: events_athena=750
    if jets[0]==5: events_athena=200 # Should work
    if jets[0]==6: events_athena=50
    if jets[0]==7: events_athena=100

    # Ok, just kidding, we aren't doing 7 partons, we're doing 5 inclusive
    if jets[0]==7: jets[0]=5

    W_mode = int((run_num-200200)/8)%3+1
    special1 = """iwdecmode %i          ! W decay mode (1: el, 2: mu, 3: tau)
ihvy 5        ! Select W+bb
 """%(W_mode)
    special2 = special1

# Zbb+jets samples - thanks to O. Rosenthal
if run_num>=200300 and run_num<=200331:
    log.info('Recognized Zbb+jets run number.  Will generate for run '+str(run_num))
    process = 'zqq'
    jets = [(run_num-200300)%8,1] # Exclusive matching to the last number of partons
    if jets[0]>5: jets[1]=0

    if (run_num-200300)>=16: UseHerwig=False # Move to Pythia for half the samples
    
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

    Z_mode = int((run_num-200300)/8)%2+1
    special1 = """mllmin  60           ! Minimum M_ll
mllmax  2000          ! Maximum M_ll 
ihvy    5             ! Select Z+bb
"""
    if Z_mode<3: special1 += """ilep 0        ! Use leptons in the final state (not neutrinos) \n """
    else: special1 += """ilep 1       ! Use neutrinos in the final state (not leptons) \n """
    special2 = special1

# Run numbers currently reserved: 168000 - 168100
# Njet samples obo M. Stoebe
UseJetFilter = False
if run_num>=168000 and run_num<168100:
    log.info('Recognized Njet run number.  Will generate for run '+str(run_num))
    process = 'Njet'
    UseJetFilter = True
    UseHerwig = True
    slice = (run_num-168050)%10
    num_parton = int((run_num-168050)/10)+2
    pT_range = ['0-20','20-80','80-200', '200-500', '500-1000', '1000-1500', '1500-2000+', '2000+']
    if (run_num-168000)<50: 
        UseHerwig=False # Move to Pythia for half the samples
        slice = (run_num-168000)%10
        num_parton = int((run_num-168000)/10)+2
    log.info('Will generate Njet process 2 -> '+str(num_parton)+'. For pT range JZ'+str(slice)+' = '+pT_range[slice])
    if num_parton > 6:
        log.info('Process for num_partons = '+str(num_parton)+' not defined. Will generate Njet process 2 -> 6+ instead. For pT range JZ'+str(slice)+' = '+pT_range[slice]+' GeV')
        num_parton = 6
    jets = [num_parton,1] # Exclusive matching to the last number of partons
    if jets[0] == 6: 
        jets[1]=0
    warm_numbers=[500000,1000000,5000000,5000000,3000000]
    ncwarm=[3,3,4,4,4]
    nwarm = [ ncwarm[jets[0]-2] , warm_numbers[(jets[0]-2)] ]
    event_numbers=[10000000,150000000,150000000,30000000,20000000]
    events_alpgen = event_numbers[(jets[0]-2)]
    
    ev_athena=[5000,5000,2000,100,50]
    events_athena=ev_athena[(jets[0]-2)] 

    et_range_max = [100,120,230,550,1050,1600,2100,10000]
    et_range_min = [0,10,60,180,450,950,1450,1950]

    special1 = """ptj1max %d
    ptj1min %d
    """%(et_range_max[slice], et_range_min[slice])
    special2 = special1

# Run numbers currently reserved: 168100 - 168200
# bb-bar samples obo M. Stoebe
if run_num>=168100 and run_num<168200:
    log.info('Recognized bb-bar run number.  Will generate for run '+str(run_num))
    process = '2Q'
    UseJetFilter = True
    UseHerwig = True
    slice = (run_num-168150)%10
    num_parton = int((run_num-168150)/10)
    pT_range = ['0-20','20-80','80-200', '200-500', '500-1000', '1000-1500', '1500-2000+', '2000+']
    if (run_num-168100)<50: 
        UseHerwig=False # Move to Pythia for half the samples
        slice = (run_num-168100)%10
        num_parton = int((run_num-168100)/10)
    log.info('Will generate 2Q process 2 -> bb-bar + '+str(num_parton)+'. For pT range JZ'+str(slice)+' = '+pT_range[slice])
    if num_parton > 4:
        log.info('Process for num_partons = bb-bar + '+str(num_parton)+' not defined. Will generate Njet process 2 -> bb-bar + 4+ jets instead. For pT range JZ'+str(slice)+' = '+pT_range[slice]+' GeV')
        num_parton = 4
    jets = [num_parton,1] # Exclusive matching to the last number of partons
    if jets[0] == 4: 
        jets[1]=0
    warm_numbers=[500000,1000000,3000000,5000000,5000000]
    ncwarm=[3,3,4,4,4]
    nwarm = [ ncwarm[jets[0]] , warm_numbers[(jets[0])] ]
    event_numbers=[5000000,30000000,30000000,40000000,40000000]
    events_alpgen = event_numbers[(jets[0])]
 
    ev_athena=[5000,1000,500,100,50]
    events_athena=ev_athena[(jets[0])] 
 
    et_range_max = [100,120,230,550,1050,1600,2100,10000]
    et_range_min = [0,10,60,180,450,950,1450,1950]
 
    special1 = """ptj1max %d
    ptj1min %d
    ihvy 5
    """%(et_range_max[slice], et_range_min[slice])
    special2 = special1

# Run numbers currently reserved: 168200 - 168999
# cc-bar samples obo M. Stoebe
if run_num>=168201 and run_num<168300:
    log.info('Recognized cc-bar run number.  Will generate for run '+str(run_num))
    process = '2Q'
    UseJetFilter = True
    UseHerwig = True
    slice = (run_num-168250)%10
    num_parton = int((run_num-168250)/10)
    pT_range = ['0-20','20-80','80-200', '200-500', '500-1000', '1000-1500', '1500-2000+', '2000+']
    if (run_num-168200)<50: 
        UseHerwig=False # Move to Pythia for half the samples
        slice = (run_num-168200)%10
        num_parton = int((run_num-168200)/10)
    log.info('Will generate 2Q process 2 -> cc-bar + '+str(num_parton)+' jet. For pT range JZ'+str(slice)+' = '+pT_range[slice])
    if num_parton > 4:
        log.info('Process for num_partons = cc-bar + '+str(num_parton)+' not defined. Will generate Njet process 2 -> cc-bar + 4+ jets instead. For pT range JZ'+str(slice)+' = '+pT_range[slice]+' GeV')
        num_parton = 4
    jets = [num_parton,1] # Exclusive matching to the last number of partons
    if jets[0] == 4: 
        jets[1]=0
    warm_numbers=[500000,1000000,5000000,5000000,5000000]
    ncwarm=[3,3,4,4,4]
    nwarm = [ ncwarm[jets[0]] , warm_numbers[(jets[0])] ]
    event_numbers=[5000000,30000000,50000000,40000000,40000000]
    events_alpgen = event_numbers[(jets[0])]
 
    ev_athena=[5000,1000,500,100,50]
    events_athena=ev_athena[(jets[0])] 
 
    et_range_max = [100,120,230,550,1050,1600,2100,10000]
    et_range_min = [0,10,60,180,450,950,1450,1950]
 
    special1 = """ptj1max %d
    ptj1min %d
    ihvy 4
    """%(et_range_max[slice], et_range_min[slice])
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

if hasattr(runArgs,'cutAthenaEvents'):
    events_athena = events_athena/runArgs.cutAthenaEvents
    log.warning('Reducing number of Athena events by '+str(runArgs.cutAthenaEvents)+' to '+str(events_athena)+' .  Later you should fix this.')
if hasattr(runArgs,'raiseAlpgenEvents'):
    events_alpgen = events_alpgen*runArgs.raiseAlpgenEvents
    log.warning('Increasing number of AlpGen events by '+str(runArgs.raiseAlpgenEvents)+' to '+str(events_alpgen)+' .  Later you should fix this.')

from AlpGenControl.AlpGenUtils import SimpleRun

log.info('Running AlpGen on process '+process+' with ECM='+str(ecm)+' with '+str(nwarm)+' warm up rounds')
log.info('Will generate '+str(events_alpgen)+' events with jet params '+str(jets)+' and random seed '+str(rand_seed))
if special1 is not None:
    log.info('Special settings for mode 1:')
    log.info(special1)
if special2 is not None:
    log.info('Special settings for mode 2:')
    log.info(special2)
if special3 is not None:
    log.info('Special settings for pythia:')
    log.info(special3)

SimpleRun( proc_name=process , events=events_alpgen , nwarm=nwarm , energy=ecm , jets=jets , seed = rand_seed , special1=special1 , special2=special2 , special3=special3 )

if hasattr(runArgs,'inputGeneratorFile'):
    log.info('About to clobber old input generator file argument, '+str(runArgs.inputGeneratorFile))
runArgs.inputGeneratorFile='alpgen.XXXXXX.TXT.v1'

if UseHerwig:
    log.info('Passing events on to Herwig for generation of '+str(events_athena)+' events in athena')
    include('AlpGenControl/MC12.AlpGenHerwig.py') 
else:
    log.info('Passing events on to Pythia for generation of '+str(events_athena)+' events in athena')
    include('AlpGenControl/MC12.AlpGenPythia.py')

    # Additional bit for ME/PS matching
    phojf=open('./pythia_card.dat', 'w')
    phojinp = """
      !...Matching parameters...
      IEXCFILE=%i
      showerkt=T
      qcut=%i
      imss(21)=24
      imss(22)=24  
    """%(jets[1],15)
    # For the moment, hard-coded jet cut (in AlpGenUtils.py)

    phojf.write(phojinp)
    phojf.close()


if ForceLeptonicTau:
   include("MC12JobOptions/Tauola_LeptonicDecay_Fragment.py")
   # tauola multiplicative factor for AMI MetaData
   topAlg.Herwig.CrossSectionScaleFactor=0.1393

if UseDilFilter: include("AlpGenControl/MC.GenAlpGenDilFilter.py")

if UseJetFilter:
  filterString = 'MC12JobOptions/JetFilter_JZ'+str(slice)+'.py'
  include(filterString)

# Fill up the evgenConfig
evgenConfig.description = 'alpgen_autoconfig'
evgenConfig.contact = ['zach.marshall@cern.ch']
evgenConfig.process = 'auto_alpgen_'+process+'_'+`jets[0]`+'p'
evgenConfig.inputfilecheck = 'alpgen.XXXXXX.TXT.v1'
#evgenConfig.efficiency = 0.90000
evgenConfig.minevents = events_athena
