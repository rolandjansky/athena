#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import logging
import os
logging.basicConfig(level=logging.INFO)

__doc__ = """Command line interface to run egammaMVACalib or egammaMVACalibMulti
 (see test/testMVACalibMulti.sh)"""
__author__ = "Ruggero Turra <ruggero.turra@cern.ch>"


def doInteract(**kw):
    "Start the interactive mode, adding the given variables to globals"
    os.environ["PYTHONINSPECT"] = "1"
    globals().update(kw)
    print "\nEntering interactive mode, MVACalib object is called 'm'\n"


def make_list(x, N=1):
    "Return an iterable object, i.e. the object itself or a list with N (=1) elements"
    if hasattr(x, '__iter__'):
        return x
    else:
        return [x] * N


def setDefinitions(m, defs):
    """setDefinitions(m, defs) -> apply definitions of variables to m
    (e.g.: el_rawcl_Es1=1.01*el_rawcl_Es1,...)"""
    if defs:
        for definition in defs.split(','):
            m.setDefinition(*definition.split('='))


def create_TChain(inputfiles, treename):
    chain = ROOT.TChain(treename)
    for inputfile in inputfiles:
        chain.Add(inputfile)
    return chain


def create_file_list(list_filename, dataset_name):
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


def run_egammaMVACalib(outputfile, inputTree,
                       inputPath='egammaMVACalib/offline/v3', useTMVA='', particleType=0,
                       method='BDTG', calibrationType=1, nEvents=-1,
                       debug=False, printBranches=False, branchName="", copyBranches="input", shift=0,
                       etaBinDef='', energyBinDef='', particleTypeVar='',
                       filePattern="", ignoreSpectators=True,
                       interact=False, first_event=0, friend=False, defs=None):

    iter_options = inputPath, useTMVA, method, calibrationType, shift, branchName, defs
    if any(isinstance(i, (list, tuple)) and len(i) > 1 for i in iter_options):
        inputPath, useTMVA, method, calibrationType, shift, branchName, defs = iter_options
        return run_egammaMVACalibMulti(outputfile, inputTree, inputPath, useTMVA, method,
                                       particleType, calibrationType, nEvents,
                                       branchName, copyBranches, shift,
                                       debug, etaBinDef, energyBinDef, particleTypeVar, filePattern,
                                       ignoreSpectators, first_event, friend, defs)  # FIXME
    else:  # Make sure we do not have lists
        inputPath, useTMVA, method, calibrationType, shift, branchName, defs = (i[0] if isinstance(i, (list, tuple)) else i for i in iter_options)

    params = particleType, not useTMVA, inputPath, method, calibrationType, debug, etaBinDef, energyBinDef, particleTypeVar, filePattern, ignoreSpectators
    m = ROOT.egammaMVACalib(*params)
    if shift is not None and shift >= 0:
        m.setPeakCorrection(shift)
    setDefinitions(m, defs)

    if printBranches:
        m.InitTree(inputTree)
        for i in m.getListOfBranches():
            print i.GetName()
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
                            copyBranches='input', shift=0,
                            debug=False, etaBinDef='', energyBinDef='', particleTypeVar='',
                            filePattern='', ignoreSpectators=True, interact=False, friend=False, defs=None):
    logging.info("Running egammaMVACalibMulti")
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
        raise ValueError("Multiple options passed, branchName must have %s values, got %s" % (len(loop), len(branchName)))
    if len(set(branchName)) != len(branchName):
        raise ValueError("Repeated values for branchName: %s" % branchName)

    M = ROOT.egammaMVACalibMulti(particleType, inputTree, copyBranches)
    for i, j in enumerate(loop):
        inputPath, useTMVA, method, calibrationType, shift, defs = j
        params = particleType, not useTMVA, inputPath, method, calibrationType, debug, etaBinDef, energyBinDef, particleTypeVar, filePattern, ignoreSpectators
        m = ROOT.egammaMVACalib(*params)
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

