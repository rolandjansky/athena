def get_mg_variations(m_stau, m_slep, syst_mod, xqcut=None):
  if xqcut is None:
    xqcut = 500  # default
    if m_stau < xqcut * 4.: xqcut = ((m_stau + m_slep) / 2.) * 0.25
    if syst_mod:
      if 'qcup' in syst_mod:
        xqcut = xqcut * 2.
      elif 'qcdw' in syst_mod:
        xqcut = xqcut * 0.5

  mglog.info('For matching, will use xqcut of ' + str(xqcut))

  alpsfact = 1.0
  scalefact = 1.0
  if syst_mod:
    if 'alup' in syst_mod:
      alpsfact = 2.0
    elif 'aldw' in syst_mod:
      alpsfact = 0.5

    if 'scup' in syst_mod:
      scalefact = 2.0
    elif 'scdw' in syst_mod:
      scalefact = 0.5

  return xqcut, alpsfact, scalefact

from MadGraphControl.MadGraphUtils import *

# keep the output directory for testing?!
keepOutput = False

# get some parameters from the jobOptions and config and build the SLHA name
import re
plist = re.search(r'.+GMSB_(.+)\.py', runArgs.jobConfig[0]).group(1).split('_')
lambdaVal = float(plist[2])
tanbVal = float(plist[4])
production = ""
match = re.search(r'(_dy|_sl|_ew)[_.]', runArgs.jobConfig[0])
if match:
  if match.group(1):
    production = match.group(1).replace("_", "")
lifetime = float(-1.) # -1 is for stable (lifetimes not yet implemented in simulation preInclude.GMSB.py)
slha_file = 'susy.%s.gmsb_stablestau_lambda_%03d_tanb_%02d.slha'  % (runArgs.runNumber, int(lambdaVal), int(tanbVal))

# get more events out of madgraph to feed through athena (esp. for filters)
evt_multiplier = 2.0
if evt_multiplier > 0:
  if runArgs.maxEvents > 0:
    nevts = runArgs.maxEvents * evt_multiplier
  else:
    nevts = 5000 * evt_multiplier

# MadGraph5 systematics variations
list_mg_syst = ['scup',
                'scdw',
                'alup',
                'aldw',
                'qcup',
                'qcdw']

# Pythia8 systematics variations
dict_py8_syst = {'py1up': 'Pythia8_A14_NNPDF23LO_Var1Up_EvtGen_Common.py',
                 'py1dw': 'Pythia8_A14_NNPDF23LO_Var1Down_EvtGen_Common.py',
                 'py2up': 'Pythia8_A14_NNPDF23LO_Var2Up_EvtGen_Common.py',
                 'py2dw': 'Pythia8_A14_NNPDF23LO_Var2Down_EvtGen_Common.py',
                 'py3aup': 'Pythia8_A14_NNPDF23LO_Var3aUp_EvtGen_Common.py',
                 'py3adw': 'Pythia8_A14_NNPDF23LO_Var3aDown_EvtGen_Common.py',
                 'py3bup': 'Pythia8_A14_NNPDF23LO_Var3bUp_EvtGen_Common.py',
                 'py3bdw': 'Pythia8_A14_NNPDF23LO_Var3bDown_EvtGen_Common.py',
                 'py3cup': 'Pythia8_A14_NNPDF23LO_Var3cUp_EvtGen_Common.py',
                 'py3cdw': 'Pythia8_A14_NNPDF23LO_Var3cDown_EvtGen_Common.py'}

mg_syst_mod = None
par = [x for x in plist if x in list_mg_syst]
if par:
    mg_syst_mod = par[0]

py8_syst_mod = None
par = [x for x in plist if x in dict_py8_syst.keys()]
if par:
    py8_syst_mod = par[0]

