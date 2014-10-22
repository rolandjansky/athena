#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = """Command line interface to run egammaMVACalib or egammaMVACalibMulti
 (see test/testMVACalibMulti.sh)"""
__author__ = "Bruno Lenzi <Bruno.Lenzi@cern.ch>"

import os

def doInteract(**kw):
  "Start the interactive mode, adding the given variables to globals"
  import user
  os.environ["PYTHONINSPECT"] = "1"
  globals().update(kw)
  print "\nEntering interactive mode, MVACalib object is called 'm'\n"

def loadLibs():
  import ROOT
  tmp = ROOT.gROOT.ProcessLine('gSystem->Load("libCint")')
  tmp = ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C");

def make_list(x, N=1):
  "Return an iterable object, i.e. the object itself or a list with N (=1) elements"
  if hasattr(x, '__iter__'):
    return x
  else:
    return [x]*N

def setDefinitions(m, defs):
  """setDefinitions(m, defs) -> apply definitions of variables to m 
  (e.g.: el_rawcl_Es1=1.01*el_rawcl_Es1,...)"""
  if defs:
    for definition in defs.split(','):
      m.setDefinition(*definition.split('='))


def create_TChain(inputfiles, treename):
  import ROOT
  chain = ROOT.TChain(treename)
  for inputfile in inputfiles:
    chain.Add(inputfile)
  return chain

def create_file_list(list_filename, dataset_name):
  import ROOT
  f = ROOT.TFile(list_filename)
  file_collection = f.Get(dataset_name)
  urls = map(lambda x: x.GetFirstUrl().GetUrl(), file_collection.GetList())
  return (file_collection.GetDefaultTreeName(), urls)

def getTChain(inputfiles, treename, readFromFile=False, dataset=False):
  """getTChain(inputfiles, treename="", readFromFile=False, dataset=False) -->
    Return a chain from a list of inputfiles (ROOT or text files if readFromFile=True)
    or a dataset (TFileCollection). Raise ValueError if the chain is invalid/empty"""
  chain = None
  if dataset:
    if len(inputfiles) != 1:
      raise ValueError("When using -d, --dataset you must specify only the ROOT file containing the TFileCollection")
    tree_name, file_list = create_file_list(inputfiles[0], dataset)
    chain = create_TChain(file_list, tree_name)
  else:
    if readFromFile:
      # filter is to remove empty strings
      file_list = filter(bool, (i.strip('\n') for f in inputfiles for i in open(f)))
    else:
      file_list = inputfiles
    chain = create_TChain(file_list, treename)

  if not chain or not chain.GetEntries():
    raise ValueError("Input files not given or not valid")
  return chain


def run_egammaMVACalib(outputfile, inputTree, inputPath, useTMVA, particleType,
  method, calibrationType, nEvents=-1, 
  debug=False, printBranches=False, branchName="", copyBranches="input", shift=0,
  fudge=None, etaBinDef='', energyBinDef='', particleTypeVar= '',
  filePattern="", ignoreSpectators = True,
  interact=False, first_event=0, friend=False, defs=None):
  """run_egammaMVACalib(outputfile, inputTree, inputPath, method, 
  particleType, calibrationType, nEvents=-1, 
  debug=False, printBranches=False, branchName="", copyBranches="input", shift=0,
  fudge=None, etaBinDef='', energyBinDef='', particleTypeVar='',
  filePattern="", ignoreSpectators = True,
  interact=False, first_event=0, friend=False, defs=None)"""

  iter_options = inputPath, useTMVA, method, calibrationType, shift, branchName, defs
  if any(isinstance(i, (list,tuple)) and len(i) > 1 for i in iter_options):
    inputPath, useTMVA, method, calibrationType, shift, branchName, defs = iter_options
    return run_egammaMVACalibMulti(outputfile, inputTree, inputPath, useTMVA, method, 
      particleType, calibrationType, nEvents, 
      branchName, copyBranches, shift, fudge,
      debug, etaBinDef, energyBinDef, particleTypeVar, filePattern, 
      ignoreSpectators, first_event, friend, defs) # FIXME
  else: # Make sure we do not have lists
    inputPath, useTMVA, method, calibrationType, shift, branchName, defs = \
      (i[0] if isinstance(i, (list,tuple)) else i for i in iter_options)
  
  import ROOT
#   loadLibs()

  
  params = particleType, not useTMVA, inputPath, method, calibrationType, debug, \
    etaBinDef, energyBinDef, particleTypeVar, filePattern, ignoreSpectators
  if fudge is None:
    m = ROOT.egammaMVACalib(*params)
  else:
    m = ROOT.egammaMVACalibFudge(fudge, *params)
  if shift is not None and shift >= 0:
    m.setPeakCorrection(shift)
  setDefinitions(m, defs)
  
  if printBranches:
    m.InitTree(inputTree)
    for i in m.getListOfBranches(): print i.GetName()
    return
  elif interact:
    doInteract(**locals())
    return
    
  fout = ROOT.TFile(outputfile, 'update')
  mvaTree = m.getMVAResponseTree(inputTree, nEvents, branchName, copyBranches, first_event)
  mvaTree.Print()
  if friend:
	  mvaTree.AddFriend(inputTree)
  mvaTree.Write('', ROOT.TObject.kOverwrite)
  fout.Close()

def run_egammaMVACalibMulti(outputfile, inputTree, inputPath, useTMVA, method, 
      particleType, calibrationType, nEvents, branchName,
      copyBranches='input', shift=0, fudge=False,
      debug=False, etaBinDef = '', energyBinDef = '', particleTypeVar='',
      filePattern='', ignoreSpectators=True, interact=False, friend = False, defs=None):
  """run_egammaMVACalibMulti(outputfile, inputTree, inputPath, useTMVA, method, 
      particleType, calibrationType, nEvents, branchName,
      copyBranches='input', shift=0, fudge=False,
      debug=False, etaBinDef = '', energyBinDef = '', particleTypeVar='',
      filePattern='', ignoreSpectators=True, interact=False, friend = False, defs=None)"""
  
  print "Running egammaMVACalibMulti"
  # Convert all the inputs to lists and make sure all the lists have the same size
  # (lists that have size 1 are multiplied by N, the maximum size)
  iter_options = map(make_list, (inputPath, useTMVA, method, calibrationType, shift, defs))
  N = max(len(i) for i in iter_options)
  if any(len(i) not in (1, N) for i in iter_options):
    raise ValueError("Invalid input: %s" % iter_options)
  
  iter_options = (make_list(i[0], N) if len(i) == 1 else i for i in iter_options)
  loop = zip(*iter_options)
  #  print loop
  if len(loop) != len(branchName):
    raise ValueError("Multiple options passed, branchName must have %s values, got %s" % \
      (len(loop), len(branchName)) )
  if len(set(branchName)) != len(branchName):
    raise ValueError("Repeated values for branchName: %s" % branchName)
  
  import ROOT
#   loadLibs()
  M = ROOT.egammaMVACalibMulti(particleType, inputTree, copyBranches)
  for i,j in enumerate(loop):
    inputPath, useTMVA, method, calibrationType, shift, defs = j
    if fudge is None:
      print ROOT.gSystem.WorkingDirectory()
    params = particleType, not useTMVA, inputPath, method, calibrationType, debug, \
      etaBinDef, energyBinDef, particleTypeVar, filePattern, ignoreSpectators
    if fudge is None:
      m = ROOT.egammaMVACalib(*params)
    else:
      m = ROOT.egammaMVACalibFudge(fudge, *params)
    setDefinitions(m, defs)
    M.Add(m, branchName[i])
    if shift is not None and shift >= 0:
      M.GetInstance().setPeakCorrection(shift)
  
  if interact:
    doInteract(**locals())  
    return
  fout = ROOT.TFile(outputfile, 'update')
  mvaTree = M.Run(nEvents)
  mvaTree.Print()
  if friend:
	  mvaTree.AddFriend(inputTree)
  mvaTree.Write('', ROOT.TObject.kOverwrite)
  fout.Close()

def getParser(doc):
  "getParser(doc) --> Return a parser to run egammaMVACalib"
  from optparse import OptionParser, OptionGroup
  parser = OptionParser("%prog [options] <outputfile> <inputfiles>")
  parser.description = doc
  parser.epilog = "\n"
  
  parser.add_option("-N", "--nEvents", help="Events to run (all by default)", default=-1, type=int)
  parser.add_option("--first-event", help="First events to run (0 by default)", default=0, type=int)
  parser.add_option("--debug", help="Run in debug mode", default=False, action="store_true")
  parser.add_option("--friend", help="Add input tree as friend of output", default=False, action="store_true")
  parser.add_option("--printBranches", help="Only print the branches used", 
    default=False, action="store_true")
  parser.add_option("--interact", 
    help="Instantiate egammaMVACalib and enter interactive mode", 
    default=False, action="store_true")
    
  io_group = OptionGroup(parser, "I/O options")
  io_group.add_option("-f", "--readFromFile", 
    help="Read the list of inputfiles from a text file", default=False, action="store_true")
  io_group.add_option("-d", "--dataset", help="Name of the dataset used. With this options the <inputfiles> must be a ROOT file containing a TFileCollection", type=str)
  io_group.add_option("-n", "--treename", 
    help="Name of input tree (default: %default)", default="egamma")
  io_group.add_option('-i', '--inputPath', 
    help="Path containing the xml files", type=str, action='append')
  parser.add_option_group(io_group)
  
  group1 = OptionGroup(parser, "Parameters of the optimization")
  group1.add_option("--useTMVA", help="Use TMVA instead of new BDTs", 
    action='append', type=int)
  group1.add_option("-m", "--method", 
    help="Regression methods to use (default: BDTG)", action='append', type=str)
  group1.add_option("-t", "--particleType", 
    help="Type of particle (0=photon, 1=electron) (default: %default)", default=0, type=int)
  group1.add_option("-T", "--calibrationType", help="Calibration type (0=correction to Ecluster, 1=correction to Eaccordion, 2=full calibration (default: 1)", type=int, action='append')
  group1.add_option("--shift", 
    help="Rescale the energy to (0=No rescale, 1=peak, 2=mean, 3=median, 4=mean10, 5=mean20, 6=median10, 7=median20)", type=int, action='append')
  group1.add_option("--fudge", default=None, type=int,
    help="Version of fudge factors to the shower shapes before calculating the MVA response")
  group1.add_option("--filePattern", default="", type=str, 
    help="Pattern of xml files (leave blank)")
  group1.add_option("--spec", dest="ignoreSpectators", default=True, action="store_false",
    help="Enable spectators")
  group1.add_option("--defs", type=str, action='append',
    help="Definitions, comma separated (e.g.: el_rawcl_Es1=1.01*el_rawcl_Es1,...)")
  parser.add_option_group(group1)
  
  binDef_group = OptionGroup(parser, "Bin definitions")
  binDef_group.add_option("-e", "--etaBin", 
    help="Formula that defines eta bins (default: %default)", default="")
  binDef_group.add_option("-E", "--energyBin", 
    help="Formula that defines E/Et bins (default: %default)", default="")
  binDef_group.add_option("--particleTypeVar", 
    help="Formula that defines unconv, conv, SiSi photons", default="")
  parser.add_option_group(binDef_group)

  
  outtree_group = OptionGroup(parser, "Options for output tree")
  outtree_group.add_option("--branchName", 
    help="Name of the output branch (method name by default)", type=str, action="append")
  outtree_group.add_option("--copyBranches", 
    help="Branches to copy, comma-separated ('input' branches by default)", default="input", type=str)
  parser.add_option_group(outtree_group)

# Other parameters of constructor
#   std::string particleTypeVar = "",
  return parser

if __name__ == "__main__":
  parser = getParser(__doc__)
  (options, inputfiles) = parser.parse_args()
  if not options.useTMVA:
    options.useTMVA = False
  if not options.method:
    options.method = 'BDTG'
  if not options.shift:
    options.shift = -1
  if not options.calibrationType:
    options.calibrationType = 1
  if not options.branchName:
    options.branchName = ''
  if not options.inputPath:
    options.inputPath = ''
  
  try:
    outputfile = inputfiles.pop(0)
  except IndexError:
    outputfile = None
    if not options.interact:
      raise ValueError("Output/Inputfiles not given")

  inputfiles = sum((i.split(',') for i in inputfiles), [])
  assert inputfiles or options.interact, "Inputfiles not given"

  loadLibs() # has to be called before creating chain, at least on lxplus...
  chain = None if not inputfiles else \
    getTChain(inputfiles, options.treename, options.readFromFile, options.dataset)
  
      
  run_egammaMVACalib(outputfile, chain, options.inputPath, 
    options.useTMVA, options.particleType, 
    options.method, options.calibrationType,
    options.nEvents, options.debug, options.printBranches,
    options.branchName, options.copyBranches, options.shift, options.fudge,
    options.etaBin, options.energyBin, options.particleTypeVar, options.filePattern,
    options.ignoreSpectators, 
    options.interact, options.first_event, options.friend, options.defs)

  
