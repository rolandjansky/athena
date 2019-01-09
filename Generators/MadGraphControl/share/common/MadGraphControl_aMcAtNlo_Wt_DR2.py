from MadGraphControl.MadGraphUtils import *
import fileinput
import shutil
import subprocess
import os

#############
# General settings
nevents=int(1.1*runArgs.maxEvents)
mode=0

# MG Particle cuts
mllcut=-1

maxjetflavor=5

### DSID list

Wt_incl_H7    = [410400]
Wt_dil_H7     = [410401]
Wt_incl_P8    = [410402]
Wt_dil_P8     = [410403]

mgproc="""generate p p > ttbar w [QCD]"""

if runArgs.runNumber==410400:
    name='Wt_NLO_inclusive_H7'
    decay_mode = 'inclusive'
    parton_shower='HERWIGPP'
elif runArgs.runNumber==410401:
    name='Wt_NLO_dilepton_H7'
    decay_mode = 'dilepton'
    parton_shower='HERWIGPP'
elif runArgs.runNumber==410402:
    name='Wt_NLO_inclusive_P8'
    decay_mode = 'inclusive'
    parton_shower='PYTHIA8'
elif runArgs.runNumber==410403:
    name='Wt_NLO_dilepton_P8'
    decay_mode = 'dilepton'
    parton_shower='PYTHIA8'
else:
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


zdecay="decay z > l+ l-"


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model loop_sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
define w = w+ w-
define ttbar = t t~
"""+mgproc+"""
output -f
""")
fcard.close()


process_dir = new_process()

mglog.info('Hacking common Madgraph code')
##
get_matrix_card = subprocess.Popen(['get_files','-data','fks_singular_MG233.f'])
get_matrix_card.wait()
shutil.copy("fks_singular_MG233.f",process_dir+"/SubProcesses/fks_singular.f")
get_matrix_card = subprocess.Popen(['get_files','-data','coupl_Wt_MG233.inc'])
get_matrix_card.wait()
shutil.copy("coupl_Wt_MG233.inc",process_dir+"/SubProcesses/coupl.inc")
mglog.info('Done hacking common Madgraph code')
mglog.info('Replacing matrix files.')
get_matrix_card = subprocess.Popen(['get_files','-data','bg_matrix_2_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("bg_matrix_2_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_bg_wmt/matrix_2.f")
get_matrix_card = subprocess.Popen(['get_files','-data','bg_matrix_5_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("bg_matrix_5_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_bg_wmt/matrix_5.f")
get_matrix_card = subprocess.Popen(['get_files','-data','bxg_matrix_2_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("bxg_matrix_2_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_bxg_wptx/matrix_2.f")
get_matrix_card = subprocess.Popen(['get_files','-data','bxg_matrix_6_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("bxg_matrix_6_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_bxg_wptx/matrix_6.f")
get_matrix_card = subprocess.Popen(['get_files','-data','gb_matrix_4_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("gb_matrix_4_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_gb_wmt/matrix_4.f")
get_matrix_card = subprocess.Popen(['get_files','-data','gb_matrix_6_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("gb_matrix_6_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_gb_wmt/matrix_6.f")
get_matrix_card = subprocess.Popen(['get_files','-data','gbx_matrix_5_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("gbx_matrix_5_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_gbx_wptx/matrix_5.f")
get_matrix_card = subprocess.Popen(['get_files','-data','gbx_matrix_6_Wt_DR2_MG233.f'])
get_matrix_card.wait()
shutil.copy("gbx_matrix_6_Wt_DR2_MG233.f",process_dir+"/SubProcesses/P0_gbx_wptx/matrix_6.f")
##
mglog.info('Matrix files replaced.')

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")


lhaid=260000
pdflabel='lhapdf'

#Fetch default LO run_card.dat and set parameters
extras = { 'lhaid'         : lhaid,
	   'pdlabel'       : "'"+pdflabel+"'",
           'maxjetflavor'  : maxjetflavor,
           'parton_shower' : parton_shower,
	   'fixed_ren_scale' : "True",
	   'fixed_fac_scale' : "True",
	   'muR_ref_fixed' : 172.5,
	   'muF1_ref_fixed' : 172.5,
           'muF2_ref_fixed' : 172.5,
	   'fixed_QES_scale' : "True",
	   'QES_ref_fixed' : 172.5
	}

doSyst=True

if doSyst:
    lhe_version=3
    extras.update({'reweight_scale': '.true.',
                   'rw_Rscale_down':  0.5,
                   'rw_Rscale_up'  :  2.0,
                   'rw_Fscale_down':  0.5,
                   'rw_Fscale_up'  :  2.0,
                   'reweight_PDF'  : '.true.',
                   'PDF_set_min'   : 260001,
                   'PDF_set_max'   : 260100,
                   'mll_sf'        : mllcut})
else:
    lhe_version=1
    extras.update({'reweight_scale': '.false.',
                   'reweight_PDF'  : '.false.'})


madspin_card_loc='madspin_card.dat'

if decay_mode=='inclusive':
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
 # set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
  set max_weight_ps_point 500 
  set Nevents_for_max_weigth 500
  set BW_cut 50
 #
 set seed %i
 # specify the decay for the final state particles
 define q = u d s c b
 define q~ = u~ d~ s~ c~ b~
 define l+ = e+ mu+ ta+
 define l- = e- mu- ta-
 define v = ve vm vt
 define v~ = ve~ vm~ vt~
 decay t > w+ b, w+ > all all
 decay t~ > w- b~, w- > all all
 decay w+ > all all
 decay w- > all all
 %s
 # running the actual code
 launch"""%(runArgs.randomSeed,zdecay))
 mscard.close()