if production == 'dy':
  # stau1 DY (process numbers follow https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs)
  mglog.info('GMSBPROD - direct stau production')
  process = '''
  generate p p > ta1- ta1+    # 206 - ta1- ta1+
  add process p p > ta1- ta1+ j   # 206 - ta1- ta1+
  add process p p > ta1- ta1+ j j  # 206 - ta1- ta1+
  '''
elif production == 'sl':
  # direct slepton production (process numbers follow https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs)
  mglog.info('GMSBPROD - direct slepton production')
  process = '''
  generate p p > el- el+         # 201 - el- el+
  add process p p > er- er+      # 202 - er- er+
  add process p p > ta1- ta1+    # 206 - ta1- ta1+
  add process p p > ta2- ta2+    # 207 - ta2- ta2+
  add process p p > ta1- ta2+    # 208 - ta1- ta2+
  add process p p > ta2- ta1+    # 208 - ta1- ta2+
  add process p p > mul- mul+    # 216 - mul- mul+
  add process p p > mur- mur+    # 217 - mur- mur+
  add process p p > el- el+ j     # 201 - el- el+
  add process p p > er- er+ j     # 202 - er- er+
  add process p p > ta1- ta1+ j   # 206 - ta1- ta1+
  add process p p > ta2- ta2+ j   # 207 - ta2- ta2+
  add process p p > ta1- ta2+ j   # 208 - ta1- ta2+
  add process p p > ta2- ta1+ j   # 208 - ta1- ta2+
  add process p p > mul- mul+ j   # 216 - mul- mul+
  add process p p > mur- mur+ j   # 217 - mur- mur+
  add process p p > el- el+ j j    # 201 - el- el+
  add process p p > er- er+ j j    # 202 - er- er+
  add process p p > ta1- ta1+ j j  # 206 - ta1- ta1+
  add process p p > ta2- ta2+ j j  # 207 - ta2- ta2+
  add process p p > ta1- ta2+ j j  # 208 - ta1- ta2+
  add process p p > ta2- ta1+ j j  # 208 - ta1- ta2+
  add process p p > mul- mul+ j j  # 216 - mul- mul+
  add process p p > mur- mur+ j j  # 217 - mur- mur+
  '''
