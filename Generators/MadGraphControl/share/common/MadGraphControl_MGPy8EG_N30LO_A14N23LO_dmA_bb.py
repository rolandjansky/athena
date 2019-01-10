from MadGraphControl.MadGraphUtils import *
import math

fcard = open('proc_card_mg5.dat','w')
# generate ... QED=0 QCD=3
fcard.write("""
import model ./DMsimp_s_spin1 -modelname
define j = b b~
generate p p > Y1, Y1 > j j

output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")

process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'2.0', 
          'cut_decays' :'F', 
          'pdlabel'    : "'lhapdf'",
          'lhaid'      : 263000,
	  'xptj'       : 50 ,
          'etaj'       : 5 }
#build_run_card(run_card_old='PROC_DMsimp_s_spin1_0/Cards/run_card_default.dat',run_card_new='run_card.dat', xqcut=10,
#               nevts=runArgs.maxEvents*2,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

#from https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MadGraph5aMCatNLOForAtlas#Problems_with_run_card_dat_in_ne    
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat', xqcut=10,
               nevts=runArgs.maxEvents*2,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)
print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_DMsimp_s_spin1.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_DMsimp_s_spin1.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_DMsimp_s_spin1.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if '# gVXd' in line:
            newcard.write('   2 %e # gVXd \n'%(gVXd))
        elif '# gAXd' in line:
            newcard.write('   3 %e # gAXd \n'%(gAXd))
        elif '# gVd11' in line:
            newcard.write('   4 %e # gVd11 \n'%(gVd11))
        elif '# gVu11' in line:
            newcard.write('   5 %e # gVu11 \n'%(gVu11))
        elif '# gVd22' in line:
            newcard.write('   6 %e # gVd22 \n'%(gVd22))
        elif '# gVu22' in line:
            newcard.write('   7 %e # gVu22 \n'%(gVu22))
        elif '# gVd33' in line:
            newcard.write('   8 %e # gVd33 \n'%(gVd33))
        elif '# gVu33' in line:
            newcard.write('   9 %e # gVu33 \n'%(gVu33))
        elif '# gAd11' in line:
            newcard.write('   13 %e # gAd11 \n'%(gAd11))
        elif '# gAu11' in line:
            newcard.write('   14 %e # gAu11 \n'%(gAu11))
        elif '# gAd22' in line:
            newcard.write('   15 %e # gAd22 \n'%(gAd22))
        elif '# gAu22' in line:
            newcard.write('   16 %e # gAu22 \n'%(gAu22))
        elif '# gAd33' in line:
            newcard.write('   17 %e # gAd33 \n'%(gAd33))
        elif '# gAu33' in line:
            newcard.write('   18 %e # gAu33 \n'%(gAu33))
        elif ' MY1 ' in line:
            newcard.write('   5000001 %e # MY1 \n'%(MY1))
        elif 'DECAY 5000001' in line :
            newcard.write('DECAY 5000001 auto #WY1 \n')
        elif ' xd : MXd ' in line:
            newcard.write('   1000022 %e # xd : MXd \n'%(MXd))
        elif ' # MXd ' in line:
            newcard.write('   1000022 %e # MXd \n'%(MXd)) 
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()


runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = "Wimp dmA mediator from DMSimp, ptj>50 GeV"
evgenConfig.keywords = ["exotic","BSM"]
evgenConfig.process = "pp > Y1 > j j"
evgenConfig.inputfilecheck = runName                                                                                                                                    
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                              
evgenConfig.contact = ["Marie-Helene Genest <mgenest@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")
#include("Pythia8_i/Pythia8_aMcAtNlo.py")  

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["1000022:all = xd xd~ 2 0 0 %d 0" %(MXd),
                            "1000022:isVisible = false"]
