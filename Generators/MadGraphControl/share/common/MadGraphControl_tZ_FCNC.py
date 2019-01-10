import logging
import os
import re
import re
import subprocess
import sys


logger = logging.getLogger('MadGraphControl_tZ_FCNC')


def configure_tZ_FCNC(configName, tags, couplings, showerGenerator=None, tuneName=None, pdfName=None, tuneSuffix=None):
  config = GeneratorConfig()

  # physics constants
  # (must be copied from the param_card!)
  param = dict(
      MZ=91.1876,
      MT=172.5,
    )

  # default arguments
  if showerGenerator is None:
     showerGenerator = 'Pythia8'
  if tuneName is None:
     tuneName = 'A14'
  if pdfName is None:
     pdfName = 'NNPDF23LO'

  # MadGraph/MadSpin particle definitions
  config.particleDefs = [
      'l+ = e+ mu+ ta+',
      'l- = e- mu- ta-',
      'nu = ve vm vt ve~ vm~ vt~',
      'wpm = w+ w-',
      'bpm = b b~',
      'tpm = t t~',
      'k = l- l+ nu',
      'f = k u c d s u~ c~ d~ s~',
    ]

  # helper for testing tags
  class TagSet(object):
    def __init__(self, tags):
      self._tags = frozenset(tags.split())
    def __getattr__(self, name):
      return name in self._tags
  tags = TagSet(tags)
  k = tags

  # configure generator and processes ...
  modelNameShort = None
  procName = None
  procDesc = None
  keywords = []

  if k.LO:
    config.generator = 'MadGraph'

  if k.NLO:
    config.generator = 'aMcAtNlo'

  if k.tZ:
    keywords += 'singleTop', 'top'

  if k.tZ and k.LO:
    config.processes = ['p p > tpm Z QCD<=1']

  if k.tZ and k.NLO:
    config.processes = ['p p > %s Z $$ %s [QCD]' % tt for tt in (('t', 't~'), ('t~', 't'))]

  if k.tZ and k.trilep:
    config.madSpinDecays = [
        't > w+ b, w+ > l+ nu',
        't~ > w- b~, w- > l- nu',
        'z > l+ l-',
      ]
    procName = 'tZ_3l'
    procDesc = 'pp>(t>b(W>lv))(Z>ll)'
    keywords += 'tZ', '3lepton'

  if k.Artur:
    config.modelName = 'topFCNC_UFO'
    modelNameShort = 'topFCNC'
    config.paramCardTemplate = 'MadGraph_param_card_topFCNC_UFO.dat'

  if k.Cecile:
    config.modelName = 'TopFCNC-onlyGam'
    modelNameShort = 'TopFCNC'
    config.paramCardTemplate = 'MadGraph_param_card_ttFCNC_NLO.dat'

  # set model parameters
  if k.Artur:
    allCouplings = {c: 0. for c in 'ReXLut ImXLut ReXRut ImXRut ReXLct ImXLct ReXRct ImXRct ReKLut ImKLut ReKRut ImKRut ReKLct ImKLct ReKRct ImKRct ReZetaLut ImZetaLut ReZetaRut ImZetaRut ReZetaLct ImZetaLct ReZetaRct ImZetaRct ReEtaLut ImEtaLut ReEtaRut ImEtaRut ReEtaLct ImEtaLct ReEtaRct ImEtaRct ReLambdaLut ImLambdaLut ReLambdaRut ImLambdaRut ReLambdaLct ImLambdaLct ReLambdaRct ImLambdaRct'.split()}
    config.paramExtras['NEWCOUP'] = allCouplings
  if k.Cecile:
    allCouplings = {c: (0., 1.e-99)[k.NLO] for c in 'RCtphi ICtphi RCtG ICtG RCtW ICtW RCtB ICtB RCuphi ICuphi RCuG ICuG RCuW ICuW RCuB ICuB RC1utR IC1utR RC1utL IC1utL RC3utL IC3utL RCtcphi ICtcphi RCtcG ICtcG RCtcW ICtcW RCtcB ICtcB RCctphi ICctphi RCctG ICctG RCctW ICctW RCctB ICctB RC1ctR IC1ctR RC1ctL IC1ctL RC3ctL IC3ctL'.split()}
    config.paramExtras['DIM6'] = allCouplings
  for c, v in couplings.iteritems():
    allCouplings[c] # ensure valid coupling name
    allCouplings[c] = v

  # set shower tune and PDFs
  config.pdfName = pdfName
  config.showerGenerator = showerGenerator
  config.showerTune = '_'.join([tuneName, pdfName] + ([tuneSuffix] if tuneSuffix else []))

  # set theory scale
  mu = param['MT'] + param['MZ']
  for x in 'fixed_ren_scale fixed_fac_scale'.split():
    config.runExtras[x] = 'T'
  for x in 'muR_ref_fixed muF_ref_fixed muF1_ref_fixed muF2_ref_fixed QES_ref_fixed scale dsqrt_q2fact1 dsqrt_q2fact2'.split():
    config.runExtras[x] = repr(mu)

  # build dataset metadata
  config.name = '%s%sEvtGen_%s_%s_%s_%s_%s' % (config.generator, config.showerGenerator, pdfName, tuneName, modelNameShort, configName, procName)
  couplingsDesc = ''.join([', %s=%s' % (k, repr(couplings[k])) for k in sorted(couplings)])
  config.description = "Production of single top-quarks via FCNC (%s) with %s model%s" % (procDesc, config.modelName, couplingsDesc)
  if couplings:
    keywords += 'BSM', 'BSMtop', 'FCNC'
  config.contact = ['sebastian.mergelmeyer@cern.ch']
  config.keywords = sorted(set(keywords))

  return config


