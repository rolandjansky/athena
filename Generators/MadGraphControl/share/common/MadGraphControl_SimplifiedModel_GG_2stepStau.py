include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

mGluino = float(runArgs.jobConfig[0].split('_')[4])
mCN = float(runArgs.jobConfig[0].split('_')[5])
mStau = float(runArgs.jobConfig[0].split('_')[6])
mLSP = float(runArgs.jobConfig[0].split('_')[7].rstrip('.py'))

evgenLog.info('Generation of gluino pairs with 2-step decay via stau, DSID = '+str(runArgs.runNumber))
masses['1000021'] = mGluino
masses['1000023'] = mCN
masses['1000024'] = mCN
masses['1000015'] = mStau
masses['1000016'] = mStau
masses['1000022'] = mLSP

gentype = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

njets=2

process = '''
generate p p > go go $ susysq susysq~ @1
add process p p > go go j $ susysq susysq~ @2
add process p p > go go j j $ susysq susysq~ @3
'''

evgenConfig.contact  = [ "martindl@cern.ch" ]
evgenConfig.keywords += ['gluino','simplifiedModel']
evgenConfig.description = 'Simplified model of gluino pair production with 2-step decay via C1/N2 then stau/sneutrino, m_glu = %s GeV, m_C1N2 = %s GeV, m_stau = %s GeV, m_N1 = %s GeV'%(mGluino,mCN,mStau,mLSP)

# filter efficiency depends on mass splitting
if mGluino-mLSP <= 80 and '_J85' in runArgs.jobConfig[0]:
    evt_multiplier = 8.
elif mGluino-mLSP <= 240 and '_J85' in runArgs.jobConfig[0]:
    evt_multiplier = 5.

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

# MadGraph/Pythia matching
if njets>0:
    genSeq.Pythia8.Commands += ["Merging:Process = pp>{go,1000021}{go,1000021}"]

# truth-jet filter for compressed points
if '_J85' in runArgs.jobConfig[0]:
    evgenLog.info('Adding truth jet filter, Pt > 85 GeV')

    include("GeneratorFilters/AntiKt4TruthJets.py")

    from GeneratorFilters.GeneratorFiltersConf import TruthJetFilter
    filtSeq += TruthJetFilter("TruthJetFilter")
    filtSeq.TruthJetFilter.jet_pt1 = 85000.
    filtSeq.TruthJetFilter.NjetMinPt = 0.
    filtSeq.TruthJetFilter.NjetMaxEta = 2.8
    filtSeq.TruthJetFilter.Njet = 1
