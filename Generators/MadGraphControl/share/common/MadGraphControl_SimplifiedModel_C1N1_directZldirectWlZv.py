include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPreInclude.py' )

#Define C1 and N1 masses and mass splitting
splitConfig = runArgs.jobConfig[0].rstrip('.py').split('_')
masses['1000024'] = float(splitConfig[4]) # C1 mass
masses['1000022'] = float(splitConfig[5]) # N1 mass

# N1: 1000022 ;  N2: 1000023 ; C1: 1000024 ; C2: 1000037

# Will be C1N1
gentype = str(splitConfig[2])
# Will be All RPV decays of both C1 and N1 
decaytype = str(splitConfig[3]) 

process = '''
generate p p > x1+ n1 / susystrong @1
add process p p > x1- n1 / susystrong @1
add process p p > x1+ n1 j / susystrong @2
add process p p > x1- n1 j / susystrong @2
add process p p > x1+ n1 j j / susystrong @3
add process p p > x1- n1 j j / susystrong @3
'''
njets = 2
evgenLog.info('Registered generation of ~chi1+/- ~chi10 production, decay via Zl,Wv,Hl and Zv,Wl,Hv respectively; grid point '+str(runArgs.runNumber))

evgenConfig.contact  = [ "lucas.macrorie.flores@cern.ch" ]
evgenConfig.keywords += ['simplifiedModel','SUSY','neutralino','RPV']
evgenConfig.description = '~chi1+/- ~chi10 production,  decay via Zl,Wv,Hl and Zv,Wl,Hv (all RPV) respectively in simplified model, m_N1 = %s GeV'%(masses['1000022'])

# To maximize 3 lepton C1 resonance
from GeneratorFilters.GeneratorFiltersConf import ZtoLeptonFilter
filtSeq += ZtoLeptonFilter("ZtoLeptonFilter")

# to fight against basline SUSY recomendation (more CPU time but saves space)
from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
filtSeq += MultiElecMuTauFilter("MultiElecMuTauFilter")
ElecMuTauFilter = filtSeq.MultiElecMuTauFilter
ElecMuTauFilter.MinPt = 9000. # minimum offline lepton pT threshold, minus a bit
ElecMuTauFilter.MaxEta = 2.8
ElecMuTauFilter.NLeptons = 3
ElecMuTauFilter.IncludeHadTaus = 0    # don't include hadronic taus

filtSeq.Expression = "ZtoLeptonFilter and MultiElecMuTauFilter"

evt_multiplier = 20.
if masses['1000024']<1000:
    evt_multiplier = 50.

include ( 'MadGraphControl/MadGraphControl_SimplifiedModelPostInclude.py' )

if njets>0:
    evgenLog.info('Performing the Pythia guess CKKW-L merging feature')
    genSeq.Pythia8.Commands += ["Merging:Process = guess"]
    if "UserHooks" in genSeq.Pythia8.__slots__.keys():
        genSeq.Pythia8.UserHooks += ['JetMergingaMCatNLO']
    else:
        genSeq.Pythia8.UserHook = 'JetMergingaMCatNLO'
