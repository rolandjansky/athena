include('MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py')

tokens = runArgs.jobConfig[0].replace(".py","").split('_')
gentype, decaytype = tokens[2], tokens[3]
mstop, mstau = int(tokens[-2]), int(tokens[-1])

masses['1000006'] = mstop # stop
masses['1000015'] = mstau # stau
masses['1000022'] = 0.5   # neutralino "massless"

# the following lines tell Madgraph to generate stop-pairs with 0,1,2 extra jets.
process = """
generate p p > t1 t1~ {decay} @1
add process p p > t1 t1~ j {decay} @2
add process p p > t1 t1~ j j {decay} @3
""".format(decay=", (t1  > b ta1+ vt), (t1~ > b~ ta1- vt~) $ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~")

njets = 2

evgenConfig.contact  = [ "kilian.rosbach@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel', 'stop']
evgenConfig.description = 'stop direct pair production, stop->stau+nu+b,stau->tau+neutralino in simplified model, m_stop = %s GeV, m_stau = %s GeV, m_N1 = %s GeV.' % (masses['1000006'], masses['1000015'], masses['1000022'])
evgenConfig.inputconfcheck = "MadGraph_NNPDF23LO_TT_stau_%03i_%03i" % (mstop, mstau)

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets > 0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
