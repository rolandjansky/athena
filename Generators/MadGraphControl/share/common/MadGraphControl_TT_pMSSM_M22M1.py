# SUSY MadGraph+Pythia8 jobOptions for pMSSM direct stop pair generation

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

# e.g. MC15.389370.MGPy8EG_A14N23LO_TT_pMSSM_M22M1_pMU_300_50_MET100.py
dsid       = str(runArgs.jobConfig[0].split('.')[1]) #389370
gentype    = str(runArgs.jobConfig[0].split('_')[2]) #TT
signmu     = str(runArgs.jobConfig[0].split('_')[5]) #pMU
mq3l       = str(runArgs.jobConfig[0].split('_')[6]) #600
m1         = str(runArgs.jobConfig[0].split('_')[7].split('.')[0]) #50
decaytype  = 'pMSSM'

process = '''
generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
'''

njets=2

#defining input slha file
# e.g. susy.389370.TT_pMSSM_M22M1_pMU_mq3L_300_M1_50_MET100.slha
param_card_in = 'susy.%s.TT_pMSSM_M22M1_%s_mq3L_%s_M1_%s.slha'%(dsid,signmu,mq3l,m1)
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#Reading stop mass from parameter file
include ( 'MadGraphControl/SUSYMetadata.py' )
stopMass = mass_extract( param_card_out, ['1000006'] )
masses['1000006'] = float(stopMass[0])
mt1 = str(masses['1000006'])

jobConfigParts = runArgs.jobConfig[0].split("_")

if 'MET100' in "%s_%s" % (jobConfigParts[-2], jobConfigParts[-1]):

    log.info('MET100 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100*GeV
    filtSeq.Expression = "MissingEtFilter"

    evt_multiplier = 10 

evgenConfig.contact  = [ "Keisuke.Yoshihara@cern.ch" ]
evgenConfig.keywords += ['SUSY','stop', 'pMSSM']
evgenConfig.description = 'stop direct pair production in pMSSM, mq3L = %s GeV, M1 = %s GeV, mt1 = %s GeV'%(mq3l,m1,mt1)

genSeq.Pythia8.Commands += ["23:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]

