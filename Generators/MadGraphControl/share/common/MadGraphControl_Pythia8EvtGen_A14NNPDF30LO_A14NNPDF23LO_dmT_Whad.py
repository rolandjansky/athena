import os, subprocess
import MadGraphControl.MadGraphUtils as MG

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model dmS_T -modelname

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
                                   'MadGraph_param_card_dmT_WZhad.dat'])
if not os.access('MadGraph_param_card_dmT_WZhad.dat', os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat', os.R_OK):
    print ('ERROR: Old param card in the current directory.  Dont wan\'t to' 
           'clobber it.  Please move it first.')
else:
    oldcard = open('MadGraph_param_card_dmT_WZhad.dat', 'r')
    newcard = open('param_card.dat', 'w')
    for line in oldcard:
        if '# MEtaD' in line:
            newcard.write('  9000006 {0:0.6e} # MEtaD \n'.format(mMed))
        elif '# MEtaU' in line:
            newcard.write('  9000007 {0:0.6e} # MEtaU \n'.format(mMed))
        elif '# MEtaS' in line:
            newcard.write('  9000008 {0:0.6e} # MEtaS \n'.format(mMed))
        elif '# MEtaC' in line:
            newcard.write('  9000009 {0:0.6e} # MEtaC \n'.format(mMed))
        elif '# MEtaT' in line:
            newcard.write('  9000011 {0:0.6e} # MEtaT \n'.format(mMed))
        elif '# MEtaB' in line:
            newcard.write('  9000014 {0:0.6e} # MEtaB \n'.format(mMed))
        elif '# MChi' in line:
            newcard.write('  1000022 {0:0.6e} # MChi \n'.format(mDM))
        elif '# WEtaD' in line:
            newcard.write('DECAY 9000006 {0:0.6e} # WEtaD \n'.format(wEtad))
        elif '# WEtaU' in line:
            newcard.write('DECAY 9000007 {0:0.6e} # WEtaU \n'.format(wEtau))
        elif '# WEtaS' in line:
            newcard.write('DECAY 9000008 {0:0.6e} # WEtaS \n'.format(wEtas))
        elif '# WEtaC' in line:
            newcard.write('DECAY 9000009 {0:0.6e} # WEtaC \n'.format(wEtac))
        elif '# WEtaT' in line:
            newcard.write('DECAY 9000011 {0:0.6e} # WEtaT \n'.format(wEtat))
        elif '# WEtaB' in line:
            newcard.write('DECAY 9000014 {0:0.6e} # WEtaB \n'.format(wEtab))
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


evgenConfig.description = ('Wimp pair mono-WZ with simplified model, dmST, '
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



