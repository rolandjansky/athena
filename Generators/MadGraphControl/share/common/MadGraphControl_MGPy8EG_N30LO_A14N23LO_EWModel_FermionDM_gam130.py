from MadGraphControl.MadGraphUtils import *
import math

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model EWModel_FermionDM_UFO -modelname

generate p p > x x~ A QED=3 QCD=0

output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0',
          'cut_decays' :'F',
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 263000,
          'xpta'       : 130 }
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               nevts=runArgs.maxEvents*2,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
    
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_EWModelFermionDM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_EWModelFermionDM.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_EWModelFermionDM.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if 'MX' in line:
            newcard.write('  1000022 %e # MX \n'%(MX))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

process_dir = new_process()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = "Wimp pair monophoton with dmA, pta>130 GeV"
evgenConfig.keywords = ["monophoton","exotic","BSM","WIMP"]
evgenConfig.process = "pp>chichi~a"
evgenConfig.inputfilecheck = runName                                                                                                                                    
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                              
evgenConfig.contact = ["Marie-Helene Genest <mgenest@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["1000022:all = x x~ 2 0 0 "+str(MX)+" 0.0 0.0 0.0 0.0",
                            "1000022:isVisible = false"]
