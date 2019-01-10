include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#Read filename of jobOptions to obtain: gluino mass, neutralino mass, decay flavour & lifetime
tokens = runArgs.jobConfig[0].split('_')
gentype = str(tokens[2])
decaytype = str(tokens[3])
mass_stopgluino = float(tokens[4])
masses['1000022'] = float(tokens[5])
decayflavor = str(tokens[6])
lifetimeString = str(tokens[7])
neutralinoLifetime = lifetimeString.replace("ns","").replace(".py","").replace("p","0.")
hbar = 6.582119514e-16
decayWidth = hbar/float(neutralinoLifetime)

#Decay flavour
decayWidthStr = '%e' % decayWidth
decayStringHeader = 'DECAY   1000022  '
header = decayStringHeader + decayWidthStr 
evgenLog.info('lifetime of 1000022 is set to %s ns'% neutralinoLifetime)
if decayflavor == "rpvHF":
	branchingRatios = '''
	#          BR          NDA       ID1       ID2       ID3
	0.5000	3    6	3  5
	0.5000	3   -6 -3 -5
	#'''
elif decayflavor == "rpvLF":
	branchingRatios = '''
  	#          BR          NDA       ID1       ID2       ID3
  	0.2500	3    2	1  3
  	0.2500	3    4	1  3
  	0.2500	3   -2 -1 -3
  	0.2500	3   -4 -1 -3
  	#'''
elif decayflavor == "rpvdem":
        branchingRatios = '''
        #          BR          NDA       ID1       ID2       ID3      	
        0.05555556	3    2	1  3
        0.05555556	3    2	1  5
        0.05555556	3    2	3  5
        0.05555556	3    4	1  3
        0.05555556	3    4	1  5
        0.05555556	3    4	3  5
        0.05555556	3    6	1  3
        0.05555556	3    6	1  5
        0.05555556	3    6	3  5
        0.05555556	3   -2 -1 -3
        0.05555556	3   -2 -1 -5
        0.05555556	3   -2 -3 -5
        0.05555556	3   -4 -1 -3
        0.05555556	3   -4 -1 -5
        0.05555556	3   -4 -3 -5
        0.05555556	3   -6 -1 -3
        0.05555556	3   -6 -1 -5
        0.05555556	3   -6 -3 -5
        #'''

decays['1000022'] = header + branchingRatios
print decays['1000022']

#Neutralino generation, gluinos or stop quarks
if gentype == "GG":
	masses['1000021'] = mass_stopgluino
	process = '''
	generate p p > go go $ susysq susysq~ @1
	add process p p > go go j $ susysq susysq~ @2
	add process p p > go go j j $ susysq susysq~ @3
	'''

elif gentype == "TT":
	masses['1000006'] = mass_stopgluino
	process = '''
	generate p p > t1 t1~ $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @1
	add process p p > t1 t1~ j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @2
	add process p p > t1 t1~ j j $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~ @3
	'''
njets=2

evgenConfig.contact  = [ "rebecca.carney@cern.ch", "jmontejo@cern.ch" ]

if gentype == "GG":
	evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))
	evgenConfig.keywords += ['gluino','SUSY','simplifiedModel','RPV', 'neutralino','longLived']
	evgenConfig.description = 'gluino pair production with gluino -> qq+neutralino, neutralino->qqq via RPV UDD coupling. m_glu = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000022'])
elif gentype == "TT":
	evgenLog.info('Registered generation of stop grid '+str(runArgs.runNumber))
	evgenConfig.keywords += ['stop','SUSY','simplifiedModel','RPV', 'neutralino','longLived']
	evgenConfig.description = 'stop pair production with stop -> top+neutralino, neutralino->tbs via RPV UDD coupling. m_stop = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000022'])

#keepOutput=True
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
	if gentype == "GG":
		genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]
	elif gentype == "TT":
		genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,1000006}"]

testSeq.TestHepMC.MaxVtxDisp = 1e8 #in mm
testSeq.TestHepMC.MaxTransVtxDisp = 1e8