elif production == 'ew':
  # EW production (process numbers follow https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs)
  mglog.info('GMSBPROD - electroweak production')
  process = '''
  generate p p > n1 n1      # 111 - n1 n1
  add process p p > n1 n2   # 112 - n1 n2
  add process p p > n1 n3   # 113 - n1 n3
  add process p p > n1 n4   # 114 - n1 n4
  add process p p > n1 x1+  # 115 - n1 x1+
  add process p p > n1 x2+  # 116 - n1 x2+
  add process p p > n1 x1-  # 117 - n1 x1-
  add process p p > n1 x2-  # 118 - n1 x2-
  add process p p > n2 n2   # 122 - n2 n2
  add process p p > n2 n3   # 123 - n2 n3
  add process p p > n2 n4   # 124 - n2 n4
  add process p p > n2 x1+  # 125 - n2 x1+
  add process p p > n2 x2+  # 126 - n2 x2+
  add process p p > n2 x1-  # 127 - n2 x1-
  add process p p > n2 x2-  # 128 - n2 x2-
  add process p p > n3 n3   # 133 - n3 n3
  add process p p > n3 n4   # 134 - n3 n4
  add process p p > n3 x1+  # 135 - n3 x1+
  add process p p > n3 x2+  # 136 - n3 x2+
  add process p p > n3 x1-  # 137 - n3 x1-
  add process p p > n3 x2-  # 138 - n3 x2-
  add process p p > n4 n4   # 144 - n4 n4
  add process p p > n4 x1+  # 145 - n4 x1+
  add process p p > n4 x2+  # 146 - n4 x2+
  add process p p > n4 x1-  # 147 - n4 x1-
  add process p p > n4 x2-  # 148 - n4 x2-
  add process p p > x1+ x1- # 157 - x1+ x1-
  add process p p > x1+ x2- # 158 - x1+ x2-
  add process p p > x2+ x1- # 167 - x2+ x1-
  add process p p > x2+ x2- # 168 - x2+ x2-
  add process p p > n1 n1   j # 111 - n1 n1
  add process p p > n1 n2   j # 112 - n1 n2
  add process p p > n1 n3   j # 113 - n1 n3
  add process p p > n1 n4   j # 114 - n1 n4
  add process p p > n1 x1+  j # 115 - n1 x1+
  add process p p > n1 x2+  j # 116 - n1 x2+
  add process p p > n1 x1-  j # 117 - n1 x1-
  add process p p > n1 x2-  j # 118 - n1 x2-
  add process p p > n2 n2   j # 122 - n2 n2
  add process p p > n2 n3   j # 123 - n2 n3
  add process p p > n2 n4   j # 124 - n2 n4
  add process p p > n2 x1+  j # 125 - n2 x1+
  add process p p > n2 x2+  j # 126 - n2 x2+
  add process p p > n2 x1-  j # 127 - n2 x1-
  add process p p > n2 x2-  j # 128 - n2 x2-
  add process p p > n3 n3   j # 133 - n3 n3
  add process p p > n3 n4   j # 134 - n3 n4
  add process p p > n3 x1+  j # 135 - n3 x1+
  add process p p > n3 x2+  j # 136 - n3 x2+
  add process p p > n3 x1-  j # 137 - n3 x1-
  add process p p > n3 x2-  j # 138 - n3 x2-
  add process p p > n4 n4   j # 144 - n4 n4
  add process p p > n4 x1+  j # 145 - n4 x1+
  add process p p > n4 x2+  j # 146 - n4 x2+
  add process p p > n4 x1-  j # 147 - n4 x1-
  add process p p > n4 x2-  j # 148 - n4 x2-
  add process p p > x1+ x1- j # 157 - x1+ x1-
  add process p p > x1+ x2- j # 158 - x1+ x2-
  add process p p > x2+ x1- j # 167 - x2+ x1-
  add process p p > x2+ x2- j # 168 - x2+ x2-
  add process p p > n1 n1   j j # 111 - n1 n1
  add process p p > n1 n2   j j # 112 - n1 n2
  add process p p > n1 n3   j j # 113 - n1 n3
  add process p p > n1 n4   j j # 114 - n1 n4
  add process p p > n1 x1+  j j # 115 - n1 x1+
  add process p p > n1 x2+  j j # 116 - n1 x2+
  add process p p > n1 x1-  j j # 117 - n1 x1-
  add process p p > n1 x2-  j j # 118 - n1 x2-
  add process p p > n2 n2   j j # 122 - n2 n2
  add process p p > n2 n3   j j # 123 - n2 n3
  add process p p > n2 n4   j j # 124 - n2 n4
  add process p p > n2 x1+  j j # 125 - n2 x1+
  add process p p > n2 x2+  j j # 126 - n2 x2+
  add process p p > n2 x1-  j j # 127 - n2 x1-
  add process p p > n2 x2-  j j # 128 - n2 x2-
  add process p p > n3 n3   j j # 133 - n3 n3
  add process p p > n3 n4   j j # 134 - n3 n4
  add process p p > n3 x1+  j j # 135 - n3 x1+
  add process p p > n3 x2+  j j # 136 - n3 x2+
  add process p p > n3 x1-  j j # 137 - n3 x1-
  add process p p > n3 x2-  j j # 138 - n3 x2-
  add process p p > n4 n4   j j # 144 - n4 n4
  add process p p > n4 x1+  j j # 145 - n4 x1+
  add process p p > n4 x2+  j j # 146 - n4 x2+
  add process p p > n4 x1-  j j # 147 - n4 x1-
  add process p p > n4 x2-  j j # 148 - n4 x2-
  add process p p > x1+ x1- j j # 157 - x1+ x1-
  add process p p > x1+ x2- j j # 158 - x1+ x2-
  add process p p > x2+ x1- j j # 167 - x2+ x1-
  add process p p > x2+ x2- j j # 168 - x2+ x2-
  '''
