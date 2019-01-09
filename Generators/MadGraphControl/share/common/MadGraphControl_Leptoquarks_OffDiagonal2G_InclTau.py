from MadGraphControl.MadGraphUtils import *
import re
import numpy as np


nevents=runArgs.maxEvents
mode=0
# set all couplings to 0 to be sure 
couplingDir = {}
for q in ['u', 'c', 't', 'd', 's','b']:
    for l in ['E','M','T','VE','VM','VT']:
        cpStr = "gs"+q+l+"L"
        couplingDir[cpStr] = 0.0
    for l in ['E','M','T']:
        cpStr = "gs"+q+l+"R"
        couplingDir[cpStr] = 0.0

JOname = runArgs.jobConfig[0]
     
# which of the two LQs in the model?
# lqsd: pdg ID 42, charge -1/3, labelled "down" in the JOs
# lqsu: pdg ID 43, charge 2/3, labelled "up" in the JOs
lqType = ""
lqPDGID = 0
if "_LQu_" in JOname:
    lqType = "lqsu"
    lqPDGID = 43
elif "_LQd_" in JOname:
    lqType = "lqsd"
    lqPDGID = 42
else:
    raise RuntimeError("Cannot find LQ type in JO name.")

if not lqType:
    raise RuntimeError("No LQ type set.")
if not lqPDGID:
    raise RuntimeError("No LQ PDG ID set.")

matchesMass = re.search("M([0-9]+)\.py", JOname)
if matchesMass is None:
    raise RuntimeError("Cannot find mass string in JO name.")     
else:
    lqmass = float(matchesMass.group(1))

decays = []
JOlist = JOname.split("_")

###########################################################################################
# couplings are calculated as lambda * sqrt(beta) and lambda * sqrt(1 - beta) for decays  #
# in charged and uncharged leptons, respectively                                          #
# default values: beta = 0.5, lambda = 0.3                                                #
# values can be changed by including "lmbd_*p*" and "beta_*p*" in the JO filename         #
# hnd is the fraction of charged leptons that are left-handed, i.e. applied as sqrt(hdn)  #
# for left- and sqrt(1 - hnd) for right-handed leptons, respectively                      #
# default value is hnd = 1.0, can be changed by including "hnd_*p*" in the JO filename    #
###########################################################################################

lambda_cp = 0.3
if "ld" in JOlist: lambda_cp = eval( JOlist[ JOlist.index("ld")+1 ].replace('p','.') )

beta = 0.5
if "beta" in JOlist: beta = eval( JOlist[ JOlist.index("beta")+1 ].replace('p','.') )

hnd = 1.0
if "hnd" in JOlist: hnd = eval( JOlist[ JOlist.index("hnd")+1 ].replace('p','.') )

if beta > 1.0 or beta < 0.0: raise RuntimeError("Unexpected value for beta. Should be in the range from 0 to 1.")
if hnd > 1.0 or hnd < 0.0: raise RuntimeError("Unexpected value for hnd. Should be in the range from 0 to 1.")

f_uncharged = np.sqrt( 1 - beta )
f_charged = np.sqrt( beta )
f_lefthanded = np.sqrt( hnd )
f_righthanded = np.sqrt( 1 - hnd )

cps = []
if "_ta_" in JOname:
    if lqPDGID == 43: cps.extend( ["sTL", "sTR", "cVTL"] )
    elif lqPDGID == 42:  cps.extend( ["sVTL", "cTL", "cTR"] )

if "_mu_" in JOname:
    if lqPDGID == 43: cps.extend( ["sML", "sMR", "cVML"] ) # b,d,s = -1/3 t,c,u = 2/3
    elif lqPDGID == 42:  cps.extend( ["sVML", "cML", "cMR"] )

if "_el_" in JOname:
    if lqPDGID == 43: cps.extend( ["sEL", "sER", "cVEL"] )
    elif lqPDGID == 42:  cps.extend( ["sVEL", "cEL", "cER"] )

for cp in cps:
    if "L" in cp:
        if "V" in cp: 
            lepton = cp[1]+cp[2]
            couplingDir["gs"+cp] = lambda_cp * f_uncharged
            if f_uncharged: decays.append([cp[0],lepton.lower()])
        else:
            lepton = cp[1]
            couplingDir["gs"+cp] = lambda_cp * f_charged * f_lefthanded
            if f_charged * f_lefthanded: decays.append([cp[0],lepton.lower().replace("m","mu").replace("t","ta")])
    else:
        lepton = cp[1]
        couplingDir["gs"+cp] = lambda_cp * f_charged * f_righthanded
        if f_charged * f_righthanded and not f_charged * f_lefthanded: decays.append([cp[0],lepton.lower().replace("m","mu").replace("t","ta")])    
            
