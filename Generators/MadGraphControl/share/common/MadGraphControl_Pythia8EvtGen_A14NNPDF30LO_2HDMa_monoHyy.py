from MadGraphControl.MadGraphUtils import *
import math

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model Pseudoscalar_2HDM -modelname

define p = g d u s c b d~ u~ s~ c~ b~
define j = g d u s c b d~ u~ s~ c~ b~
define dm = Xd Xd~
generate g g > xd xd~ h1 / z [QCD]
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
          'lhe_version': '3.0',
          'cut_decays': 'F',
          'pdlabel': "'lhapdf'",
          'lhaid': 263000,
          'maxjetflavor'  : 5,
          'asrwgtflavor'  : 5,
          'use_syst': 'False',
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

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz', lhe_version=3, saveProcDir=True)                                                                         
                                                                                                                                                                       
#### Shower                                                                                                                                                             
evgenConfig.description = "Pseudoscalar_2HDM monoHyy"
evgenConfig.keywords = ["exotic","BSM","Higgs","WIMP"]
evgenConfig.process = "g g > xd xd~ h1"
evgenConfig.inputfilecheck = runName                                                                                                                                   
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'                                                                                                             
evgenConfig.contact = ["Chen Zhou <chen.zhou@cern.ch>"]

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

#include("Pythia8_i/Pythia8_aMcAtNlo.py")
include("Pythia8_i/Pythia8_MadGraph.py")
#include("Pythia8_i/Pythia8_CKKWL_kTMerge.py") # we include a local copy instead, where a bug is fixed (will be unnecessary from MC15JobOptions-00-05-70)
#include("Pythia8_CKKWL_kTMerge.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["SLHA:allowUserOverride = on",
                            "1000022:all = xd xd~ 2 0 0 %d 0.0 0.0 0.0 0.0" % (int(THDMparams['MXd'])),
                            "1000022:isVisible = false"]

genSeq.Pythia8.Commands += ["25:oneChannel = on 1.0 100 22 22 "]

if not hasattr( filtSeq, "DiPhotonFilter" ):
    from GeneratorFilters.GeneratorFiltersConf import DiPhotonFilter
    filtSeq += DiPhotonFilter ()

DiPhotonFilter = filtSeq.DiPhotonFilter
DiPhotonFilter.PtCut1st = 30000.
DiPhotonFilter.PtCut2nd = 20000.
