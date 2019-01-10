from MadGraphControl.MadGraphUtils import *

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model DMScalarMed_loop -modelname

define l+ = e+ mu+
define l- = e- mu-
generate p p > chi chi~ Z QED=3 QCD=0, Z > l+ l- 

output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

### Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 263000 } 
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               nevts=runArgs.maxEvents*3,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_dmS.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_dmS.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_dmS.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' Mphi ' in line:
          if ' 1000000 ' in line:
            newcard.write('  1000000 %e # Mphi \n'%(Mphi))
          else:
            newcard.write('  2 %e # Mphi \n'%(Mphi))
        elif ' Mchi ' in line:
          if ' 1000022 ' in line:
            newcard.write('  1000022 %e # Mchi \n'%(Mchi))
          else:
            newcard.write('  1 %e # Mchi \n'%(Mchi))
        elif 'DECAY ' in line:
          if ' 1000000 ' in line:
            newcard.write('DECAY 1000000 %e # Wphi  \n'%(Wphi)) 
          else:
            newcard.write(line)
        elif ' Wphi ' in line:
            newcard.write('    3 %e # Wphi \n'%(Wphi))
        elif 'gDM' in line:
            newcard.write('    4 %e # gDM \n'%(gDM))
        elif '# gSM' in line:
            newcard.write('    5 %e # gSM \n'%(gSM))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = new_process()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

#### Shower    
evgenConfig.description = "Wimp pair monoZ with dmS"
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = "pp>chichiz"
evgenConfig.inputfilecheck = runName 
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Kenji Hamano <kenji.hamano@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["1000022:all = chid chid~ 2 0 0 %d 0" %(Mchi),
                            "1000022:isVisible = false"]

include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 40*GeV

