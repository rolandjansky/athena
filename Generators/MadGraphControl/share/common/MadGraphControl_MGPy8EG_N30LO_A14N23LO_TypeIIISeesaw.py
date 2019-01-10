import subprocess
from fileinput import FileInput
from MadGraphControl import MadGraphUtils

evgenConfig.description = 'MadGraph5+Pythia8 Type III Seesaw Model'
evgenConfig.contact = ['tadej.novak@cern.ch']
evgenConfig.keywords = ['BSM', 'exotic', 'seeSaw', '2lepton', '2jet']

proc_name = 'PROC_TypeIIISeesaw' + str(mL)

proc_card = """
import model typeIIIseesaw
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
define vl = v1 v2 v3
generate p p > tr0 tr+, tr0 > l+ W-, tr+ > vl W+
add process p p > tr0 tr+, tr0 > l- W+, tr+ > vl W+
add process p p > tr0 tr-, tr0 > l- W+, tr- > vl W-
add process p p > tr0 tr-, tr0 > l+ W-, tr- > vl W-
"""
proc_card += "\noutput %s -f" % proc_name

# modifications to the param_card.dat (generated from the proc_card i.e. the specific model)
# if you want to see the resulting param_card, run Generate_tf with this jobo, and look at the param_card.dat in the cwd
# If you want to see the auto-calculated values of the decay widths, look
# at the one in <proc_name>/Cards/param_card.dat (again, after running a
# Generate_tf)
param_card_extras = {
    # auto-calculate decay widths and BR
    'DECAY': {'Wtr0': 'Auto', 'Wtrch': 'Auto'},
    'MASS': {'mtr0': mL, 'mtrch': mL},  # set masses of heavy leptons
    'NEWMASSES': {'mtr': mL, 'mtrm': mL},
    'MIXING': {'Ve': 5.5e-2, 'Vm': 5.5e-2, 'Vtt': 5.5e-2},
}

run_card_extras = {
    'pdlabel': 'lhapdf',
    'lhaid': '263000'  # NNPDF30_lo_as_0130 pdf set
}

save_proc_dir = False

#################################

safefactor = 7 if enableFilters and not lightLeptons else 3  # generate the requested events due to filter efficiencies
nevents = 5000 * safefactor
if runArgs.maxEvents > 0:
    nevents = runArgs.maxEvents * safefactor

beamEnergy = -999
if hasattr(runArgs, 'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError('No center of mass energy found')

# make the proc card
fcard = open('proc_card_mg5.dat', 'w')
fcard.write(proc_card)
fcard.close()

# does mg5 to determine all the diagrams etc ... generates the process
# directory corresponding to proc_name
process_dir = MadGraphUtils.new_process()

if proc_name != '' and process_dir != proc_name:
    raise RuntimeError(
        'Unexpected process_dir %s when expected %s' % (process_dir, proc_name))

# create the param_card
if os.access('param_card.dat', os.R_OK):
    print('Deleting old param_card.dat')
    os.remove('param_card.dat')
param_card = '%s/Cards/param_card.dat' % process_dir

if(MadGraphUtils.build_param_card(param_card_old=param_card, param_card_new='param_card.dat', params=param_card_extras) == -1):
    raise RuntimeError('Could not create param_card.dat')

# create the run card: FIXME: Should check for success
if os.access('run_card.dat', os.R_OK):
    print('Deleting old run_card.dat')
    os.remove('run_card.dat')
run_card = MadGraphUtils.get_default_runcard(proc_dir=process_dir)
if MadGraphUtils.build_run_card(run_card_old=run_card, run_card_new='run_card.dat', nevts=nevents, rand_seed=runArgs.randomSeed, beamEnergy=beamEnergy, extras=run_card_extras) == -1:
    raise RuntimeError('Could not create run_card.dat')

# ready to generate
MadGraphUtils.generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat',
                       mode=0, njobs=1, run_name='Test', proc_dir=process_dir)

stringy = 'madgraph.' + str(runArgs.runNumber) + '.MadGraph_' + str(proc_name)

MadGraphUtils.arrange_output(run_name='Test', proc_dir=process_dir,
                             outputDS=stringy + '._00001.events.tar.gz', saveProcDir=save_proc_dir)

# modify the neutrino mass eigenstates to be neutrino flavour eigenstates
print('Modifying the neutrino mass eigenstates to be neutrino flavour eigenstates')
outputDS = 'tmp_' + stringy + '._00001.events.events'
outputZip = 'tmp_' + stringy + '._00001.events.tar.gz'
for line in FileInput(outputDS, inplace=1):
    line = line.replace('8000012', '12')
    line = line.replace('8000014', '14')
    line = line.replace('8000016', '16')
    print line,
print('Re-zipping into dataset name ' + outputZip)
rezip = subprocess.Popen(['tar', 'cvzf', outputZip, outputDS])
rezip.wait()
print('Done')

if 'ATHENA_PROC_NUMBER' in os.environ:
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    if not hasattr(opts, 'nprocs'):
        mglog.warning('Did not see option!')
    else:
        opts.nprocs = 0

evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile = stringy + '._00001.events.tar.gz'

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('Pythia8_i/Pythia8_MadGraph.py')

# filters
if enableFilters:
    include('GeneratorFilters/JetFilterAkt6.py')
    JetFilter = QCDTruthJetFilter
    JetFilter.MinPt = 10000

    include('GeneratorFilters/MultiLeptonFilter.py')
    MultiLeptonFilter = MultiLeptonFilter
    MultiLeptonFilter.Ptcut = 5000
    MultiLeptonFilter.Etacut = 3
    MultiLeptonFilter.NLeptons = 2

    if lightLeptons:
        filtSeq.Expression = "QCDTruthJetFilter and MultiLeptonFilter"
    else:
        include('GeneratorFilters/MultiElecMuTauFilter.py')
        MultiElecMuTauFilter = MultiElecMuTauFilter
        MultiElecMuTauFilter.MinPt = 5000
        MultiElecMuTauFilter.MaxEta = 3
        MultiElecMuTauFilter.NLeptons = 2
        MultiElecMuTauFilter.IncludeHadTaus = True
        MultiElecMuTauFilter.MinVisPtHadTau = 5000

        filtSeq.Expression = "QCDTruthJetFilter and (not MultiLeptonFilter) and MultiElecMuTauFilter"
