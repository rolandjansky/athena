#!/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import ROOT
import math
from functools import partial
from itertools import islice

import logging
logging.basicConfig(level=logging.INFO, format="%(filename)s\t%(levelname)s    %(message)s")

ROOT.PyConfig.IgnoreCommandLineOptions = True


def get_electron_collection(tree):
    try:
        collection = tree.Electrons
    except AttributeError:
        collection = tree.ElectronCollection
    return collection


def get_photon_collection(tree):
    try:
        collection = tree.Photons
    except AttributeError:
        collection = tree.PhotonCollection
    return collection


def xAOD_particle_generator(tree, collection_getter, newevent_function=None, endevent_function=None,
                            event_numbers=None,
                            min_pt=None, min_abseta=None, max_abseta=None):
    if event_numbers is None:
        event_numbers = []
    elif type(event_numbers) is int:
        event_numbers = [event_numbers]

    for ievent in range(tree.GetEntries()):
        tree.GetEntry(ievent)
        ei = tree.EventInfo
        event_number = ei.eventNumber()
        if event_numbers:
            if event_number not in event_numbers:
                continue
        logging.debug("=== event number %d ievent = %d", event_number, ievent)
        if newevent_function is not None:
            newevent_function()

        collection = collection_getter(tree)

        for i in range(collection.size()):
            p = collection.at(i)
            if min_pt is not None and p.pt() < min_pt:
                continue
            if min_abseta is not None and abs(p.eta()) < min_abseta:
                continue
            if max_abseta is not None and abs(p.eta()) > max_abseta:
                continue
            yield p

        if endevent_function is not None:
            endevent_function()


xAOD_photon_generator = partial(xAOD_particle_generator, collection_getter=get_photon_collection)
xAOD_electron_generator = partial(xAOD_particle_generator, collection_getter=get_electron_collection)


