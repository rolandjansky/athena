include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

def MassToFloat(s):
  if "p" in s:
    return float(s.replace("p", "."))
  return float(s)

dsid       = str(runArgs.jobConfig[0].split('.')[1])
gentype    = str(runArgs.jobConfig[0].split('_')[2])
cardAppend = str(runArgs.jobConfig[0].split('_')[4])+"_"+str(runArgs.jobConfig[0].split('_')[5].split('.')[0])
decaytype  = str(runArgs.jobConfig[0].split('_')[3])

process = '''
generate p p > ta1+ ta1- $ susystrong @1
add process p p > ta2+ ta2- $ susystrong @1
add process p p > ta1+ ta2- $ susystrong @1
add process p p > ta1- ta2+ $ susystrong @1
add process p p > ta1+ ta1- j $ susystrong @2
add process p p > ta2+ ta2- j $ susystrong @2
add process p p > ta1+ ta2- j $ susystrong @2
add process p p > ta1- ta2+ j $ susystrong @2
add process p p > ta1+ ta1- j j $ susystrong @3
add process p p > ta2+ ta2- j j $ susystrong @3
add process p p > ta1+ ta2- j j $ susystrong @3
add process p p > ta1- ta2+ j j $ susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of stau pair production, decay via stau; grid point '+str(runArgs.runNumber) )

evgenConfig.keywords += ['gaugino', 'chargino', 'neutralino', 'stau']
evgenConfig.description = 'direct stau pair production in the pMSSM with direct decays, m_stauLR = %s GeV, M1 = %s GeV'%(MassToFloat(cardAppend.split('_')[0]),MassToFloat(cardAppend.split('_')[1]))

#defining input slha file
param_card_in = 'susy.%s.pMSSM_%s_%s_%s.slha'%(dsid,gentype,decaytype,cardAppend)
#defing output card file
decaytype  += "."+cardAppend
param_card_out = 'param_card.SM.%s.%s.dat'%(gentype,decaytype)

#Creating final parameter card
from MadGraphControl.MadGraphUtils import build_param_card
build_param_card( param_card_old = param_card_in, param_card_new = param_card_out )

#Reading stau-mass from parameter file
include ( 'MadGraphControl/SUSYMetadata.py' )
stauMass = mass_extract( param_card_out, ['1000015'] )
masses['1000015'] = MassToFloat(stauMass[0])

evt_multiplier = 3

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    genSeq.Pythia8.Commands += [ "Merging:Process = pp>{ta1+,1000015}{ta1-,-1000015}",
                                 "Merging:Process = pp>{ta2+,2000015}{ta2-,-2000015}",
                                 "Merging:Process = pp>{ta1+,1000015}{ta2-,-2000015}",
                                 "Merging:Process = pp>{ta2+,2000015}{ta1-,-1000015}"]
