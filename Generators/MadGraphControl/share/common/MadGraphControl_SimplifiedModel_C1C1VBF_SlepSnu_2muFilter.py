from MadGraphControl.MadGraphUtils import *

mode = 0

masses={}
masses['1000024'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000023'] = float(runArgs.jobConfig[0].split('_')[4])
masses['1000022'] = float(runArgs.jobConfig[0].split('_')[5].split('.')[0])
mslep = (masses['1000024'] + masses['1000022']) * 0.5
masses['1000011'] = mslep
masses['1000012'] = mslep
masses['1000013'] = mslep
masses['1000014'] = mslep
masses['1000015'] = mslep
masses['1000016'] = mslep

gentype = 'C1C1'
gentype_JO = str(runArgs.jobConfig[0].split('_')[2])
decaytype = str(runArgs.jobConfig[0].split('_')[3])

if gentype_JO != gentype+'VBF':
    raise RuntimeError("This control file is only for "+gentype+"VBF production")

mgproc = """generate p p > x1+ x1+ j j QED=99 QCD=99"""
mjjmin = 350
detajj = 3.0
ptjet1min = 50

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+gentype+'_'+decaytype+str(masses['1000024'])+'_'+str(masses['1000022'])

# process card
fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model mssm
define j = g u c d s u~ c~ d~ s~
define p = g u c d s u~ c~ d~ s~
define j = p
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
"""+mgproc+"""
output -f
""")
fcard.close()

beamEnergy = -999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")

msplit = abs(masses['1000024'] - masses['1000022'])
if   msplit > 32: evt_mult = 10
elif msplit > 17: evt_mult = 15
elif msplit > 12: evt_mult = 20
elif msplit >  8: evt_mult = 25
else:             evt_mult = 45

if runArgs.maxEvents > 0:
    nevents = runArgs.maxEvents*evt_mult
else:
    nevents = 5000*evt_mult

pdflabel = "nn23lo1"

extras = {'lhe_version' : '2.0',
          'pdlabel'     : "'"+pdflabel+"'",
          'ptj'         : 20,
          'etaj'        : 5.0,
          'mmjj'        : mjjmin,
          'deltaeta'    : detajj,
          'ptj1min'     : ptjet1min
          }

build_param_card(param_card_old='param_card.SM.%s.%s.dat'%(gentype,decaytype),param_card_new='param_card.dat',masses=masses)

build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,extras=extras)

print_cards()
process_dir = new_process()
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')
          
evgenLog.info('Generation of ~chi1+ ~chi1+ production via VBF, decay via sleptons; grid point '+str(runArgs.runNumber)+' decoded into mass point ' + str(masses['1000024']) + ' ' + str(masses['1000022']))

#############################################
# Showering 
#############################################
evgenConfig.contact += ["colleen.jennifer.treado@cern.ch", "drandyhaas@gmail.com"]
evgenConfig.keywords = ["SUSY", "MSSM","compressedSUSY","chargino","sameSign","VBF","2muon"]
evgenConfig.description = 'compressed C1C1 pair production via VBF in simplified model w/ m_C1=' + str(masses['1000024']) + ', m_N1=' + str(masses['1000022']) + ' GeV'
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


#############################################
# Filtering (two muons)
#############################################
if runArgs.jobConfig[0].split('_')[-1].split('.')[0] == '2mu':

    from GeneratorFilters.GeneratorFiltersConf import MultiMuonFilter
    filtSeq += MultiMuonFilter('DiMuFilter')
    myFilter = filtSeq.DiMuFilter
    myFilter.NMuons = 2
    myFilter.Ptcut = 3000
    myFilter.Etacut = 5.0
