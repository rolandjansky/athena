from MadGraphControl.MadGraphUtils import *
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

joparts = runArgs.jobConfig[0].rstrip('.py').split('_')
masses['1000021'] = float(joparts[4])
masses['1000022'] = float(joparts[5])
if masses['1000022']<0.5: masses['1000022']=0.5
gentype = joparts[2]
decaytype = '_'.join([joparts[3],joparts[6]]) 
short_decaytype = joparts[3]
neutralino_ctau = float(joparts[6].replace('p','.')) #in mm 
neutralino_width = 3.*1e11*6.581e-25/neutralino_ctau # in GeV
decays = {'1000022':"""DECAY   1000022  """   +str(neutralino_width)+  """ # neutralino decay
##            BR         NDA      ID1       ID2
        1.00000000E+00    2     1000039        23   #  BR(~chi_10 -> ~G Z)"""}
process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''
njets = 2
evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))

build_param_card(param_card_old='param_card.SM.GG.'+short_decaytype+'.dat', param_card_new='param_card.SM.GG.'+decaytype+'.dat',      masses=masses,decays=decays)

evgenConfig.contact  = [ "giovanna.cottin@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','gluino']
evgenConfig.description = 'gluino production in simplified model, GMSB scenario with displaced neutralino decay to ZG, m_glu = %s GeV, m_N1 = %s GeV, ctau(N1) = %s mm'%(masses['1000021'],masses['1000022'],neutralino_ctau)

randomSeed_original = 1234
if hasattr(runArgs, "randomSeed"):
    randomSeed_original = runArgs.randomSeed
# Giving a unique seed number (not exceeding the limit of ~30081^2)
runArgs.randomSeed = 1000 * int(str(runArgs.runNumber)[1:6]) + randomSeed_original
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
runArgs.randomSeed = randomSeed_original

if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]

# Turn off checks for displaced vertices. 
# Other checks are fine.
testSeq.TestHepMC.MaxVtxDisp = 1000*1000 #In mm
testSeq.TestHepMC.MaxTransVtxDisp = 1000*1000
