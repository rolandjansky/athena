from MadGraphControl.MadGraphUtils import *
import math

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model Pseudoscalar_2HDM -modelname

define p = g d u s c b d~ u~ s~ c~ b~
define j = g d u s c b d~ u~ s~ c~ b~
define dm = Xd Xd~
""")
# Z's don't contribute because sinbma (sin(beta-alpha)) is 1.0, so excluding them with '/z' reduces runtime for the LHE generation, 
# without changing the physics/cross-sections/kinematic ditributions
if initialGluons:
    fcard.write("generate g g > xd xd~ h1 / z [QCD]\n") 
else:
    fcard.write("generate b b~ > xd xd~ h1 / z\n")
fcard.write("""
output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else: 
  raise RuntimeError("No center of mass energy found.")

# options
qcut = 0 # for consistency with iCKKW=0

extras = {
    'pdlabel'       : "'lhapdf'",
    'lhaid'         : '263000',
    'maxjetflavor'  : 5,
    'asrwgtflavor'  : 5,
    'lhe_version': '3.0',
    'cut_decays': 'F',
    'use_syst': 'False'
}

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat', xqcut = qcut,
               nevts=runArgs.maxEvents*2.,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

print_cards()

paramcard = subprocess.Popen(['get_files','-data','MadGraph_param_card_Pseudoscalar2HDM.dat'])
paramcard.wait()
if not os.access('MadGraph_param_card_Pseudoscalar2HDM.dat',os.R_OK):
    print 'ERROR: Could not get param card'
elif os.access('param_card.dat',os.R_OK):
    print 'ERROR: Old param card in the current directory.  Dont want to clobber it.  Please move it first.'
else:
    oldcard = open('MadGraph_param_card_Pseudoscalar2HDM.dat','r')
    newcard = open('param_card.dat','w')
    import re
    THDM_regexp = re.compile('\s+([0-9]+)\s+([0-9+-.e]+)\s+#\s+(\w+)\s*')
    for line in oldcard:
        isTHDMparam = False
        for param_name, newvalue in THDMparams.items():
          if param_name in line:
            THDM_match = THDM_regexp.match(line.rstrip('\n'))
            if THDM_match:
              THDM_pdgID = int(THDM_match.group(1))
              THDM_oldvalue = float(THDM_match.group(2))
              THDM_param_name = str(THDM_match.group(3))
              if THDM_param_name != param_name:
                print param_name, THDM_param_name
                raise RuntimeError('Mismatching parameter names, please double-check logic')
              newcard.write('     %d %s # %s\n' % (THDM_pdgID, str(newvalue), THDM_param_name))
              isTHDMparam = True
            else:
              print line.rstrip('\n')
              raise RuntimeError('Unable to parse line')
        if not isTHDMparam:
          newcard.write(line)
    oldcard.close()
    newcard.close()

runName='run_01'

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=0,njobs=1,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz',lhe_version=3,saveProcDir=True)                                                                         
                                                                                                                                                                       
#### Shower                                                                                                                                                             
if initialGluons:
    evgenConfig.process = "g g > xd xd~ h1"
    initialStateString = "gluon fusion"
else:
    evgenConfig.process = "b b~ > xd xd~ h1"
    initialStateString = "b quark annihilation"
evgenConfig.description = "Pseudoscalar Mediator simplified Model for mono-Higgs(-> b bbar) " +initialStateString + " initiated process  \
with tan(beta) = " + str(THDMparams['tanbeta']) + ", sin(theta) = " + str(THDMparams['sinp']) + ", mA = " + str(THDMparams['mh3']) + ", ma"\
 + str(THDMparams['mh4'])
evgenConfig.keywords = ["exotic","BSMHiggs","Higgs","WIMP", "simplifiedModel","bbbar"]
evgenConfig.inputfilecheck = runName                                                                                                                                   
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                             
evgenConfig.contact = ["Chen Zhou <chen.zhou@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

include("Pythia8_i/Pythia8_MadGraph.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["SLHA:allowUserOverride = on",
                            "1000022:all = xd xd~ 2 0 0 %d 0.0 0.0 0.0 0.0" % (int(THDMparams['MXd'])),
                            "1000022:isVisible = false"]


genSeq.Pythia8.Commands += ["25:oneChannel = on 1.0 100 5 -5 "]

include("GeneratorFilters/MissingEtFilter.py")
filtSeq.MissingEtFilter.METCut = 50*GeV
filtSeq.MissingEtFilter.UseNeutrinosFromHadrons = True
