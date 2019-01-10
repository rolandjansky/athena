# SUSY MadGraph+Pythia8 jobOptions for simplified model gluino pair generation

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#naming examples: MC15.374327.MGPy8EG_A14N23LO_GGM_GG_bhmix_1900_650.py
joparts = runArgs.jobConfig[0].rstrip('.py').split('_')
gentype = joparts[3]
decaytype = joparts[4]

process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''

njets=2

#--------------------------------------------------------------
# create param_card
#--------------------------------------------------------------

#defining input slha file
# e.g. susy.374327.GGM_GG_bhmix_1900_650.slha
param_card_in = 'susy.%s.GGM_%s_%s_%s_%s.slha' %(runArgs.runNumber,joparts[3],joparts[4],joparts[5],joparts[6])
log.info("Using paramCard %s" % param_card_in)

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card, print_cards
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#---------------------------------------------------------------------------
#Reading gluino mass from parameter file
include ( 'MadGraphControl/SUSYMetadata.py' )
glMass = mass_extract( param_card_out, ['1000021'] )
masses['1000021'] = float(glMass[0])

evgenConfig.contact  = [ "joaquin.hoya@cern.ch", "hernan.wahlberg@cern.ch", "keisuke.yoshihara@cern.ch" ]
evgenConfig.keywords += ['SUSY','gluino','neutralino','simplifiedModel']
evgenConfig.description = 'gluino pair production, bino-higgsinos admixture grid: m_gl = %s GeV, M3 = %s GeV, m_mu = % GeV'%(masses['1000021'],float(joparts[5]),float(joparts[6]))

print_cards(param_card='param_card.SM.GG.bhmix.dat')

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += ["SLHA:file = param_card.SM.GG.bhmix.dat",
#                                "SLHA:useDecayTable = on", # allows decay parameters to be read in from <slha> block
                                "SLHA:readFrom = 2",
#                                "Beams:LHEF = tmp_madgraph_OTF._00001.events.events",
                                "Merging:Process = pp>{go,1000021}{go,1000021}"]
