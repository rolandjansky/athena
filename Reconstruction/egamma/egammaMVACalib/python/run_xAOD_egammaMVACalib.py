# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import math

def xAOD_electron_generator(tree, event_numbers=None):
    if event_numbers is None:
        event_numbers = []
    elif type(event_numbers) is int:
        event_numbers = [event_numbers]
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        if event_numbers:
            ei = tree.EventInfo
            if not ei.eventNumber() in event_numbers:
                print 'skipping', ei.eventNumber()
                continue

        try:
            collection = tree.Electrons
        except AttributeError:
            collection = tree.ElectronCollection
        for i in xrange(collection.size()):
            el = collection.at(i)
            yield el


def xAOD_photon_generator(tree):
    for ievent in xrange(tree.GetEntries()):
        tree.GetEntry(ievent)
        for i in xrange(tree.Photons.size()):
            el = tree.Photons.at(i)
            yield el


def main(filename, treename):
    if (not ROOT.xAOD.Init().isSuccess()):
        print "Failed xAOD.Init()"
    f = ROOT.TFile.Open(filename)
    if not f:
        print "ERROR: problem opening eos file"
    tree = ROOT.xAOD.MakeTransientTree(f, treename)

    tool = ROOT.egammaMVATool("egammaMVATool")
    tool.setProperty("folder", "egammaMVACalib/v1")
    tool.initialize()

    for electron in xAOD_electron_generator(tree):
        xAOD_energy = electron.e()
        MVA_energy = tool.getEnergy(electron.caloCluster(), electron)
        if math.isnan(xAOD_energy) or math.isnan(MVA_energy) or xAOD_energy < 1 or MVA_energy < 1:
            print electron.author(), electron.eta(), electron.phi(), xAOD_energy, MVA_energy


if __name__ == '__main__':
    ROOT.gROOT.ProcessLine(".x $ROOTCOREDIR/scripts/load_packages.C")
    filename = 'root://eosatlas.cern.ch://eos/atlas/atlaslocalgroupdisk/dq2/rucio/user/eifert/52/b7/user.eifert.4952555.EXT0._001968.DAOD_SUSY5.mc14_13TeV.110401.PowhegPythia_P2012_ttbar_nonallhad.e2928_s1982_s2008_r5787_r5853.p1.root'
    main(filename, 'CollectionTree')
