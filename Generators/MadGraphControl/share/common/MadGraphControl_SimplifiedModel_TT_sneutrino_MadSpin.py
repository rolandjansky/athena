include('MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py')

tokens = runArgs.jobConfig[0].replace(".py","").split('_')
gentype, decaytype = tokens[2], tokens[3]
mstop, msneu = int(tokens[-3]), int(tokens[-2])

masses['1000006'] = mstop # stop
masses['1000016'] = msneu # tau sneutrino_L
masses['1000022'] = 0.5   # neutralino "massless"

# the following lines tell Madgraph to generate stop-pairs with 0,1,2 extra jets.
process = """
generate p p > t1 t1~ {decay} @1
add process p p > t1 t1~ j {decay} @2
add process p p > t1 t1~ j j {decay} @3
""".format(decay="$ go susylq susylq~ b1 b2 t2 b1~ b2~ t2~")

njets = 2

evgenConfig.contact  = [ "mann@cern.ch" ]
evgenConfig.keywords += ['SUSY', 'simplifiedModel', 'stop']
evgenConfig.description = 'direct stop-pair production (using MadSpin for 3-body decay), stop -> tau + sneutrino + b in simplified model, m_stop = %s GeV, m_sneutrino = %s GeV, m_N1 = %s GeV.' % (masses['1000006'], masses['1000016'], masses['1000022'])
evgenConfig.inputconfcheck = "MadGraph_NNPDF23LO_TT_sneutrino_%03i_%03i" % (mstop, msneu)

evgenLog.info('Running w/ MadSpin')
madspin_card = "madspin_card.dat"
with open(madspin_card, "wt") as f:
    f.write("""set seed {seed}
set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
set spinmode none
decay t1 > b ta+ svt
decay t1~ > b~ ta- svt~
launch
""".format(seed=runArgs.randomSeed))

include('MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py')

if njets > 0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{t1,1000006}{t1~,-1000006}"]
