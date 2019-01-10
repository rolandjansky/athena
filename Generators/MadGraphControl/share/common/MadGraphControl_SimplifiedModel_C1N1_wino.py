include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

process = '''
generate p p > x1+ n1 $ susystrong @1
add process p p > x1- n1 $ susystrong @1
add process p p > x1+ n1 j $ susystrong @2
add process p p > x1- n1 j $ susystrong @2
add process p p > x1+ n1 j j $ susystrong @3
add process p p > x1- n1 j j $ susystrong @3
'''

njets=2

#naming examples: MC15.374040.MGPy8EG_A14N23LO_GGM_C1N1_wino_400.py
joparts = runArgs.jobConfig[0].rstrip('.py').split('_')
gentype = joparts[3]
decaytype = joparts[4]
masses['1000022'] = float(joparts[5]) # wino mass
masses['1000024'] = float(joparts[5]) # wino mass

#--------------------------------------------------------------
# create param_card
#--------------------------------------------------------------

#defining input slha file
# e.g. susy.374040.GGM_C1N1_wino_400.slha
param_card_in = 'susy.%s.GGM_%s_%s_%s.slha' %(runArgs.runNumber,joparts[3],joparts[4],joparts[5])
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#--------------------------------------------------------------
# Find filter
#--------------------------------------------------------------
include ( 'GeneratorFilters/MultiParticleFilter.py' )
filtSeq.MultiParticleFilter.Np  = 1
filtSeq.MultiParticleFilter.ptMinParticle = 105000.
filtSeq.MultiParticleFilter.etaRangeParticle = 4.9
filtSeq.MultiParticleFilter.particlePDG = [11,-11,22]
filtSeq.MultiParticleFilter.particleStatus = [1]
filtSeq.Expression = "MultiParticleFilter"
evt_multiplier = 6

evgenLog.info('Registered generation of simplified model direct wino to gravitino grid')

evgenConfig.contact  = [ "francisco.anuar.arduh@cern.ch", "keisuke.yoshihara@cern.ch" ]
evgenConfig.keywords += ['SUSY','gluino','neutralino','simplifiedModel', 'gravitino']
evgenConfig.description = 'EW wino pair production decaying into gravitino: m_wi = %s GeV'%(float(joparts[5]))

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{n1,1000022}{x1+,1000024}{x1-,-1000024}", 
                                "1000022:spinType = 1",
                                "1000024:spinType = 1" ]
