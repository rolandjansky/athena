from MadGraphControl.MadGraphUtils import *
import re


nevents=6000
mode=0
lambdalq = 0.3
beta1 = 0.5
beta2 = 0.5
beta3 = 0.0


JOname = runArgs.jobConfig[0]

generation = 0
if "LQ1" in JOname:
    generation = 1
elif "LQ2" in JOname:
    generation = 2
else:
    raise RuntimeError("Cannot determine LQ generation.")     


matches = re.search("M([0-9]+)\.py", JOname)
if matches is None:
    raise RuntimeError("Cannot find mass string.")     
else:
    lqmass = float(matches.group(1))


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

if generation==1:
    fcard.write("""generate p p > lqsd lqsd~ [all=QCD]
output -f""")
    fcard.close()
elif generation==2:
    fcard.write("""generate p p > lqss lqss~ [all=QCD]
output -f""")
    fcard.close()
else: 
    raise RuntimeError("generation indicator %i not recognised in these jobOptions."%generation)



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

if generation==1:
    mscard.write("""decay lqsd > u e-
decay lqsd > d ve
decay lqsd~ > u~ e+
decay lqsd~ > d~ ve~
# running the actual code
launch""")    
    mscard.close()
elif generation==2:
    mscard.write("""decay lqss > c mu-
decay lqss > s vm
decay lqss~ > c~ mu+
decay lqss~ > s~ vm~
# running the actual code
launch""")    
    mscard.close()
else: 
    raise RuntimeError("generation indicator %i not recognised in these jobOptions."%generation)

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

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
  


#### Shower
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")


evgenConfig.description = ('Pair production of scalar leptoquarks, beta=0.5, generation: {0:d}, mLQ={1:d}').format(int(generation), int(lqmass))
evgenConfig.keywords+=['BSM','exotic','leptoquark','scalar']
evgenConfig.generators += ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> LQ LQ'
evgenConfig.contact = ["Ruth Poettgen <ruth.poettgen@cern.ch>"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'