elif decay_mode=='dilepton':
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
 # set max_weight_ps_point 400  # number of PS to estimate the maximum for each event
  set max_weight_ps_point 500 
  set Nevents_for_max_weigth 500
  set BW_cut 50
 #
 set seed %i
 # specify the decay for the final state particles
 define q = u d s c b
 define q~ = u~ d~ s~ c~ b~
 define l+ = e+ mu+ ta+
 define l- = e- mu- ta-
 define v = ve vm vt
 define v~ = ve~ vm~ vt~
 decay t > w+ b, w+ > l+ v
 decay t~ > w- b~, w- > l- v~
 decay w+ > l+ v
 decay w- > l- v~
 %s
 # running the actual code
 launch"""%(runArgs.randomSeed,zdecay))
 mscard.close()


build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()



param_card_loc='aMcAtNlo_param_card_loopsmnobmass.dat'
paramcard = subprocess.Popen(['get_files','-data',param_card_loc])
paramcard.wait()
if not os.access(param_card_loc,os.R_OK):
    raise RuntimeError("ERROR: Could not get %s"%param_card.loop_sm-no_b_mass.dat)



generate(run_card_loc='run_card.dat',param_card_loc=param_card_loc,mode=mode,proc_dir=process_dir,madspin_card_loc=madspin_card_loc)
outputDS=arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz', lhe_version=lhe_version)

#### Shower


if parton_shower=='PYTHIA8':
 ##########
 ## run Pythia8 on-the-fly -----------------------------------------------------
 ## Provide config information
 evgenConfig.generators += ["aMcAtNlo", "Pythia8"]
 evgenConfig.description = "MG5aMCatNLO/Pythia8 Wt DR2 " + decay_mode
 evgenConfig.keywords    = ["SM","top"]
 evgenConfig.contact     = ["olga.bylund@cern.ch"]
 #evgenConfig.minevents = nevents
 runArgs.inputGeneratorFile=outputDS

 include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
 include("Pythia8_i/Pythia8_aMcAtNlo.py")

elif parton_shower=="HERWIGPP":
 ## run Herwig7 on-the-fly -----------------------------------------------------
 ## Provide config information
 evgenConfig.generators += ["aMcAtNlo", "Herwig7"]
 evgenConfig.tune        = "MMHT2014"
 evgenConfig.description = "MG5aMCatNLO/Herwig7 Wt DR2"+decay_mode
 evgenConfig.keywords    = ["SM","top"]
 evgenConfig.contact     = ["olga.bylund@cern.ch"]
 #evgenConfig.minevents = nevents
 runArgs.inputGeneratorFile=outputDS

 ## initialize Herwig7 generator configuration object
 from Herwig7_i.Herwig7_iConf import Herwig7
 from Herwig7_i.Herwig7ConfigLHEF import Hw7ConfigLHEF
 genSeq += Herwig7()
 Herwig7Config = Hw7ConfigLHEF(genSeq, runArgs, run_name="HerwigLHEF")
 ## configure generator
 Herwig7Config.me_pdf_commands(order="NLO", name="NNPDF30_nlo_as_0118")
 Herwig7Config.tune_commands()
 Herwig7Config.lhef_mg5amc_commands(lhe_filename=runArgs.inputGeneratorFile,me_pdf_order="NLO")

 # add EvtGen
 include("Herwig7_i/Herwig7_EvtGen.py")

 #New commands to fix QED radiation
 Herwig7Config.add_commands("""
set /Herwig/Shower/LtoLGammaSudakov:pTmin 0.000001
set /Herwig/Shower/QtoGammaQSudakov:Alpha /Herwig/Shower/AlphaQED 
""")
 
 ## run generator
 Herwig7Config.run()

else:
 print "Parton shower not specified, should be HERWIGPP or Pythia8"