#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys
import os
import ROOT


def xAODDigest(evt):
    result = list()

    for i in range(0, evt.getEntries()):
        evt.getEntry(i)
        ei = evt.retrieve("xAOD::EventInfo", "EventInfo")
        runnbr = ei.runNumber()
        evtnbr = ei.eventNumber()

        clusters = evt.retrieve(
            "xAOD::CaloClusterContainer", "CaloCalTopoClusters")
        nclus = len(clusters)

        idTracks = evt.retrieve(
            "xAOD::TrackParticleContainer", "InDetTrackParticles")
        nIdTracks = len(idTracks)

        jets = evt.retrieve("xAOD::JetContainer", "AntiKt4EMTopoJets")
        nJets = len(jets)

        muons = evt.retrieve("xAOD::MuonContainer", "Muons")
        nMuons = len(muons)

        electrons = evt.retrieve("xAOD::ElectronContainer", "Electrons")
        nElec = len(electrons)

        photons = evt.retrieve("xAOD::PhotonContainer", "Photons")
        nPhot = len(photons)

        nTrueElectrons = 0
        nTruePhotons = 0
        acc = ROOT.SG.AuxElement.ConstAccessor(
            'ElementLink< xAOD::TruthParticleContainer>')('truthParticleLink')

        if nElec > 0 and acc.isAvailable(electrons.at(0)):
            for i in range(nElec):
                truthLink = acc(electrons.at(i))
                if(truthLink.isValid()):
                    pdgId = truthLink.pdgId()
                    if abs(pdgId) == 11:
                        nTrueElectrons += 1

        if nPhot > 0 and acc.isAvailable(photons.at(0)):
            for i in range(nPhot):
                truthLink = acc(photons.at(i))
                if(truthLink.isValid()):
                    pdgId = truthLink.pdgId()
                    if (pdgId == 22):
                        nTruePhotons += 1

        nFakeElectrons = nElec - nTrueElectrons
        nFakePhotons = nPhot - nTruePhotons

        result.append((runnbr, evtnbr, nclus, nIdTracks, nJets, nMuons,
                       nElec, nTrueElectrons, nFakeElectrons,
                       nPhot, nTruePhotons, nFakePhotons))
        pass

    # Sort by run/event number
    result.sort(key=lambda er: er[0] << 32 | er[1])
    return result


def main():
    if len(sys.argv) < 2 or sys.argv[1] == "-h" or len(sys.argv) > 3:
        print("Usage:")
        print("xAODDigest.py <xAODFile> [<outputFile>]")
        print("Extracts a few basic quantities from the"
              "xAOD file and dumps them into a text file")
        sys.exit(-1)

    # Check input file existnace
    if not os.access(sys.argv[1], os.R_OK):
        print("ERROR, can't access file {}".format(sys.argv[1]))
        sys.exit(-1)

    # Check output file ...
    outfilename = None
    if len(sys.argv) == 3:
        outfilename = sys.argv[2]
        print("Writing to file ", outfilename)

    evt = ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess)
    stat = evt.readFrom(sys.argv[1])
    if not stat:
        print("ERROR, failed to open file {} with POOL.TEvent".format(
            sys.argv[1]))
        sys.exit(-1)
        pass

    digest = xAODDigest(evt)

    if outfilename:
        outstr = open(outfilename, "w")
    else:
        outstr = sys.stdout

    header = ("run", "event", "nTopo", "nIdTracks", "nJets", "nMuons",
              "nElec", "nTrueElec", "nFakeElec",
              "nPhot", "nTruePhot", "nFakePhot")

    row_format = "{:>12}" * len(header)
    row_format += os.linesep
    outstr.write(row_format.format(*header))
    for evt in digest:
        outstr.write(row_format.format(*evt))

    if outfilename:
        outstr.close()

    return 0


if __name__ == "__main__":

    main()