def main(filename, **args):
    logging.debug("initializing xAOD")
    if (not ROOT.xAOD.Init().isSuccess()):
        raise IOError("Failed xAOD.Init()")


    tree = None
    if ".txt" in filename:
        logging.debug("filename is a list of files")
        chain = ROOT.TChain(args['tree_name'])
        for line in islice(open(filename), 10):
            logging.debug("adding %s", line.strip())
            chain.Add(line.strip())
        tree = ROOT.xAOD.MakeTransientTree(chain)
    else:
        logging.debug("opening file %s", filename)
        f = ROOT.TFile.Open(filename)
        if not f:
            logging.error("problem opening file %s", filename)
        tree = ROOT.xAOD.MakeTransientTree(f, args['tree_name'])
    if not tree:
        logging.warning("cannot find tree in the file")
        f.Print()
        return

    logging.info("input has %d entries", tree.GetEntries())

    logging.debug("initializing tool")
    tool = ROOT.CP.EgammaCalibrationAndSmearingTool("tool")
    tool.setProperty("ESModel", args["esmodel"]).ignore()
    tool.setProperty("decorrelationModel", args["decorrelation"]).ignore()
    if args["no_smearing"]:
        tool.setProperty("int")("doSmearing", 0).ignore()
    if args['debug']:
        tool.msg().setLevel(0)
    if args['use_afii'] is not None:
        tool.setProperty("bool")("useAFII", bool(args['use_afii'])).ignore()

    tool.initialize()

    if args['variation'] is not None:
        logging.info("applying systematic variation %s", args['variation'])
        sys_set = ROOT.CP.SystematicSet()
        sys_set.insert(ROOT.CP.SystematicVariation(args['variation'], args['variation_sigma']))
        tool.applySystematicVariation(sys_set)

    logging.debug("creating output tree")
    fout = ROOT.TFile("output.root", "recreate")
    tree_out = ROOT.TNtuple(args["tree_name"], args["tree_name"], "eventNumber:eta:phi:true_e:pdgId:e:xAOD_e:raw_e:raw_ps")

    logging.debug("looping on electron container")

    def newevent_function():
        logging.debug("executing new event function")
        tool.beginInputFile()
        tool.beginEvent()

    def endevent_function():
        logging.debug("executing end event function")
        tool.endInputFile()

    generator = xAOD_photon_generator(tree, newevent_function = newevent_function,
                                      endevent_function = endevent_function,
                                      min_pt=args['min_pt'],
                                      min_abseta=args['min_abseta'], max_abseta=args['max_abseta'],
                                      event_numbers=args['event_number'])

    _ = ROOT.xAOD.TruthParticle_v1    # this is needed to run the next line, don't know why, dictionary stuff...
    get_truth_particle = ROOT.xAOD.TruthHelpers.getTruthParticle

    for particle in islice(generator, None, args['nparticles']):
        ei = tree.EventInfo
        logging.debug(" === new photon eventNumber|eta|phi|e| = |%d|%.2f|%.2f|%.2f|", ei.eventNumber(), particle.eta(), particle.phi(), particle.e())
        if not particle.caloCluster():
            logging.warning("particle has no calo cluster")
            continue
        logging.debug("                |rawe0|raweAcc|TileGap3| = |%.2f|%.2f|%.2f|",
                      particle.caloCluster().energyBE(0),
                      particle.caloCluster().energyBE(1) + particle.caloCluster().energyBE(2) + particle.caloCluster().energyBE(3),
                      particle.caloCluster().eSample(17))
        xAOD_energy = particle.e()
        cluster = particle.caloCluster()
        raw_e = cluster.energyBE(1) + cluster.energyBE(2) + cluster.energyBE(3)
        raw_ps = cluster.energyBE(0)
        true_particle = get_truth_particle(particle)
        true_e = true_particle.e() if true_particle else 0
        pdgId = true_particle.pdgId() if true_particle else 0
        calibrated_energy = tool.getEnergy(particle)
        tree_out.Fill(ei.eventNumber(), cluster.eta(), cluster.phi(),
                      true_e, pdgId, calibrated_energy, xAOD_energy, raw_e, raw_ps)
        if math.isnan(calibrated_energy) or math.isnan(calibrated_energy) or calibrated_energy < 1:
            print("==>", particle.author(), particle.eta(), particle.phi(), xAOD_energy, calibrated_energy)


    logging.info("%d events written", tree_out.GetEntries())

    tree_out.Write()
    fout.Close()

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Run on xAOD and dump calibrated energy for electron and photons',
                                     formatter_class=argparse.RawTextHelpFormatter,
                                     epilog='example: ./run_xAOD_ElectronPhotonFourMomentumCorrection.py root://eosatlas.cern.ch//eos/atlas/user/t/turra/Hgamgam_20.7/mc15_13TeV:AOD.07922786._000014.pool.root.1')
    parser.add_argument('filename', type=str, help='path to xAOD')
    parser.add_argument('--nparticles', type=int, help='number of particles')
    parser.add_argument('--tree-name', type=str, default='CollectionTree')
    parser.add_argument('--min-pt', type=float, help='minimum pT')
    parser.add_argument('--min-abseta', type=float, help='minimum |eta|')
    parser.add_argument('--max-abseta', type=float, help='maximum |eta|')
    parser.add_argument('--event-number', type=int)
    parser.add_argument('--debug', action='store_true', default=False)
    parser.add_argument('--no-layer-correction', action='store_true', default=False)
    parser.add_argument('--no-smearing', action='store_true')
    parser.add_argument('--esmodel', default="es2015c_summer")
    parser.add_argument('--decorrelation', default='1NP_v1')
    parser.add_argument('--variation', type=str, help='variation to apply (optional)')
    parser.add_argument('--variation-sigma', type=int, default=1, help='number of sigma for the variation (+1 or -1)')
    parser.add_argument('--use-afii', type=int)

    args = parser.parse_args()
    if args.debug:
        logging.getLogger().setLevel(logging.DEBUG)
    main(**vars(args))
    ROOT.xAOD.ClearTransientTrees()