else:
  # GMSB production (process numbers follow https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/SUSYSignalUncertainties#Subprocess_IDs)
  mglog.info('GMSBPROD - full GMSB production')
  process = '''
  define susyall = susystrong susyweak
  generate p p > susyall susyall @1
  add process p p > susyall susyall j @2
  add process p p > susyall susyall j j @3
  '''

# set number of jets done in madgraph
njets = 2

# choose pdf set
pdlabel = 'nn23lo1'
lhaid = 247000

# set beam energy (overwrite by job settings)
beamEnergy = 6500.
if hasattr(runArgs, 'ecmEnergy'):
  beamEnergy = runArgs.ecmEnergy * 0.5

# set random seed (overwrite by job settings)
rand_seed = 1234
if hasattr(runArgs, "randomSeed"):
  rand_seed = runArgs.randomSeed
  # rand_seed = 1000 * int(str(runArgs.runNumber)[1:6]) + runArgs.randomSeed

if not os.environ.has_key('MADGRAPH_DATA'):
  os.environ['MADGRAPH_DATA'] = os.getcwd()
  mglog.warning('Setting your MADGRAPH_DATA environmental variable to the working directory')

if py8_syst_mod:
## ??? needs to be modified !!! ###
  include("Pythia8_i/"+dict_py8_syst[py8_syst_mod])
else:
  include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

include("Pythia8_i/Pythia8_MadGraph.py")

# generate the new process
full_proc = """
import model mssm-full
""" + helpful_definitions() + """
# Specify process(es) to run

""" + process + """
# Output processes to MadEvent directory
output -f
"""

thedir = new_process(card_loc = full_proc)
if 1 == thedir:
  mglog.error('Error in process generation!')
mglog.info('Using process directory ' + thedir)

# grab param_card and move new masses into place
from PyJobTransformsCore.trfutil import get_files
get_files(slha_file, keepDir=False, errorIfNotFound=True)
include('MadGraphControl/SUSYMetadata.py')
(m_stau, m_slepton) = mass_extract(slha_file, ['1000015', '2000011'])
mglog.info('stau mass = ' + m_stau + ' slepton mass = ' + m_slepton)

if lifetime != 0:
  # remove stau1 decay from slha file
  remove_decay(slha_file, '1000015')

build_param_card(param_card_old = slha_file, param_card_new = 'param_card.dat')

xqcut = None
xqcut, alpsfact, scalefact = get_mg_variations(abs(float(m_stau)), abs(float(m_slepton)), mg_syst_mod, xqcut)
mglog.info("MG5 params: %s %s %s"%(xqcut, alpsfact, scalefact))

# grab run_card and move into place
extras = {'ktdurham': xqcut,
          'lhe_version': '2.0',
          'cut_decays': 'F',
          'pdlabel': pdlabel,
          'lhaid': lhaid,
          'drjj': 0.0,
          'ickkw': 0
          }
build_run_card(run_card_old = get_default_runcard(proc_dir = thedir),
               run_card_new = 'run_card.dat',
               xqcut = xqcut,
               nevts = nevts,
               rand_seed = rand_seed,
               beamEnergy = beamEnergy,
               scalefact = scalefact,
               alpsfact = alpsfact,
               extras = extras)

if generate(run_card_loc = 'run_card.dat', param_card_loc = 'param_card.dat', mode = 0, njobs = 1, run_name = 'Test', proc_dir = thedir):
  mglog.error('Error generating events!')

# move output files to appropriate place, with appropriate name
the_spot = arrange_output(run_name = 'Test', proc_dir = thedir, outputDS = 'madgraph_OTF._00001.events.tar.gz')
if the_spot == '':
  mglog.error('Error arranging output dataset!')

mglog.info('Removing process directory...')
shutil.rmtree(thedir, ignore_errors=True)

mglog.info('All done generating events!!')

outputDS = the_spot

