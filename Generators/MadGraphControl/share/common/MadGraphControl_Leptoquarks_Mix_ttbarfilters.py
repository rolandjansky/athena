from MadGraphControl.MadGraphUtils import *
import re


nevents=5500
mode=0
#set all couplings to 0 to be sure 
#note that the couplings are lambda*sqrt(BR), we use lambda=0.3 usually
couplingDir = {}
for q in ['u', 'c', 't', 'd', 's','b']:
    for l in ['E','M','T','VE','VM','VT']:
        cpStr = "gs"+q+l+"L"
        couplingDir[cpStr] = 0.0
    
    for l in ['E','M','T']:
        cpStr = "gs"+q+l+"R"
        couplingDir[cpStr] = 0.0

JOname = runArgs.jobConfig[0]
     
#which of the two LQs in the model?
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

evgenConfig.description = ('Pair production of scalar leptoquarks, {0}, mLQ={1:d}').format(lqType,int(lqmass))

decays = []
JOlist = JOname.split("_")
for cp in couplingDir.keys():
    if cp in JOlist:
        idx = JOlist.index(cp)
        cpVal = JOlist[idx+1]
        cpVal = cpVal.replace('p','.')
        couplingDir[cp] = float(cpVal)
        quark = cp[2]
        lepton = ''
        if len(cp)==5:
            lepton = cp[3]
            if lepton=='T':
                lepton = lepton+'A'
            if lepton=='M':
                lepton = lepton+'U'
        elif len(cp)==6:
            lepton = cp[3]+cp[4]
        else:
            raise RuntimeError("Unexpected length of coupling sting.")
        decays.append([quark.lower(),lepton.lower()])
 
#assign correct decays
decayLines = []

#set top decay mode if specified
top_flg=False
for dec in decays:
    if dec[0] in ['t']:
        top_flg=True
tdecay = ""
tbardecay = ""
nLeptons = -99
if "_dil" in JOname:
    if not top_flg:
        raise RuntimeError("Top decay mode should not be used without top quark in the final state.")
    decayLines.append("define lv = e+ mu+ ta+ ve vm vt e- mu- ta- ve~ vm~ vt~")
    tdecay = ", (t > w+ b, w+ > lv lv)"
    tbardecay = ", (t~ > w- b~, w- > lv lv)"
    evgenConfig.description += ', dilepton'
elif "_allhad" in JOname:
    if not top_flg:
        raise RuntimeError("Top decay mode should not be used without top quark in the final state.")
    decayLines.append("define j = g u c d s b u~ c~ d~ s~ b~")
    tdecay = ", (t > w+ b, w+ > j j)"
    tbardecay = ", (t~ > w- b~, w- > j j)"
    evgenConfig.description += ', allhad'
elif "_nonallhad" in JOname:
    if not top_flg:
        raise RuntimeError("Top decay mode should not be used without top quark in the final state.")
    #decayLines.append("define all = g u c d s b u~ c~ d~ s~ b~ e+ mu+ ta+ ve vm vt e- mu- ta- ve~ vm~ vt~")
    tdecay = ", (t > w+ b, w+ > all all)"
    tbardecay = ", (t~ > w- b~, w- > all all)"
    nLeptons = -1
    nevents = nevents*2.0
    evgenConfig.description += ', non-allhad'
else:
    if top_flg:
        evgenConfig.description += ', inclusive'       
        
for dec in decays:
     #charge 2/3
     if lqPDGID==43:
         if dec[0] in ['u','c','t']:
             if dec[1] in ['e','mu','ta']:
                 raise RuntimeError("Fermion charges cannot add up to LQ charge.")
             if dec[0] in ['t']:
                 decayStatement = "decay " + lqType + " > " + dec[0] + " " + dec[1]+"~"+tdecay
                 decayLines.append(decayStatement)
                 decayStatement = "decay " + lqType+"~ > " + dec[0]+"~ " + dec[1]+tbardecay
                 decayLines.append(decayStatement)
             else:
                 decayStatement = "decay " + lqType + " > " + dec[0] + " " + dec[1]+"~"
                 decayLines.append(decayStatement)
                 decayStatement = "decay " + lqType+"~ > " + dec[0]+"~ " + dec[1]
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
             if dec[0] in ['t']:
                 decayStatement = "decay " +  lqType + " > " + dec[0] + " " + dec[1]+"-"+tdecay
                 decayLines.append(decayStatement)
                 decayStatement = "decay " +  lqType+"~ > " + dec[0]+"~ " + dec[1]+"+"+tbardecay
                 decayLines.append(decayStatement)
             else:
                 decayStatement = "decay " +  lqType + " > " + dec[0] + " " + dec[1]+"-"
                 decayLines.append(decayStatement)
                 decayStatement = "decay " +  lqType+"~ > " + dec[0]+"~ " + dec[1]+"+"
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

for l in decayLines:
    mscard.write(l+'\n')

mscard.write("""# running the actual code
launch""")    
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

# --------------------------------------------------------------
# Apply TTbarWToLeptonFilter
# --------------------------------------------------------------
# Only truely needed for non-all hadronic decays
if nLeptons != -99:
  include("GeneratorFilters/TTbarWToLeptonFilter.py")
  filtSeq.TTbarWToLeptonFilter.NumLeptons = nLeptons
  filtSeq.TTbarWToLeptonFilter.Ptcut      = 0.0


evgenConfig.keywords+=['BSM','exotic','leptoquark','scalar']
evgenConfig.generators += ["aMcAtNlo", "Pythia8", "EvtGen"]
evgenConfig.process = 'pp -> LQ LQ'
evgenConfig.contact = ["Ruth Poettgen <ruth.poettgen@cern.ch>"]
runArgs.inputGeneratorFile=outputDS


