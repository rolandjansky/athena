include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

therun = -1
tokens = runArgs.jobConfig[0].replace(".py","").split('_')
gentype, decaytype, m_n1 = tokens[1], tokens[4], float(tokens[2])

#####################################################################
#                  MadGraph+Pythia8                                 # 
#####################################################################
m_grav = 1E-9
masses['1000022'] = m_n1   # m_N1
masses['1000023'] = -m_n1-4.5   # m_N2
masses['1000024'] = m_n1+3.5   # m_Chi1+
masses['1000039'] = m_grav
stringy = str(m_n1)+'_'+str(m_grav)
gentype = 'N1N1'
decaytype = 'directHG'
njets = 2 
use_Photos = False
use_Tauola = False
use_decays = False
doKtMerge = True
evgenLog.info('Registered generation of neutralino pair production, neutralino to h+~G; grid point '+str(therun)+' decoded into mass point ' + str(m_n1) + ', with ' + str(njets) + ' jets.')

evt_multiplier=2
evgenConfig.contact  = [ "takashi.yamanaka@cern.ch", "Fernando.Monticelli@cern.ch" ]
evgenConfig.keywords += ['neutralino']
evgenConfig.description = 'neutralino inclusive pair production, ~chi10->h+~G in simplified model, Mass N1='+ str(m_n1)



jetloop=['']
if 0<njets: jetloop += [ 'j' ]
if 1<njets: jetloop += [ 'j j' ]

# Add the procesess

counter=0
starter = 'generate'
process = ''
production = ['n1 ', 'n2 ', 'x1+ ', 'x1- ' ]
for part1_idx in range(len(production)):
    for part2_idx in range(len(production)):
        if part2_idx<part1_idx: continue
        part1=production[part1_idx]
        part2=production[part2_idx]
        if part1==part2: continue
        s_prod=part1+part2
        for jets in jetloop:
            process += '%s p p > %s%s $ go susysq susysq~ @%s \n'%(starter,s_prod,jets,str(counter))
            counter = counter+1
            starter = 'add process'

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

genSeq.Pythia8.Commands += [ "Merging:Process = pp>{x1+,1000024}{x1-,-1000024}{n2,1000023}{n1,1000022}"]

evgenConfig.generators += ["MadGraph", "Pythia8"]

