# MC15.xxxxxx.MadGraphPythia8EvtGen_A14NNPDF23LO_GG_rpvLF_2000_1250.py  
# MC15.xxxxxx.MadGraphPythia8EvtGen_A14NNPDF23LO_GG_rpvHF_2000_1250.py  
# MC15.xxxxxx.MadGraphPythia8EvtGen_A14NNPDF23LO_TT_rpvHF_2000_1250.py  

import os.path
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )
tokens = runArgs.jobConfig[0].replace(".py","").split('_')
gentype = str(tokens[2])
decaytype = str(tokens[3])
mass_stopgluino = float(tokens[4])
masses['1000022'] = float(tokens[5])

njets=2
if gentype == "GG":
	masses['1000021'] = mass_stopgluino
	process = '''
	generate p p > go go @1
	add process p p > go go j @2
	add process p p > go go j j @3
	'''
	evgenLog.info('Registered generation of gluino grid '+str(runArgs.runNumber))
	evgenConfig.keywords += ['gluino','SUSY','simplifiedModel','RPV', 'neutralino']
	evgenConfig.description = 'gluino pair production with gluino->qqq (1/2 br) and gluino -> qq+neutralino, neutralino->qqq (1/2 br) via RPV UDD coupling. m_glu = %s GeV, m_N1 = %s GeV'%(masses['1000021'],masses['1000022'])
	genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]

elif gentype == "TT":
	masses['1000006'] = mass_stopgluino
	process = '''
	generate p p > t1 t1~ @1
	add process p p > t1 t1~ j @2
	add process p p > t1 t1~ j j @3
	'''
	evgenLog.info('Registered generation of stop grid '+str(runArgs.runNumber))
	evgenConfig.keywords += ['stop','SUSY','simplifiedModel','RPV', 'neutralino']
	evgenConfig.description = 'stop pair production with stop -> bs (1/2 br) and stop -> top+neutralino, neutralino->tbs (1/2 br) via RPV UDD coupling. m_stop = %s GeV, m_N1 = %s GeV'%(masses['1000006'],masses['1000022'])
	genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,1000006}"]

evgenConfig.contact  = ["jmontejo@cern.ch" ]
keepOutput=True
include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )
