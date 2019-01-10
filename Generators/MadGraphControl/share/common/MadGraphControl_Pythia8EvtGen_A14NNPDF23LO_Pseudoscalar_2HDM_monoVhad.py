from MadGraphControl.MadGraphUtils import *
import math

## safe factor applied to nevents, to account for the filter efficiency
#safefactor=1.1

fcard = open('proc_card_mg5.dat', 'w')
fcard.write("""
import model Pseudoscalar_2HDM -modelname

define p = g d u s c b d~ u~ s~ c~ b~
define j = g d u s c b d~ u~ s~ c~ b~
define dm = Xd Xd~
generate g g > dm dm z [QCD]
output -f
""")
fcard.close()

beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy / 2.
else: 
  raise RuntimeError("No center of mass energy found.")

extras = {
    'pdlabel'       : "'lhapdf'",
    'lhaid'         : '263000',
    'lhe_version': '3.0',
    'maxjetflavor'  : 5,
    'asrwgtflavor'  : 5,
    'cut_decays': 'F',
    'use_syst': 'T',
    'sys_alpsfact': '0.5 1 2',
    'sys_pdf':"NNPDF30_lo_as_0130.LHgrid"
}

process_dir = new_process()

build_run_card(run_card_old=get_default_runcard(process_dir),run_card_new='run_card.dat',
               nevts=int(runArgs.maxEvents*safefactor),rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy, extras=extras)

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

### creating mad spin card

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
set spinmode none
define j = g u c d s u~ c~ d~ s~
define jb = j b b~
decay z > jb jb
# running the actual code
launch"""%runArgs.randomSeed)  

mscard.close()

generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',madspin_card_loc=madspin_card_loc,mode=2,njobs=8,run_name=runName,proc_dir=process_dir)

arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz', saveProcDir=True)                                                                         
import os
if 'ATHENA_PROC_NUMBER' in os.environ:
    evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
    njobs = os.environ.pop('ATHENA_PROC_NUMBER')
    # Try to modify the opts underfoot
    if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
    else: opts.nprocs = 0
    print opts                                                                                                                                                                       
#### Shower                                                                                                                                                             
evgenConfig.description = "Test with Pseudoscalar_2HDM, DO NOT USE FOR PHYSICS"
evgenConfig.keywords = ["exotic","BSM","WIMP"]
evgenConfig.process = "g g > xd xd~ z"
evgenConfig.contact = ["Lailin Xu <lailin.xu@cern.ch>"]
evgenConfig.inputfilecheck = runName

runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'
                                                                                                             

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")


include("Pythia8_i/Pythia8_MadGraph.py")

#particle data = name antiname spin=2s+1 3xcharge colour mass width (left out, so set to 0: mMin mMax tau0)
genSeq.Pythia8.Commands += ["1000022:all = xd xd~ 2 0 0 %d 0.0 0.0 0.0 0.0" % (int(THDMparams['MXd'])),
                            "1000022:isVisible = false"]

