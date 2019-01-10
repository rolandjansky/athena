import os, subprocess
import MadGraphControl.MadGraphUtils as MG

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model DMPseudoscalarMed_loop -modelname

define pb = p b b~
define jb = j b b~
define W = W+ W-
generate pb pb > chi chi~ W, W > jb jb

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
                  nevts=runArgs.maxEvents*3, rand_seed=runArgs.randomSeed,
                  beamEnergy=beamEnergy, xqcut=0.0, extras=extras)
MG.print_cards()

paramcard = subprocess.check_call(['get_files','-data',
                                   'MadGraph_param_card_dmP_WZhad.dat'])
if not os.access('MadGraph_param_card_dmP_WZhad.dat', os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat', os.R_OK):
    print ('ERROR: Old param card in the current directory.  Dont wan\'t to' 
           'clobber it.  Please move it first.')
else:
    oldcard = open('MadGraph_param_card_dmP_WZhad.dat', 'r')
    newcard = open('param_card.dat', 'w')
    for line in oldcard:
        if 'Mchi' in line:
            if '1000022' in line:
                newcard.write('  1000022 {0:0.6e} # Mchi\n'.format(mDM))
            else:
                newcard.write('  1 {0:0.6e} # Mchi\n'.format(mDM))
        elif 'Mphi' in line:
            if '1000000' in line:
                newcard.write('  1000000 {0:0.6e} # Mphi\n'.format(mMed))
            else:
                newcard.write('  2 {0:0.6e} # Mphi\n'.format(mMed))
        elif 'DECAY 1000000' in line:
            newcard.write('DECAY 1000000 {0:0.6e}\n'.format(wMed))
        elif 'Wphi' in line:
            newcard.write('    3 {0:0.6e} # Wphi\n'.format(wMed))
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


evgenConfig.description = ('Wimp pair mono-WZ with simplified model, dmP, '
                           'M_WIMP={0:d} GeV, M_MED={1:d} GeV').format(int(mDM),
                           int(mMed))
evgenConfig.keywords = ['WZ','exotic','BSM','WIMP']
evgenConfig.process = 'pp -> W chi chibar'
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ['andrew.james.nelson@cern.ch']

include('Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py')
include('Pythia8_i/Pythia8_MadGraph.py')



genSeq.Pythia8.Commands += [('1000022:all = '
                             'chid chid~ 2 0 0 {0:0.6e} 0').format(mDM),
                             '1000022:isVisible = false']



