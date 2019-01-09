include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

process = '''
generate p p > n1 n1  $ susystrong @1
add process p p > n1 n2  $ susystrong @1
add process p p > n1 n3  $ susystrong @1
add process p p > n1 x1+  $ susystrong @1
add process p p > n1 x1-  $ susystrong @1
add process p p > n2 n2  $ susystrong @1
add process p p > n2 n3  $ susystrong @1
add process p p > n2 x1+  $ susystrong @1
add process p p > n2 x1-  $ susystrong @1
add process p p > n3 n3  $ susystrong @1
add process p p > n3 x1+  $ susystrong @1
add process p p > n3 x1-  $ susystrong @1
add process p p > x1+ x1-  $ susystrong @1
add process p p > n1 n1 j $ susystrong @2
add process p p > n1 n2 j $ susystrong @2
add process p p > n1 n3 j $ susystrong @2
add process p p > n1 x1+ j $ susystrong @2
add process p p > n1 x1- j $ susystrong @2
add process p p > n2 n2 j $ susystrong @2
add process p p > n2 n3 j $ susystrong @2
add process p p > n2 x1+ j $ susystrong @2
add process p p > n2 x1- j $ susystrong @2
add process p p > n3 n3 j $ susystrong @2
add process p p > n3 x1+ j $ susystrong @2
add process p p > n3 x1- j $ susystrong @2
add process p p > x1+ x1- j $ susystrong @2
add process p p > n1 n1 j j $ susystrong @3
add process p p > n1 n2 j j $ susystrong @3
add process p p > n1 n3 j j $ susystrong @3
add process p p > n1 x1+ j j $ susystrong @3
add process p p > n1 x1- j j $ susystrong @3
add process p p > n2 n2 j j $ susystrong @3
add process p p > n2 n3 j j $ susystrong @3
add process p p > n2 x1+ j j $ susystrong @3
add process p p > n2 x1- j j $ susystrong @3
add process p p > n3 n3 j j $ susystrong @3
add process p p > n3 x1+ j j $ susystrong @3
add process p p > n3 x1- j j $ susystrong @3
add process p p > x1+ x1- j j $ susystrong @3
'''

njets=2

#naming examples:MC15.374401.MGPy8EG_A14N23LO_GGM_CN_bhmix_150.py
joparts = runArgs.jobConfig[0].rstrip('.py').split('_')
gentype = joparts[3]
decaytype = joparts[4]

#--------------------------------------------------------------
# create param_card
#--------------------------------------------------------------

#defining input slha file
# e.g. susy.374401.GGM_CN_bhmix_150.slha
param_card_in = 'susy.%s.GGM_%s_%s_%s.slha' %(runArgs.runNumber,joparts[3],joparts[4],joparts[5])
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#---------------------------------------------------------------------------
#Reading gluino mass from parameter file
include ( 'MadGraphControl/SUSYMetadata.py' )
c1Mass = mass_extract( param_card_out, ['1000024'] )
masses['1000024'] = float(c1Mass[0])

evgenLog.info('Registered generation of simplified model direct higgsino/bino to gravitino grid')
evgenConfig.contact = ["joaquin.hoya@cern.ch", "hernan.wahlberg@cern.ch", "keisuke.yoshihara@cern.ch"]
evgenConfig.keywords += ['SUSY','gluino','neutralino','simplifiedModel']
evgenConfig.description = 'chargino-neutralinos production, bino-higgsino mix, simplified model: m_C1 = % GeV, m_mu = %s GeV'%(masses['1000024'],float(joparts[5]))

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{n1,1000022}{n2,1000023}{n3,1000025}{x1+,1000024}{x1-,-1000024}",
                                "1000022:spinType = 1",
                                "1000023:spinType = 1",
                                "1000024:spinType = 1",
                                "1000025:spinType = 1"]
