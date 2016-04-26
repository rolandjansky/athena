#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__ = "To compute corrected layer energies"

from glob import glob
import os.path
import os
from array import array
import sys
from itertools import izip
import logging
logging.basicConfig(level=logging.DEBUG)

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

try:
    ROOT.gROOT.Macro("$ROOTCOREDIR/scripts/load_packages.C")
except RuntimeError:
    print """you have to compile egammaLayerRecalibTool first:
rc find_packages
rc compile
"""


def LayerRecalibratedGenerator(input_tree, branches):
    ""
    tool = ROOT.egammaLayerRecalibTool("2012_alt_with_layer2")

    def _rec(eta, e0, e1, e2, e3):
        std_calibration_input = ROOT.StdCalibrationInputs(0, eta, e0, e1, e2, e3, 0)
        tool.scale_inputs(std_calibration_input)
        return std_calibration_input.E0raw, std_calibration_input.E1raw, std_calibration_input.E2raw, std_calibration_input.E3raw
    fcn = _rec
    entries = input_tree.GetEntries()
    for i in xrange(entries):
        input_tree.GetEntry(i)
        # check if branch exists
        try:
            inputs = map(input_tree.__getattr__, branches)
        except AttributeError:
            raise AttributeError("cannot find branches %s" % branches)

        try:
            yield fcn(*inputs)  # scalar
        except TypeError:
            result = [fcn(*ii) for ii in izip(*inputs)]  # vectors
            yield zip(*result)

if __name__ == "__main__":
    from optparse import OptionParser

    def foo_callback(option, opt, value, parser):
        setattr(parser.values, option.dest, value.split(','))

    parser = OptionParser()
    parser.add_option("-i", "--input", help="input file or directory")
    parser.add_option("--nevents", type=int, help="number of events")
    parser.add_option("-t", "--tree", help="input tree name")
    parser.add_option("-p", "--particle", type="choice", help="particle type (electron|photon)", choices=("electron","photon"))
    parser.add_option("-b", "--branches-to-copy", type='string',
                      action='callback',
                      callback=foo_callback,
                      help="comma separated list of branches to copy from the input tree, you can use \*, do not use spaces")

    (options, args) = parser.parse_args()

    input_filenames = None
    if os.path.isdir(options.input):
        input_filenames = glob(os.path.join(options.input, "*.root*"))
    else:
        input_filenames = [options.input]
    if not input_filenames:
        print "ERROR: no files found"

    if not options.particle:
        raise ValueError("you need to specify a value of --particle")

    logging.info("found %s files", len(input_filenames))

    try:
        ROOT.egammaLayerRecalibTool
    except:
        raise AttributeError('You have to load egammaLayerRecalibTool package')

    prefix = 'el_' if options.particle == 'electron' else 'ph_'

    for filename in input_filenames:
        print "patching file %s" % filename

        f = ROOT.TFile.Open(filename, "update")
        tree = f.Get(options.tree)
        if not tree:
            raise IOError("cannot find %s" % options.tree)


        branches = [prefix + 'cl_eta'] + [prefix + 'rawcl_Es%d' % i for i in range(4)]
        if not all(tree.GetBranch(i) for i in branches):
            logging.info("skipping %s*", prefix)
            continue

        branches_to_copy = options.branches_to_copy or []
        logging.info("copying these branches: %s", ",".join(branches_to_copy) if branches_to_copy else "None")

        new_branch_names = [prefix + 'rawcl_Es%d' % i for i in range(4)]
        if set(new_branch_names).intersection(branches_to_copy):
            logging.warning("raw_clE* variables in the copy list -> going to rename. PROBABILY YOU DON'T WANT TO DO IT!")
            new_branch_names = [prefix + 'rawcl_Es%d_new' % i for i in range(4)]

        tree.SetBranchStatus("*", 0)
        for branch in branches_to_copy:
            tree.SetBranchStatus(branch, 1)
        newtree = tree.CloneTree(0)
        newtree.SetName("layerRecalibratedTree")
        newtree.SetTitle("layerRecalibratedTree")

        # Disable all branches but the needed ones
        tree.SetBranchStatus('*', 0)
        for br in set(branches + branches_to_copy):
            logging.info("activating branch %s", br)
            tree.SetBranchStatus(br, 1)

        # Create new branch, the same type as the old one
        isVector = 'vector' in tree.GetLeaf(prefix + 'rawcl_Es0').GetTypeName()
        new_storages = []
        if isVector:
            for new_branch_name in new_branch_names:
                new_storage = ROOT.vector(float)()
                new_storages.append(new_storage)
                newtree.Branch(new_branch_name, new_storage)
        else:
            for new_branch_name in new_branch_names:
                new_storage = array('f', [0.])
                new_storages.append(new_storage)
                newtree.Branch(new_branch_name, new_storage, new_branch_name + '/F')

        for i, new_value in enumerate(LayerRecalibratedGenerator(tree, branches)):
            if i % 1000 == 0:
                print i
            if options.nevents and i >= options.nevents:
                break

            if isVector:
                for s, d in zip(new_storages, new_value):
                    s.clear()
                    map(s.push_back, d)
            else:
                for s, v in zip(new_storages, new_value):
                    s[0] = v
            newtree.Fill()
        print "loop ended after %d events" % i

        logging.debug("all events done")
        tree.SetBranchStatus('*', 1)
        logging.debug("adding friend")
        tree.AddFriend(newtree)
        logging.info("updating old tree")
        tree.GetCurrentFile().Write("", ROOT.TObject.kOverwrite)
        logging.info("writing new tree")
        if f.Get("layerRecalibratedTree"):
            newtree.GetCurrentFile().Write("", ROOT.TObject.kOverwrite)
        else:
            newtree.GetCurrentFile().Write()
        logging.debug("closing file")

        # this is because ROOT is stupid
        tree.SetDirectory(0)
        newtree.SetDirectory(0)

        f.Close()
        logging.debug("closed")

        logging.info("creating check plots")
        chain = ROOT.TChain(options.tree)
        for filename in input_filenames:
            chain.Add(filename)

        for old_branch_name, new_branch_name in zip([prefix + 'rawcl_Es%d' % i for i in range(4)], new_branch_names):
            canvas = ROOT.TCanvas()
            chain.Draw("layerRecalibratedTree.%s/%s:%scl_eta>>(100, -2.5, 2.5, 100, 0.9, 1.1)" % (new_branch_name, old_branch_name, prefix),
                       "abs(%scl_eta) < 2.5 && %s != 0" % (prefix, old_branch_name), "", options.nevents or 1000000000)
            canvas.SaveAs("canvas_check_%s.png" % new_branch_name)