if __name__ == "__main__":
    from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)
    parser.description = __doc__
    parser.epilog = "Example:"

    parser.add_argument("outputfile")
    parser.add_argument("inputfiles", nargs='+')

    parser.add_argument("-N", "--nEvents", help="Events to run (all by default)", default=-1, type=int)
    parser.add_argument("--first-event", help="First events to run (0 by default)", default=0, type=int)
    parser.add_argument("--debug", help="Run in debug mode", default=False, action="store_true")
    parser.add_argument("--friend", default=False, action="store_true",
                        help="Add input tree as friend of output", )
    parser.add_argument("--printBranches", help="Only print the branches used", default=False, action="store_true")
    parser.add_argument("--interact", help="Instantiate egammaMVACalib and enter interactive mode", default=False, action="store_true")

    io_group = parser.add_argument_group("I/O options")
    io_group.add_argument("-f", "--readFromFile", help="Read the list of inputfiles from a text file", default=False, action="store_true")
    io_group.add_argument("-d", "--dataset", help="Name of the dataset used. With this options the <inputfiles> must be a ROOT file containing a TFileCollection", type=str)
    io_group.add_argument("-n", "--treename", help="Name of input tree", default="egamma")
    io_group.add_argument('-i', '--inputPath', nargs='*', help="Path containing the xml/ROOT files", type=str, default=['egammaMVACalib/offline/v3'])

    group1 = parser.add_argument_group("Parameters of the optimization")
    group1.add_argument("--useTMVA", nargs='*', help="Use TMVA instead of new BDTs", type=bool, default=[False])
    group1.add_argument("-m", "--method", help="Regression methods to use (default: BDTG)", nargs='*', default=['BDTG'], type=str)
    group1.add_argument("-t", "--particleType", required=True, help="Type of particle (0=photon, 1=electron)",  type=int)
    group1.add_argument("-T", "--calibrationType", nargs='*', help="Calibration type (0=correction to Ecluster, 1=correction to Eaccordion, 2=full calibration", type=int, default=[1])
    group1.add_argument("--shift", help="Rescale the energy to (0=No rescale, 1=peak, 2=mean, 3=median, 4=mean10, 5=mean20, 6=median10, 7=median20)", type=int, action='append')
    group1.add_argument("--filePattern", default="", type=str, help="Pattern of xml files (leave blank)")
    group1.add_argument("--spec", dest="ignoreSpectators", default=True, action="store_false", help="Enable spectators")
    group1.add_argument("--defs", type=str, action='append', help="Definitions, comma separated (e.g.: el_rawcl_Es1=1.01*el_rawcl_Es1,...)")

    binDef_group = parser.add_argument_group("Bin definitions")
    binDef_group.add_argument("-e", "--etaBin", help="Formula that defines eta bins", default="")
    binDef_group.add_argument("-E", "--energyBin", help="Formula that defines E/Et bins", default="")
    binDef_group.add_argument("--particleTypeVar", help="Formula that defines unconv, conv, SiSi photons", default="")

    outtree_group = parser.add_argument_group("Options for output tree")
    outtree_group.add_argument("--branchName", nargs='*', help="Name of the output branch (method name by default)", default=['output_energy'])
    outtree_group.add_argument("--copyBranches", help="Branches to copy, comma-separated ('input' branches by default)", default="input", type=str)

    args = parser.parse_args()

    import ROOT
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    chain = getTChain(args.inputfiles, args.treename, args.readFromFile, args.dataset)
    logging.info("running on %d events", chain.GetEntries())

    run_egammaMVACalib(args.outputfile, chain, args.inputPath,
                       args.useTMVA, args.particleType,
                       args.method, args.calibrationType,
                       args.nEvents, args.debug, args.printBranches,
                       args.branchName, args.copyBranches, args.shift,
                       args.etaBin, args.energyBin, args.particleTypeVar, args.filePattern,
                       args.ignoreSpectators,
                       args.interact, args.first_event, args.friend, args.defs)