class GeneratorConfig(object):
  __slots__ = '''
         contact
         description
         generator
         keywords
         modelName
         name
         paramCardTemplate
         paramExtras
         particleDefs
         pdfName
         processes
         madSpinDecays
         showerGenerator
         showerTune
         runExtras
      '''.split()
  def __init__(self):
    self.paramExtras = {}
    self.runExtras = {}


def readRunCard(path):
  """Return the set of parameters found in the run card."""
  known_settings = set()
  with open(path, 'rb') as fin:
    for line in fin:
      rline = line[::-1]
      reol, rtext = re.match(r'((?:.*[!#]|)\s*)(.*)$', rline, re.S).groups()
      if rtext:
        rkey, = re.match(r'(\w+)\s*=', rtext).groups()
        key = rkey[::-1]
        known_settings.add(key)
  return known_settings


def resolveModelPath(modelName):
  return modelName


def runGenerator(config, globals=globals()):
  import MadGraphControl.MadGraphUtils as mg
  runArgs, evgenConfig, include = globals['runArgs'], globals['evgenConfig'], globals['include']
  c = config

  # generate 10% more events than requested because some may not pass phase-space cuts etc.
  nevts = int(1.10 * ( runArgs.maxEvents if runArgs.maxEvents > 0 else 5000 ))

  # common particle definitions for MadGraph and MadSpin
  particleDefs = ''.join(( 'define %s\n' % x for x in c.particleDefs ))

  # prepare process card
  procCardPath = 'proc_card_mg5.dat'
  with open(procCardPath, 'wb') as f:
    logger.info('random seed for MadEvent = %d', runArgs.randomSeed)
    if c.modelName:
      f.write('import model %s\n\n' % resolveModelPath(c.modelName))
    f.write(particleDefs)
    cmd = '\ngenerate'
    for process in c.processes:
      f.write('%s %s\n' % (cmd, process))
      cmd = 'add process'
    f.write('\noutput -f\n')

  # prepare MadSpin card
  madSpinCardPath = None
  if getattr(c, 'madSpinDecays', None):
    madSpinCardPath = 'madspin_card.dat'
    seed = int(runArgs.randomSeed) + 100000
    logger.info('random seed for MadSpin = %d', seed)
    with open(madSpinCardPath, 'wb') as f:
      f.write('set Nevents_for_max_weigth %d # number of events for the estimate of the max. weight\n' % 250)
      f.write('set max_weight_ps_point %d # number of PS to estimate the maximum for each event\n' % 1000)
      f.write('set seed %d\n' % seed)
      f.write(particleDefs)
      f.write('\n')
      for decay in c.madSpinDecays:
        f.write('decay %s\n' % decay)
      f.write('\nlaunch\n')

  # create process directory
  isNLO = {'MadGraph': False, 'aMcAtNlo': True}[c.generator]
  procDir = mg.new_process(card_loc=procCardPath)
  if not procDir:
    raise RuntimeError("MadGraphControl.MadGraphUtils.new_process returned failure")
  logger.debug("created process directory at: %s", repr(procDir))

  # settings for run card
  runCardTemplate = mg.get_default_runcard(proc_dir=procDir)
  knownSettings = readRunCard(runCardTemplate)
  lhaids = {
      'NNPDF23LO': (247000, 247001, 247101),
    }
  basicSettings = dict(
      lhe_version='3.0',
      pdlabel=repr('lhapdf'),
      lhaid=repr(lhaids[c.pdfName][0]),
    )
  if isNLO:
    basicSettings.update(
        parton_shower={
            'Pythia8': 'PYTHIA8',
          }[c.showerGenerator],
      )
  settings = {}
  settings.update(basicSettings)
  for k, v in config.runExtras.iteritems():
    if k in basicSettings:
      raise RuntimeError("run-card parameter %s must not be given explicitly" % repr(k))
    if k in knownSettings:
      settings[k] = v

  # make it so!
  if not mg.build_run_card(run_card_old=runCardTemplate, run_card_new='run_card.dat', nevts=nevts, rand_seed=runArgs.randomSeed, beamEnergy=runArgs.ecmEnergy/2., extras=settings) == 'run_card.dat':
    raise RuntimeError("MadGraphControl.MadGraphUtils.build_run_card failed")
  if not os.path.exists(config.paramCardTemplate):
    subprocess.check_call(['get_files', '-data', config.paramCardTemplate])
  if not mg.build_param_card(param_card_old=config.paramCardTemplate, param_card_new='param_card.dat', params=c.paramExtras) == 'param_card.dat':
    raise RuntimeError("MadGraphControl.MadGraphUtils.build_param_card failed")
  mg.generate(run_card_loc='run_card.dat', param_card_loc='param_card.dat', madspin_card_loc=madSpinCardPath, run_name='run', proc_dir=procDir)
  lhePrefix = 'MadGraph'
  lhePath = mg.arrange_output(run_name='run', lhe_version=int(settings['lhe_version'].split('.', 1)[0]), proc_dir=procDir, outputDS='%s._.tar.gz' % lhePrefix)
  runArgs.inputGeneratorFile = lhePath

  # make a copy of the LHE file
  m = re.match(r'(.*)\.tar\.gz$', lhePath)
  os.link('%s.events' % m.group(1), 'events.lhe')
  subprocess.check_call(['tar', '-czf', 'events.lhe.tgz', 'events.lhe'])
  os.unlink('events.lhe')

  # save metadata
  evgenConfig.contact = c.contact
  evgenConfig.description = c.description
  evgenConfig.generators = [c.generator, c.showerGenerator, 'EvtGen']
  evgenConfig.inputfilecheck = lhePrefix
  evgenConfig.keywords += c.keywords
  evgenConfig.process = c.name

  # run shower generator
  # (with multi-core fully disabled)
  # location corrected for 21.6 - needs testing !
  os.environ.pop('ATHENA_PROC_NUMBER', None)
  include('%s_i/%s_%s_EvtGen_Common.py' % (config.showerGenerator, config.showerGenerator, config.showerTune))
  include('%s_i/%s_%s.py' % (config.showerGenerator, config.showerGenerator, config.generator))
