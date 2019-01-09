# SUSY MadGraph+Pythia8 jobOptions for pMSSM direct stop pair generation

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

# e.g. MC15.390530.MGPy8EG_A14N23LO_TT_pMSSM_DM_mq3L_260_110_MET100.py
dsid       = str(runArgs.jobConfig[0].split('.')[1]) #390530
gentype    = str(runArgs.jobConfig[0].split('_')[2]) #TT
mq3type     = str(runArgs.jobConfig[0].split('_')[5]) #mq3L
mq3        = str(runArgs.jobConfig[0].split('_')[6]) #260
m1         = str(runArgs.jobConfig[0].split('_')[7].split('.')[0]) #110
decaytype  = 'pMSSM'

if gentype == 'TT' :
    process = '''
    generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
    add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
    add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
    '''
elif gentype == 'BB' :
    process = '''
    generate p p > b1 b1~ $ go susylq susylq~ t1 b2 t2 t1~ b2~ t2~ @1
    add process p p > b1 b1~ j $ go susylq susylq~ t1 b2 t2 t1~ b2~ t2~ @2
    add process p p > b1 b1~ j j $ go susylq susylq~ t1 b2 t2 t1~ b2~ t2~ @3
    '''
else:
    raise RunTimeError("ERROR: Do not understand gentype: TT or BB?")

njets=2

#defining input slha file
# e.g. susy.390530.TT_pMSSM_DM_mq3L_260_M1_110_MET100.slha
param_card_in = 'susy.%s.%s_pMSSM_DM_%s_%s_M1_%s.slha'%(dsid,gentype,mq3type,mq3,m1)
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#Reading stop mass from parameter file
include ( 'MadGraphControl/SUSYMetadata.py' )
if gentype == 'TT' :
    stopMass = mass_extract( param_card_out, ['1000006'] )
    masses['1000006'] = float(stopMass[0])
    mt1 = str(masses['1000006'])
elif gentype == 'BB' :
    sbottomMass = mass_extract( param_card_out, ['1000005'] )
    masses['1000005'] = float(sbottomMass[0])
    mb1 = str(masses['1000005'])
else:
    raise RunTimeError("ERROR: Do not understand gentype: TT or BB?")

jobConfigParts = runArgs.jobConfig[0].split("_")

if 'MET100' in "%s_%s" % (jobConfigParts[-2], jobConfigParts[-1]):

    log.info('MET100 filter is applied')
    include('GeneratorFilters/MissingEtFilter.py')
    filtSeq.MissingEtFilter.METCut = 100*GeV
    filtSeq.Expression = "MissingEtFilter"

    evt_multiplier = 10 

evgenConfig.contact  = [ "Keisuke.Yoshihara@cern.ch" ]
evgenConfig.keywords += ['SUSY','stop','sbottom','pMSSM']

if gentype == 'TT' :
    evgenConfig.description = 'stop direct pair production in pMSSM, mq3L(mtR) = %s GeV, M1 = %s GeV, mt1 = %s GeV'%(mq3,m1,mt1)
elif gentype == 'BB' :
    evgenConfig.description = 'sbottom direct pair production in pMSSM, mq3L(mtR) = %s GeV, M1 = %s GeV, mb1 = %s GeV'%(mq3,m1,mb1)
else:
    raise RunTimeError("ERROR: Do not understand gentype: TT or BB?")

genSeq.Pythia8.Commands += ["23:mMin = 0.2"]
genSeq.Pythia8.Commands += ["24:mMin = 0.2"]
genSeq.Pythia8.Commands += ["25:mMin = 0.2"]

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    if gentype == 'TT' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
    elif gentype == 'BB' :
        genSeq.Pythia8.Commands += ["Merging:Process = pp>{b1,1000005}{b1~,-1000005}"]
    else:
        raise RunTimeError("ERROR: Do not understand gentype: TT or BB?")

