# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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


def xAOD_particle_generator(tree, collection_getter, event_numbers=None,
                            min_pt=None, min_abseta=None, max_abseta=None):
    if event_numbers is None:
        event_numbers = []
    elif type(event_numbers) is int:
        event_numbers = [event_numbers]

    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        if event_numbers:
            ei = tree.EventInfo
            if not ei.eventNumber() in event_numbers:
                continue

        collection = collection_getter(tree)

        for i in xrange(collection.size()):
            p = collection.at(i)
            if min_pt is not None and p.pT() < min_pt:
                continue
            if min_abseta is not None and abs(p.eta()) < min_abseta:
                continue
            if max_abseta is not None and abs(p.eta()) > max_abseta:
                continue
            yield p


xAOD_photon_generator = partial(xAOD_particle_generator, collection_getter=get_photon_collection)
xAOD_electron_generator = partial(xAOD_particle_generator, collection_getter=get_electron_collection)


def main(filename, **args):
    logging.debug("initializing xAOD")
    if (not ROOT.xAOD.Init().isSuccess()):
        print "Failed xAOD.Init()"

    logging.debug("initializing tool")
    tool = ROOT.egammaMVATool("egammaMVATool")
    if args['debug']:
        tool.msg().setLevel(0)
    tool.setProperty("folder", args['mva_folder'])
    if (args['no_layer_correction']):
        tool.setProperty("use_layer_corrected", False)

    tool.initialize()

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

    logging.debug("input has %d entries" % tree.GetEntries())

    logging.debug("creating output tree")
    fout = ROOT.TFile("output.root", "recreate")
    tree_out = ROOT.TNtuple(args["tree_name"], args["tree_name"], "eta:phi:true_e:pdgId:MVA_e:xAOD_e:raw_e:raw_ps")

    logging.debug("looping on electron container")
    generator = xAOD_electron_generator(tree, min_pt=args['min_pt'],
                                        min_abseta=args['min_abseta'], max_abseta=args['max_abseta'],
                                        event_numbers=args['event_number'])

    get_truth_particle = ROOT.xAOD.TruthHelpers.getTruthParticle

    for electron in islice(generator, None, args['nparticles']):
        logging.debug(" === new electron |eta|phi|e|rawe0|rawe1|TileGap3 = |%.2f|%.2f|%.2f|%.2f|%.2f|%.2f ===",
                      electron.eta(), electron.phi(), electron.e(),
                      electron.caloCluster().energyBE(0),
                      electron.caloCluster().energyBE(1),
                      electron.caloCluster().eSample(17)
                      )
        xAOD_energy = electron.e()
        cluster = electron.caloCluster()
        MVA_energy = tool.getEnergy(cluster, electron)
        true_el = get_truth_particle(electron)
        true_e = true_el.e() if true_el else 0
        pdgId = true_el.pdgId() if true_el else 0
        raw_e = cluster.energyBE(1) + cluster.energyBE(2) + cluster.energyBE(3)
        raw_ps = cluster.energyBE(0)
        tree_out.Fill(cluster.eta(), cluster.phi(),
                      true_e, pdgId, MVA_energy, xAOD_energy, raw_e, raw_ps)
        if math.isnan(xAOD_energy) or math.isnan(MVA_energy) or xAOD_energy < 1 or MVA_energy < 1:
            print "==>", electron.author(), electron.eta(), electron.phi(), xAOD_energy, MVA_energy

    logging.info("%d events written", tree_out.GetEntries())

    tree_out.Write()
    fout.Close()

if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    import argparse

    parser = argparse.ArgumentParser(description='Run on xAOD and dump calibrated energy for electron and photons',
                                     epilog='example: ./run_xAOD_egammaMVACalib.py root://eosatlas.cern.ch//eos/atlas/user/t/turra/DAOD_EGAM1.04186716._000048.pool.root.1')
    parser.add_argument('filename', type=str, help='path to xAOD')
    parser.add_argument('--nparticles', type=int, help='number of particles')
    parser.add_argument('--tree-name', type=str, default='CollectionTree')
    parser.add_argument('--min-pt', type=float, help='minimum pT')
    parser.add_argument('--min-abseta', type=float, help='minimum |eta|')
    parser.add_argument('--max-abseta', type=float, help='maximum |eta|')
    parser.add_argument('--event-number', type=int)
    parser.add_argument('--debug', action='store_true', default=False)
    parser.add_argument('--mva-folder', type=str, help='folder to be used default=egammaMVACalib/offline/v3_E4crack_bis', default='egammaMVACalib/offline/v3_E4crack_bis')
    parser.add_argument('--no-layer-correction', action='store_true', default=False)

    args = parser.parse_args()
    if args.debug:
        logging.getLogger().setLevel(logging.DEBUG)
    main(**vars(args))
