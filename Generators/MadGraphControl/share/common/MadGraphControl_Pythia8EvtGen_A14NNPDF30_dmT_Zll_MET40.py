from MadGraphControl.MadGraphUtils import *

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model dmS_T -modelname

define l+ = e+ mu+
define l- = e- mu-
generate p p > Chi Chi~ Z QED=3 QCD=0, Z > l+ l- 

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

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_dmT.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_dmT.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_dmT.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if '# MEtaD' in line:
            newcard.write('  9000006 %e # MEtaD \n'%(MEtaD))
        elif '# MEtaU' in line:
            newcard.write('  9000007 %e # MEtaU \n'%(MEtaU))
        elif '# MEtaS' in line:
            newcard.write('  9000008 %e # MEtaS \n'%(MEtaS))
        elif '# MEtaC' in line:
            newcard.write('  9000009 %e # MEtaC \n'%(MEtaC))
        elif '# MEtaT' in line:
            newcard.write('  9000011 %e # MEtaT \n'%(MEtaT))
        elif '# MEtaB' in line:
            newcard.write('  9000014 %e # MEtaB \n'%(MEtaB))
        elif '# MChi ' in line:
            newcard.write('  1000022 %e # MChi \n'%(MChi))
        elif '# WEtaD' in line:
            newcard.write('DECAY 9000006 %e # WEtaD \n'%(WEtaD))
        elif '# WEtaU' in line:
            newcard.write('DECAY 9000007 %e # WEtaU \n'%(WEtaU))
        elif '# WEtaS' in line:
            newcard.write('DECAY 9000008 %e # WEtaS \n'%(WEtaS))
        elif '# WEtaC' in line:
            newcard.write('DECAY 9000009 %e # WEtaC \n'%(WEtaC))
        elif '# WEtaT' in line:
            newcard.write('DECAY 9000011 %e # WEtaT \n'%(WEtaT))
        elif '# WEtaB' in line:
            newcard.write('DECAY 9000014 %e # WEtaB \n'%(WEtaB))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = new_process()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz') 

#### Shower 
evgenConfig.description = "Wimp pair monoZ with dmST"
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = "pp>chichiz"
evgenConfig.inputfilecheck = runName 
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz' 
evgenConfig.contact = ["Kenji Hamano <kenji.hamano@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

genSeq.Pythia8.Commands += ["1000022:all = chid chid~ 2 0 0 %d 0" %(MChi),
                            "1000022:isVisible = false"]

include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 40*GeV

