from MadGraphControl.MadGraphUtils import *
import math

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


if (channel == "zzprime"):
    # MadgGaph configuration
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model DMsimp_s_spin1

    define l = e- e+ mu- mu+
    define v = ve ve~ vm vm~ vt vt~

    generate p p > l l y1, y1 > j j

    output -f""")
    fcard.close()

elif (channel == "wzprime"):
    # MadgGaph configuration
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model DMsimp_s_spin1

    define l = e- e+ mu- mu+
    define v = ve ve~ vm vm~ vt vt~

    generate p p > l v y1, y1 > j j

    output -f""")
    fcard.close()

else:
    print 'ERROR: Could not find production channel'


process_dir = new_process()

#Fetch default LO run_card.dat and set parameters
extras = {'lhe_version':'3.0', 
          'cut_decays' :'F',
	  'bwcutoff'   :'40', 
          'pdlabel'    :"'lhapdf'",
          'lhaid'      :'260000'}


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=int(runArgs.maxEvents*1.5),
               rand_seed=runArgs.randomSeed,
               beamEnergy=beamEnergy,
               extras=extras)


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
        if '# gVXc' in line:
            newcard.write('   1 %e # gVXc \n'%(gVXc))
        elif '# gVXd' in line:
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
        elif '# gVl11' in line:
            newcard.write('   10 %e # gVl11 \n'%(gVl11))
        elif '# gVl22' in line:
            newcard.write('   11 %e # gVl22 \n'%(gVl22))
        elif '# gVl33' in line:
            newcard.write('   12 %e # gVl33 \n'%(gVl33))
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
        elif '# gAl11' in line:
            newcard.write('   19 %e # gAl11 \n'%(gAl11))
        elif '# gAl22' in line:
            newcard.write('   20 %e # gAl22 \n'%(gAl22))
        elif '# gAl33' in line:
            newcard.write('   21 %e # gAl33 \n'%(gAl33))
        elif '# gnu11' in line:
            newcard.write('   22 %e # gnu11 \n'%(gnu11))
        elif '# gnu22' in line:
            newcard.write('   23 %e # gnu22 \n'%(gnu22))
        elif '# gnu33' in line:
            newcard.write('   24 %e # gnu33 \n'%(gnu33))
        elif '# gVh' in line:
            newcard.write('   25 %e # gVh \n'%(gVh))
        elif ' MY1 ' in line:
            newcard.write('   5000001 %e # MY1 \n'%(MY1))
        elif 'DECAY 5000001 ' in line:
            newcard.write('DECAY 5000001 %e # WY1  \n'%(WY1))
        elif ' xd : MXd ' in line:
            newcard.write('   1000022 %e # xd : MXd \n'%(MXd))
        elif ' # MXd ' in line:
            newcard.write('   1000022 %e # MXd \n'%(MXd)) 
        elif ' # MXr ' in line:
            newcard.write('   5000511 %e # MXd \n'%(MXr))
        elif ' # MXc ' in line:
            newcard.write('   5000512 %e # MXd \n'%(MXc))
        else:
           newcard.write(line)
    oldcard.close()
    newcard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')                                                                          
                                                                                                                                                                        
#### Shower                                                                                                                                                             
evgenConfig.description = 'pp > l l/v y1, y1 > jj - model DMsimp_s_spin1'
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = 'pp>l l/v y1, y1>jj'
evgenConfig.inputfilecheck = runName                                         
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")


