from MadGraphControl.MadGraphUtils import *
import re


nevents=6000
mode=0
lambdalq = 0.3
beta1 = 0.5
beta2 = 0.5
beta3 = 1.0


JOname = runArgs.jobConfig[0]

generation = 0
if "LQ3" in JOname:
    generation = 3
else:
    raise RuntimeError("Cannot determine LQ generation.")     

LQisUp = False
if "_up_" in JOname:
    LQisUp = True
elif "_down_" in JOname:
    LQisUp= False
else:
    raise RuntimeError("Cannot determine LQ type.") 
       

matches = re.search("M([0-9]+)\.py", JOname)
if matches is None:
    raise RuntimeError("Cannot find mass string.")     
else:
    lqmass = float(matches.group(1))

betaString = "" 
matches = re.search("beta([0-9]+)\_", JOname)
if matches is None:
    raise RuntimeError("Cannot find beta string.")     

betaString = matches.group(1)
print betaString
value = float(betaString)
exp = betaString.count('0')

beta3 = value * 10**(-exp)
print beta3

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
set group_subprocesses Auto
set ignore_six_quark_processes False
set loop_color_flows False
set gauge unitary
set complex_mass_scheme False
set max_npoint_for_channel 0
import model sm
define p = g u c d s u~ c~ d~ s~
define j = g u c d s u~ c~ d~ s~
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
define p = p b b~
define j = j b b~
import model LQnomix_NLO-no_b_mass\n""")

if LQisUp:
    fcard.write("""generate p p > lqst lqst~ [all=QCD]
output -f""")
    fcard.close()
else:
    fcard.write("""generate p p > lqsb lqsb~ [all=QCD]
output -f""")
    fcard.close()



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")



process_dir = new_process()

#Fetch default NLO run_card.dat and set parameters
#PDF sets: NNPDF30_nlo_as_0118, _as_0115, _as_0117, _as_0119, _as_121, MMHT2014nlo68cl, CT14nlo , , 264000, 265000, 266000, 267000, 25100, 13100
extras = { 'pdlabel'       :"'lhapdf'",
           'lhaid'         :"260000 264000 265000 266000 267000 25100 13100",
           'parton_shower' :'PYTHIA8',
           'reweight_scale':'True',
           'reweight_PDF':'True False False False False False False',
           'jetalgo':'-1.0',
           'jetradius':'0.4',
           'muR_ref_fixed' : lqmass,
           'muF_ref_fixed' : lqmass,
           'QES_ref_fixed' : lqmass}
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)



madspin_card_loc='madspin_card.dat'                                                                                                                                    

mscard = open(madspin_card_loc,'w')                                                                                                                                    
mscard.write("""#************************************************************
#*                        MadSpin                           *
#*                                                          *
#*    P. Artoisenet, R. Frederix, R. Rietkerk, O. Mattelaer *
#*                                                          *
#*    Part of the MadGraph5_aMC@NLO Framework:              *
#*    The MadGraph5_aMC@NLO Development Team - Find us at   *
#*    https://server06.fynu.ucl.ac.be/projects/madgraph     *
#*                                                          *
#************************************************************
#Some options (uncomment to apply)
#
# set seed 1
# set Nevents_for_max_weigth 75 # number of events for the estimate of the max. weight
# set BW_cut 15                # cut on how far the particle can be off-shell
 set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
#
set seed %i
# specify the decay for the final state particles
decay t > w+ b, w+ > all all
decay t~ > w- b~, w- > all all
decay w+ > all all
decay w- > all all
decay z > all all\n"""%runArgs.randomSeed)

if LQisUp:
    if 'beta1_' in JOname:
        mscard.write("""decay lqst > b ta+
decay lqst~ > b~ ta-
launch""")    
        mscard.close()
    elif 'beta0_' in JOname:
        mscard.write("""decay lqst > t vt~
decay lqst~ >  t~ vt
launch""")    
        mscard.close()
    else:
        mscard.write("""decay lqst > b ta+
decay lqst > t vt~
decay lqst~ > b~ ta-
decay lqst~ > t~ vt
launch""")    
        mscard.close()
else:
    if 'beta1_' in JOname:
        mscard.write("""decay lqsb > t ta-
decay lqsb~ > t~ ta+
launch""")    
        mscard.close()
    elif 'beta0_' in JOname:
        mscard.write("""decay lqsb > b vt
decay lqsb~ > b~ vt~
launch""")    
        mscard.close()
    else:
        mscard.write("""decay lqsb > b vt
decay lqsb > t ta-
decay lqsb~ > b~ vt~
decay lqsb~ > t~ ta+
launch""")    
        mscard.close()

paramcardname = 'aMCatNLO_param_card_LQ.dat'
paramcard = subprocess.Popen(['get_files','-data',paramcardname])
paramcard.wait()
if not os.access(paramcardname,os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(paramcardname,'r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if 'mu_r' in line:
            newcard.write('    1 %e # mu_r\n'%(lqmass))
        elif '# msu' in line:
            newcard.write('  601 %e # msu \n'%(lqmass))
        elif '# msd' in line:
            newcard.write('  602 %e # msd\n'%(lqmass)) 
        elif '# msc' in line:
            newcard.write('  603 %e # msc\n'%(lqmass))
        elif '# mss' in line:
            newcard.write('  604 %e # mss\n'%(lqmass))
        elif '# mst' in line:
            newcard.write('  605 %e # mst\n'%(lqmass))
        elif '# msb' in line:
            newcard.write('  606 %e # msb\n'%(lqmass))
        elif 'Wsu' in line:
            newcard.write('DECAY 601 Auto # Wsu\n')
        elif 'Wsd' in line:
            newcard.write('DECAY 602 Auto # Wsd\n')
        elif 'Wsc' in line:
            newcard.write('DECAY 603 Auto # Wsc\n')
        elif 'Wss' in line:
            newcard.write('DECAY 604 Auto # Wss\n')
        elif 'Wst' in line:
            newcard.write('DECAY 605 Auto # Wst\n')
        elif 'Wsb' in line:
            newcard.write('DECAY 606 Auto # Wsb\n')
        elif '# lm1' in line:
            newcard.write('    1 %e # lm1\n'%(lambdalq))
        elif '# lm2' in line:
            newcard.write('    1 %e # lm2\n'%(lambdalq))
        elif '# lm3' in line:
            newcard.write('    1 %e # lm3\n'%(lambdalq))
        elif '# beta1' in line:
            newcard.write('    2 %e # beta1\n'%(beta1))
        elif '# beta2' in line:
            newcard.write('    2 %e # beta2\n'%(beta2))
        elif '# beta3' in line:
            newcard.write('    2 %e # beta3\n'%(beta3))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

print_cards()
    
runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_LQ3'
if LQisUp:
	stringy=stringy+"up"
else:
	stringy= stringy+"down" 
stringy+="beta"+str(beta3) 
outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
  


#### Shower
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")

descr = "Pair production of scalar 3rd generation scalar leptoquarks, "
if LQisUp:
    descr = descr + "up-type"
else:
    descr = descr + "down-type"  
descr = descr + ", beta = " + str(beta3) + ", mLQ=" + str(lqmass)  


evgenConfig.description = (descr)
evgenConfig.keywords+=['BSM','exotic','leptoquark','scalar']
evgenConfig.generators += ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> LQ LQ'
evgenConfig.contact = ["Ruth Poettgen <ruth.poettgen@cern.ch>"]
runArgs.inputGeneratorFile=outputDS+'._00001.events.tar.gz'