#assign correct decays
decayLines = []
for dec in decays:
     #charge 2/3
     if lqPDGID==43:
         if dec[0] in ['u','c','t']:
             if dec[1] in ['e','mu','ta']:
                 raise RuntimeError("Fermion charges cannot add up to LQ charge.")
             decayStatement = "decay " + lqType + " > " + dec[0] + " " + dec[1]+"~, (t > w+ b, w+ > all all)"
             decayLines.append(decayStatement)
             decayStatement = "decay " + lqType+"~ > " + dec[0]+"~ " + dec[1]+", (t~ > w- b~, w- > all all)"
             decayLines.append(decayStatement)
         elif dec[0] in ['d','s','b']:
             if 'v' in dec[1]:
                 raise RuntimeError("Fermion charges cannot add up to LQ charge.")
             decayStatement = "decay " +  lqType + " > " + dec[0] + " " +dec[1]+"+"
             decayLines.append(decayStatement)
             decayStatement = "decay " +  lqType + "~ > " + dec[0]+"~ " + dec[1]+"-"
             decayLines.append(decayStatement)
         else:
             raise RuntimeError("Unexpected quark flavour.")
     
      #charge -1/3     
     if lqPDGID==42:
         if dec[0] in ['d','s','b']:
             if dec[1] in ['e','mu','ta']:
                 raise RuntimeError("Fermion charges cannot add up to LQ charge.")
             decayStatement = "decay " +  lqType + " > " + dec[0] + " " + dec[1]
             decayLines.append(decayStatement)
             decayStatement = "decay " +  lqType+"~ > " + dec[0]+"~ " + dec[1]+"~"
             decayLines.append(decayStatement)
         elif dec[0] in ['u','c','t']:
             if 'v' in dec[1]:
                 raise RuntimeError("Fermion charges cannot add up to LQ charge.")
             decayStatement = "decay " +  lqType + " > " + dec[0] + " " + dec[1]+"-, (t > w+ b, w+ > all all)"
             decayLines.append(decayStatement)
             decayStatement = "decay " +  lqType+"~ > " + dec[0]+"~ " + dec[1]+"+, (t~ > w- b~, w- > all all)"
             decayLines.append(decayStatement)
         else:
             raise RuntimeError("Unexpected quark flavour.")


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
import model LQmix_NLO\n""")

fcard.write("""generate p p > {0} {0}~ [all=QCD]
output -f""".format(lqType))
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
               nevts=1.1*nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)



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
define l+ = e+ mu+
define l- = e- mu-
define vl = ve vm vt
define vl~ = ve~ vm~ vt~\n"""%runArgs.randomSeed)

for l in decayLines:
    mscard.write(l+'\n')

mscard.write("""launch""")    
mscard.close()

paramcardname = 'aMCatNLO_param_card_LQmix.dat'
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
        if 'MU_R' in line:
            newcard.write('    1 %e # MU_R\n'%(lqmass))
        elif '# Msd' in line:
            newcard.write('   42 %e # Msd \n'%(lqmass))
        elif '# Msu' in line:
            newcard.write('   43 %e # Msu \n'%(lqmass))
        elif 'DECAY  42' in line:
            if lqPDGID==42:
                newcard.write('DECAY  42 Auto # Wsd\n')
            else:
                newcard.write(line)
        elif 'DECAY  43' in line:
            if lqPDGID==43:
                newcard.write('DECAY  43 Auto # Wsu\n')
            else:
                newcard.write(line)
        elif '# gs' in line:
            for cp in couplingDir.keys():
                if cp in line:
                    linelist = line.split(' ')
                    linelist = ' '.join(linelist).split()
                    lhacode = linelist[0]
                    newcard.write('    %s %e # %s\n'%(lhacode, couplingDir[cp], cp))
                    break
            
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

print_cards()

       
runName='run_01'     

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName,madspin_card_loc=madspin_card_loc)

stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+lqType+str(lqmass)
outputDS=arrange_output(run_name=runName,proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)  
  


#### Shower
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_aMcAtNlo.py")


evgenConfig.description = ('Pair production of scalar leptoquarks, {0}, mLQ={1:d}').format(lqType,int(lqmass))
evgenConfig.keywords+=['BSM','exotic','leptoquark','scalar']
evgenConfig.generators += ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> LQ LQ'
evgenConfig.contact = ["Adam Jaspan <adam.elliott.jaspan@cern.ch>"]
evgenConfig.minevents = 5000
runArgs.inputGeneratorFile=outputDS



