from MadGraphControl.MadGraphUtils import *

# Generate A->ZH, H->bb, Z->ll or Z->vv
# Assume job inputs of the form:
# MC15.runNumber.MadGraphPythia8EvtGen_PDF_ggA_mA[massA]_mH[massH]_[Zdecay]bb.py

# parse the job arguments to get mA,mH and Z decay mode
mA = float(runArgs.jobConfig[0].split('.')[2].split('_')[3][2:])
mH = float(runArgs.jobConfig[0].split('.')[2].split('_')[4][2:])
decay = (runArgs.jobConfig[0].split('.')[2].split('_')[5][0:2])

width = 0.0
# so far only support LW A not for H
if len(runArgs.jobConfig[0].split('.')[2].split('_')) == 7:
  width = float(runArgs.jobConfig[0].split('.')[2].split('_')[6][2:])

print "mA ",mA
print "mH ",mH
print "Z decay ",decay

# Zll/Znunu
isZll=True 
if decay=='vv':
    isZll=False

print "isZll ",isZll

# a safe margin for the number of generated events
nevents=int(runArgs.maxEvents*1.4) 
runNumber=runArgs.runNumber 
mode=0 

print 'nevents,runNumber',nevents,runNumber

# create the process string to be copied to proc_card_mg5.dat
process=""
if isZll:
    process="""
import model 2HDM_GF
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~ a
define l+ = e+ mu+ ta+
define l- = e- mu- ta-
# Define multiparticle labels
# Specify process(es) to run
"""
    if mA > mH:
      process += """
generate     g g > h3 > z h2 , z > l- l+, h2 > b b~
output -f
"""
    else:
      proess += """
generate     g g > h2 > z h3 , z > l- l+, h3 > b b~
output -f
"""

else: 
    process="""
import model 2HDM_GF
define p = g u c d s u~ c~ d~ s~ b b~
define j = g u c d s u~ c~ d~ s~ b b~ a
define vl = ve vm vt
define vl~ = ve~ vm~ vt~
# Define multiparticle labels
# Specify process(es) to run
"""
    if mA > mH:
      process += """
generate     g g > h3 > z h2 , z > vl vl~, h2 > b b~
output -f
"""
    else:
      process += """
generate     g g > h2 > z h3 , z > vl vl~, h3 > b b~
output -f
"""

print 'process string: ',process


#---------------------------------------------------------------------------------------------------
# Set masses and widths in param_card.dat
#---------------------------------------------------------------------------------------------------
mh1=125
mh2=mH
mh3=mA
mhc=(mA if mA > mH else mH)
print 'mh1,mh2,mh3,mhc ',mh1,mh2,mh3,mhc
masses ={'25':mh1,  
         '35':mh2,
         '36':mh3, 
         '37':mhc} 

print masses

decays ={'25':'DECAY 25 4.070000e-03 # Wh1',  
         '35':'DECAY 35 1.000000e-03 # Wh2',  
         '36':'DECAY 36 1.000000e-03 # Wh3',  
         '37':'DECAY 37 1.000000e-03 # whc'}  

if width != 0:
  decays ={'25':'DECAY 25 4.070000e-03 # Wh1',  
           '35':'DECAY 35 1.000000e-03 # Wh2',  
           '36':'DECAY 36 %f # Wh3' % (mh3*width/100.),
           '37':'DECAY 37 1.000000e-03 # whc'}  
 
print decays


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")


#Fetch default LO run_card.dat and set parameters (extras)
extras = { 'lhe_version':'2.0', 
           'cut_decays':'F', 
           'pdlabel':"'lhapdf'",
           'lhaid':247000} # NNPDF23_lo_as_0130_qed

runName='run_01'     

# set up process
process_dir = new_process(process) 

build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

#---------------------------------------------------------------------------------------------------
# Using the helper function from MadGraphControl for setting up the param_card
# Build a new param_card.dat from an existing one
#---------------------------------------------------------------------------------------------------
build_param_card(param_card_old='param_card_AZH.dat',param_card_new='param_card_new.dat',masses=masses,decays=decays)
    
print_cards()

# and the generation
generate(run_card_loc='run_card.dat',param_card_loc='param_card_new.dat',mode=mode,proc_dir=process_dir,run_name=runName)
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',saveProcDir=True)  


#### Pythia8 Showering with A14_NNPDF23LO
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_LHEF.py")
   
evgenConfig.generators += ["MadGraph", "Pythia8", "EvtGen"]
if isZll:
    if mA > mH:
      evgenConfig.description = 'mass splitting A(%s GeV)->ZH(%s GeV)->llbb' % (mA,mH)
    else:
      evgenConfig.description = 'mass splitting H(%s GeV)->ZA(%s GeV)->llbb' % (mH,mA)
else:
    if mA > mH:
      evgenConfig.description = 'mass splitting A(%s GeV)->ZH(%s GeV)->nunubb' % (mA,mH)
    else:
      evgenConfig.description = 'mass splitting H(%s GeV)->ZA(%s GeV)->nunubb' % (mH,mA)

evgenConfig.keywords+=['BSMHiggs','bottom']
evgenConfig.inputfilecheck = runName
evgenConfig.contact = [ 'Xiaohu.Sun@cern.ch' ]
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

genSeq.Pythia8.Commands += [
                           ]

#from GeneratorFilters.GeneratorFiltersConf import MultiElecMuTauFilter
#filtSeq += MultiElecMuTauFilter("LepTauPtFilter")
#filtSeq.LepTauPtFilter.IncludeHadTaus = False
#filtSeq.LepTauPtFilter.NLeptons = 1
#filtSeq.LepTauPtFilter.MinPt = 18000.
#filtSeq.LepTauPtFilter.MaxEta = 5.

