from MadGraphControl.MadGraphUtils import *
import re


## I am following examples here:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/MadGraph5aMCatNLOForAtlas#LO_Pythia8_Showering_Inclusive_s

# I am running it running via:
# asetup MCProd,19.2.5.19.3,here
# Generate_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=308753 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=MC15.308753.MadGraphPythia8EvtGen_A14NNPDF23_MDM_Merge_M90GeV.py

### the model contains a new BSM triplet (minimal dark matter triplet):  chi=(chi0, chi+ , chi-)
### chi0 is the dark matter candidate, i.e. neutralino n1, the susy neutralino ID is already given to chi0 in the UFO file
### I am generating events with chi particles and 2 jets in final state

def StringToFloat(s):
  if "M" in s:
    newString = s.replace("M", "")
    if "GeV" in newString:
      newFloat = float(newString.replace("GeV.py", "."))
#  print "newFloat check = ", newFloat    
  return float(newFloat)


mode=0

# define some run_card parameters
bwcutoff = 100
deltaeta = 3.0
Mjj = 500.0
ptj_min = 40.0

## define the param_card parameters
#mass = 0.9000e+02               # chi0 mass [GeV]
mass = StringToFloat(runArgs.jobConfig[0].split('_')[3])
DeltaM = 1.0                      # mass splitting between chi0 and chi+ [GeV] 
mass_plus_DeltaM = mass + DeltaM  # mass_plus_DeltaM : it is the chi+ and chi- mass; i.e. mass(chi+-) = mass(chi0) + DeltaM
print  "****** mass + DeltaM [GeV] = ", mass_plus_DeltaM


## note that you need to specify the QED and QCD couplings 
## in order to include also the QED diagrams 
## (for typical VBF processes)

fcard = open('proc_card_mg5.dat','w')
fcard.write("""
    import model MDM_Triplet_with_pion_UFO
    
    generate p p > chi+ chi- j j  QED=10 QCD=10, chi+ > pi+ n1, chi- > pi- n1
    add process p p > chi+ n1 j j QED=10 QCD=10, chi+ > pi+ n1
    add process p p > chi- n1 j j QED=10 QCD=10, chi- > pi- n1
    add process p p > n1 n1  j j  QED=10 QCD=10
    
    output -f""")
fcard.close()



beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No center of mass energy found.")




#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version' : '3.0',
    'cut_decays'  : 'F',
    'pdlabel'      :"'lhapdf'",
    'lhaid'        :230000,
    'ickkw'       : 0,
    'mmjj'        : Mjj,
    'ptj'         : ptj_min,
    'deltaeta'    : deltaeta,
    'bwcutoff'    : bwcutoff,
    'use_syst'    : 'False'}

process_dir = new_process()
build_run_card(run_card_old=get_default_runcard(proc_dir=process_dir),run_card_new='run_card.dat',xqcut=0,
               nevts=runArgs.maxEvents * 1.1,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)




## change chi0 mass 
## note that you need to set Wchi to auto 

if not os.access(process_dir+'/Cards/param_card.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old run card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open(process_dir+'/Cards/param_card.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' # MDM ' in line:
            newcard.write('      1  %i # MDM \n'%(mass))
        elif '# DeltaM' in line:
            newcard.write('      2 %i # DeltaM \n'%(DeltaM))
        elif '# n1 : MDM' in line:
            newcard.write('      1000022 %i  # n1 : MDM \n'%(mass))
        elif '# chi+ : DeltaM + MDM' in line:
            newcard.write('      9000006 %i # chi+ : DeltaM + MDM \n'%(mass_plus_DeltaM))
        elif ' # Wchi ' in line:
            newcard.write('DECAY 9000006 auto # Wchi \n')
        # set some sm parameters to atlas defaults
        elif ' # MB ' in line:
            newcard.write('    5 4.950000e+00 # MB \n')
        elif ' # MT ' in line:
            newcard.write('    6 1.725000e+02 # MT \n')
        elif ' # WT ' in line:
            newcard.write('DECAY   6 auto # WT \n')
        elif ' # WZ ' in line:
            newcard.write('DECAY  23 2.495200e+00 # WZ \n')
        elif ' # WW ' in line:
            newcard.write('DECAY  24 2.085000e+00 # WW \n')
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()



runName='run_01'


generate(run_card_loc='./run_card.dat',param_card_loc='./param_card.dat',mode=mode,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)






#### Shower
evgenConfig.description = 'Minimal Dark Matter triplet; wino-like DM'
evgenConfig.keywords+=['BSM','exotic', 'WIMP']#https://gitlab.cern.ch/atlas/athena/tree/21.6/Generators/MadGraphControl/share/common/evgenkeywords.txt
evgenConfig.generators = ["MadGraph", "Pythia8", "EvtGen"]
evgenConfig.inputfilecheck = runName
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
evgenConfig.contact = ["Marta Maria Perego <marta.maria.perego@cern.ch>"]


include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")
include("Pythia8_i/Pythia8_MadGraph.py")







