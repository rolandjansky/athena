include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

masses['1000021'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])
decaymode = str(runArgs.jobConfig[0].split('_')[4])

if decaymode == 'QQQ':
    decays['1000021'] = '''DECAY   1000021  1.40250326E-03   # gluino decays
#          BR          NDA       ID1       ID2       ID3       ID4
     1.00000000E+00    3      2         1       3      # BR(go -> u d s )
#'''

if decaymode == 'QQB':
    decays['1000021'] = '''DECAY   1000021  1.40250326E-03   # gluino decays
#          BR          NDA       ID1       ID2       ID3       ID4
     1.00000000E+00    3      2         3       5      # BR(go -> u s b ) 
#'''



process = '''
generate p p > go go $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > go go j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > go go j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets = 2

extras['pdgs_for_merging_cut']='1, 2, 3, 4, 21, 1000001, 1000002, 1000003, 1000004, 1000021, 2000001, 2000002, 2000003, 2000004'
#bug fix for syst calc issue in AGENE-1542  
extras["use_syst"]='F'                                                                                             
extras["event_norm"]='sum' 
   
evgenLog.info('Registered generation of RPV gluino pair production with UDD coupling, gluino decays to '+decaymode+'; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000021']))


evgenConfig.contact  = [ "simone.amoroso@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'gluino','RPV']
evgenConfig.description = 'gluino direct pair production, RPV decay  go->jjj in simplified model'

include ('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,-1000021}"]
