include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000006'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
decaymode = str(runArgs.jobConfig[0].split('_')[4])

if decaymode == 'QQ':
    decays['1000006'] = '''DECAY   1000006  1.40250326E-03   # stop decays
#          BR          NDA       ID1       ID2       ID3       ID4
     1.00000000E+00    2      -2        -2 # BR(~t_1 -> dbar dbar)
#'''

if decaymode == 'QB':
    decays['1000006'] = '''DECAY   1000006  1.40250326E-03   # stop decays
#          BR          NDA       ID1       ID2       ID3       ID4
     1.00000000E+00    2      -5        -2 # BR(~t_1 -> bbar dbar)
#'''

if decaymode == 'BB':
    decays['1000006'] = '''DECAY   1000006  1.40250326E-03   # stop decays
#          BR          NDA       ID1       ID2       ID3       ID4
     1.00000000E+00    2      -5        -5 # BR(~t_1 -> bbar bbar)
#'''



process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets = 2

extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'
#bug fix for syst calc issue in AGENE-1542  
extras["use_syst"]='F'                                                                                             
extras["event_norm"]='sum' 
   
evgenLog.info('Registered generation of RPV stop pair production with UDD coupling, stop decays to '+decaymode+'; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000006']))


evgenConfig.contact  = [ "simone.amoroso@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop','RPV']
evgenConfig.description = 'stop direct pair production, RPV decay  st->b+s in simplified model'

include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
