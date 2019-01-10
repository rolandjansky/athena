from MadGraphControl.MadGraphUtils import *
import math

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model dmV -modelname

define l+ = e+ m+
define l- = e- m-
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
               nevts=runArgs.maxEvents*10,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

    
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_dmV.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_dmV.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_dmV.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if 'gxichi' in line:
            newcard.write('  1 %e # gxichi \n'%(gxichi))
        elif '# gxiU' in line:
            newcard.write('  2 %e # gxiU \n'%(gxiU))
        elif '# gxiD' in line:
            newcard.write('  3 %e # gxiD \n'%(gxiU))
        elif '# gxiS' in line:
            newcard.write('  4 %e # gxiS \n'%(gxiU))
        elif '# gxiC' in line:
            newcard.write('  5 %e # gxiC \n'%(gxiU))
        elif '# gxiB' in line:
            newcard.write('  6 %e # gxiB \n'%(gxiU))
        elif '# gxiT' in line:
            newcard.write('  7 %e # gxiT \n'%(gxiU))
        elif ' Mxi ' in line:
            newcard.write('  101 %e # Mxi \n'%(Mxi))
        elif ' Wxi ' in line:
            newcard.write('DECAY 101 %e # Wxi  \n'%(Wxi))
        elif ' Mchi ' in line:
            newcard.write('  1000022 %e # Mchi \n'%(Mchi))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = new_process()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz') 

#### Shower 
evgenConfig.description = "Wimp pair monoZ with dmV"
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