if xqcut < 0 or outputDS is None or '' == outputDS:
  evgenLog.warning('Looks like something went wrong with the MadGraph generation - bailing out!')
  raise RuntimeError('Error in MadGraph generation')

import os
if 'ATHENA_PROC_NUMBER' in os.environ:
  evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
  njobs = os.environ.pop('ATHENA_PROC_NUMBER')
  # Try to modify the opts underfoot
  if not hasattr(opts, 'nprocs'):
    mglog.warning('Did not see option!')
  else:
    opts.nprocs = 0
  mglog.info(opts)

runArgs.qcut = xqcut
runArgs.inputGeneratorFile = outputDS
# runArgs.gentype = production
if mg_syst_mod:
  runArgs.syst_mod = mg_syst_mod
elif py8_syst_mod:
  runArgs.syst_mod = py8_syst_mod

# Pythia8 setup
genSeq.Pythia8.Commands += ["Init:showAllParticleData = on",
                            "Next:numberShowLHA = 10",
                            "Next:numberShowEvent = 10",
                            "1000015:mayDecay = false"
                           ]

if njets > 0:
  genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on", # PMG HAVE A LOOK
                              "Merging:nJetMax = " + str(njets),
                              "Merging:doKTMerging = on",
                              "Merging:TMS = " + str(xqcut),
                              "Merging:ktType = 1",
                              "Merging:Dparameter = 0.4",
                              "1000024:spinType = 1",
                              "1000022:spinType = 1"
                             ]
  if production == 'dy':
    genSeq.Pythia8.Commands += [
                                "Merging:Process = pp>{ta1+,1000015}{ta1-,-1000015}"
                               ]
  elif production == 'sl':
    genSeq.Pythia8.Commands += [
                                "Merging:Process = pp>{ta1+,1000015}{ta1-,-1000015}",
                                "Merging:Process = pp>{ta2+,2000015}{ta2-,-2000015}",
                                "Merging:Process = pp>{el+,1000011}{el-,-1000011}",
                                "Merging:Process = pp>{er+,2000011}{er-,-2000011}",
                                "Merging:Process = pp>{mul+,1000013}{mul-,-1000013}",
                                "Merging:Process = pp>{mur+,2000013}{mur-,-2000013}"
                               ]
  elif production == 'ew':
    pass
  else:
    pass

# configuration for EvgenJobTransforms
evgenLog.info('Registered generation of GMSB stable stau')
evgenConfig.contact  = [ "sascha.mehlhase@cern.ch" ]
evgenConfig.keywords += ['SUSY','GMSB']
evgenConfig.description = 'GMSB stable stau'
evgenConfig.generators += ["EvtGen"]
if lifetime != 0:
  evgenConfig.specialConfig = 'GMSBIndex=2;GMSBStau=%s*GeV;GMSBSlepton=%s*GeV;preInclude=SimulationJobOptions/preInclude.GMSB.py' % (m_stau, m_slepton)

if not hasattr(runArgs, 'inputGeneratorFile'):
  print 'ERROR: something wasnt write in file name transfer from the fragment.'
  runArgs.inputGeneratorFile = 'madgraph.*._events.tar.gz'
evgenConfig.inputfilecheck = runArgs.inputGeneratorFile.split('._0')[0]

# testSeq.TestHepMC.MaxVtxDisp = 1e16 # in mm
# testSeq.TestHepMC.MaxTransVtxDisp = 1e16 # in mm
testSeq.TestHepMC.MaxNonG4Energy=14000000.

# clean up
del keepOutput, plist, lambdaVal, tanbVal, production, match, lifetime, slha_file, evt_multiplier, nevts, list_mg_syst, dict_py8_syst, mg_syst_mod, par, py8_syst_mod, njets, pdlabel, lhaid, beamEnergy, rand_seed, full_proc, thedir, m_stau, m_slepton, xqcut, alpsfact, scalefact, extras, the_spot, outputDS
