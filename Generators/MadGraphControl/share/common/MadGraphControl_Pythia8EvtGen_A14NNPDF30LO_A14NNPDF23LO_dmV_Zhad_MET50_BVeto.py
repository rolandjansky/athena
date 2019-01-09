import os, subprocess
import MadGraphControl.MadGraphUtils as MG

safefactor=3.

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model dmV -modelname

define pb = p b b~
define jb = j b b~
generate pb pb > chi chi~ Z, Z > jb jb

output -f
""")
fcard.close()

beamEnergy = -999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


extras = {
          'lhe_version': '2.0',
          'cut_decays': 'F',
          'pdlabel': "'lhapdf'",
          'lhaid': 263000,
          'drjj': "0.0",
         }
MG.build_run_card(run_card_old=MG.get_default_runcard(),
                  run_card_new='run_card.dat',
                  nevts=int(runArgs.maxEvents*safefactor), rand_seed=runArgs.randomSeed,
                  beamEnergy=beamEnergy, xqcut=0.0, extras=extras)
MG.print_cards()

paramcard = subprocess.check_call(['get_files','-data',
                                   'MadGraph_param_card_dmV_WZhad.dat'])
if not os.access('MadGraph_param_card_dmV_WZhad.dat', os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat', os.R_OK):
    print ('ERROR: Old param card in the current directory.  Dont wan\'t to' 
           'clobber it.  Please move it first.')
else:
    oldcard = open('MadGraph_param_card_dmV_WZhad.dat', 'r')
    newcard = open('param_card.dat', 'w')
    for line in oldcard:
        if 'gxichi' in line:
            newcard.write('  1 {0:0.6e} # gxichi \n'.format(gxichi))
        elif '# gxiU' in line:
            newcard.write('  2 {0:0.6e} # gxiU \n'.format(gxiU))
        elif '# gxiD' in line:
            newcard.write('  3 {0:0.6e} # gxiD \n'.format(gxiU))
        elif '# gxiS' in line:
            newcard.write('  4 {0:0.6e} # gxiS \n'.format(gxiU))
        elif '# gxiC' in line:
            newcard.write('  5 {0:0.6e} # gxiC \n'.format(gxiU))
        elif '# gxiB' in line:
            newcard.write('  6 {0:0.6e} # gxiB \n'.format(gxiU))
        elif '# gxiT' in line:
            newcard.write('  7 {0:0.6e} # gxiT \n'.format(gxiU))
        elif 'Mchi' in line:
            newcard.write('  1000022 {0:0.6e} # Mchi\n'.format(mDM))
        elif 'Mxi' in line:
            newcard.write('  101 {0:0.6e} # Mxi\n'.format(mMed))
        elif 'Wxi' in line:
            newcard.write('DECAY 101 {0:0.6e} # Wxi\n'.format(wMed))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = MG.new_process()

runName='run_01'

MG.generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat',
            mode=0, njobs=1, run_name=runName, proc_dir=process_dir)

MG.arrange_output(run_name=runName, proc_dir=process_dir,
                  outputDS=runName+'._00001.events.tar.gz')


evgenConfig.description = ('Wimp pair mono-WZ with simplified model, dmV, '
                           'M_WIMP={0:d} GeV, M_MED={1:d} GeV').format(int(mDM),
                           int(mMed))
evgenConfig.keywords = ['WZ','exotic','BSM','WIMP']
evgenConfig.process = 'pp -> Z chi chibar'
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ['andrew.james.nelson@cern.ch']

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('Pythia8_i/Pythia8_MadGraph.py')



genSeq.Pythia8.Commands += [('1000022:all = '
                             'chid chid~ 2 0 0 {0:0.6e} 0').format(mDM),
                             '1000022:isVisible = false']



## MET filter
include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 50*GeV

## B filter/veto
include("GeneratorFilters/BHadronFilter.py")
filtSeq += HeavyFlavorBHadronFilter 

filtSeq.Expression = "(MissingEtFilter) and (not HeavyFlavorBHadronFilter)"

