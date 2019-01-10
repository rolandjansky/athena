include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

process = '''
define gosusylq = go susylq susylq~
generate p p > gosusylq gosusylq $ susystrong susyweak @1
add process p p > gosusylq gosusylq j $ susystrong susyweak @2
'''

njets=1

#naming examples: MC15.373690.MGPy8EG_A14N23LO_SG_pMSSM_1500_3000_0.py
joparts = runArgs.jobConfig[0].rstrip('.py').split('_')

#--------------------------------------------------------------
# Find Squark or Gluino Mass
#--------------------------------------------------------------
sqMass=float(joparts[4])
glMass=float(joparts[5])
LSPMass=float(joparts[6])
for q in squarks: masses[q] = sqMass
masses['1000021'] = glMass
log.info('squark mass = '+str(sqMass)+', gluino mass = '+str(glMass))

if sqMass<glMass: gentype = 'SS'
else: gentype = 'GG'

decaytype = 'pMSSM'

#defining input slha file
# e.g. susy.389370.pMSSM_SG_1500_3000_0.slha
#param_card_in = 'susy.%s.pMSSM_SG_%s_%s_%s.slha' %(runArgs.runNumber,joparts[4],joparts[5],joparts[6])
#log.info("Using paramCard %s" % param_card_in)


########################################################
# Produce input slha file on the fly
# 1st generate mass spectrum from template
include ( 'MadGraphControl/makeMassSpectrum.py' )
massCardName= generateMassSpectrum(sqMass,glMass,LSPMass)
log.info("Mass card %s generated" % massCardName)
# 2nd use the mass spectrum card to generate slha file with decay tables
include('MadGraphControl/makeSLHAFromSpectrum.py')
slhaFileName= 'susy.%s.pMSSM_SG_%d_%d_%d.slha' %(runArgs.runNumber,sqMass,glMass,LSPMass)
makeSLHAFromSpectrum(massCardName,slhaFileName)

param_card_in =slhaFileName

#########################################################

#defing output card file
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#--------------------------------------------------------------
# Find filter
#--------------------------------------------------------------
metmin = float(0)
import re
match = re.search(r'_MET(\d+)[_.]', runArgs.jobConfig[0])
if match and match.group(1):
  metmin = float(match.group(1))
  log.info('METFilter'+str(metmin)+' is applied')
  evt_multiplier = 20.0  

  include("GeneratorFilters/MissingEtFilter.py")
  filtSeq.MissingEtFilter.METCut = metmin*GeV


evgenLog.info('Registered generation of pMSSM squark-gluino')

evgenConfig.contact  = [ "takashi.yamanaka@cern.ch" ]
evgenConfig.keywords += ['SUSY','pMSSM','gluino','squark']
evgenConfig.description = 'pMSSM squark-gluino grid m_sq = %s GeV, m_gl = %s GeV'%(sqMass,glMass)

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    #genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}{ul,1000002}{ul~,-1000002}{ur,2000002}{ur~,-2000002}{dl,1000001}{dl~,-1000001}{dr,2000001}{dr~,-2000001}{sl,1000003}{sl~,-1000003}{sr,2000003}{sr~,-2000003}{cl,1000004}{cl~,-1000004}{cr,2000004}{cr~,-2000004}"]
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'

