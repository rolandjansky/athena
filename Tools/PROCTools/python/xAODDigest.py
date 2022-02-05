#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
import sys
import os
import ROOT
import argparse

def safeRetrieve (evt, typ, key):
    if evt.contains (typ, key):
        return evt.retrieve (typ, key)
    print (f'WARNING: Cannot find object {typ}/{key}')
    return []

def xAODDigest(evt, counter=False, extravars=False):
    result = list()

    for i in range(0, evt.getEntries()):
        if (counter and (i % 100) == 0):
            print("Processing event %s" %i) 

        evt.getEntry(i)
        ei = evt.retrieve("xAOD::EventInfo", "EventInfo")
        runnbr = ei.runNumber()
        evtnbr = ei.eventNumber()

        clusters = safeRetrieve (evt, "xAOD::CaloClusterContainer", "CaloCalTopoClusters")
        nclus = len(clusters)

        idTracks = safeRetrieve(evt,
            "xAOD::TrackParticleContainer", "InDetTrackParticles")
        nIdTracks = len(idTracks)

        tautracks = safeRetrieve(evt, "xAOD::TauTrackContainer", "TauTracks")
        nTauTracks = len(tautracks)
        taus = safeRetrieve(evt, "xAOD::TauJetContainer", "TauJets")
        nTaus = len(taus)
        if taus:
          tau1pt = taus[0].pt()
          tau1eta = taus[0].eta()
          tau1phi = taus[0].phi()
        else:
          tau1pt = tau1eta = tau1phi = 0

        muons = safeRetrieve(evt, "xAOD::MuonContainer", "Muons")
        nMuons = len(muons)
        if muons:
          muon1pt = muons[0].pt()
          muon1eta = muons[0].eta()
          muon1phi = muons[0].phi()
        else:
          muon1pt = muon1eta = muon1phi = 0

        electrons = safeRetrieve(evt,"xAOD::ElectronContainer", "Electrons")
        nElec = len(electrons)
        if electrons:
          elec1pt = electrons[0].pt()
          elec1eta = electrons[0].eta()
          elec1phi = electrons[0].phi()
        else:
          elec1pt = elec1eta = elec1phi = 0

        photons = safeRetrieve(evt,"xAOD::PhotonContainer", "Photons")
        nPhot = len(photons)
        if photons:
          phot1pt = photons[0].pt()
          phot1eta = photons[0].eta()
          phot1phi = photons[0].phi()
        else:
          phot1pt = phot1eta = phot1phi = 0

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

        if extravars:
            result.append((runnbr, evtnbr, nclus, nIdTracks,
                           nTauTracks, nTaus, tau1pt, tau1eta, tau1phi,
                           nMuons, muon1pt, muon1eta, muon1phi,
                           nElec, elec1pt, elec1eta, elec1phi, nTrueElectrons, nFakeElectrons,
                           nPhot, phot1pt, phot1eta, phot1phi ,nTruePhotons, nFakePhotons))
        else:
            result.append((runnbr, evtnbr, nclus, nIdTracks, nTauTracks, nTaus, nMuons,
                           nElec, nTrueElectrons, nFakeElectrons,
                           nPhot, nTruePhotons, nFakePhotons))

        pass

    # Sort by run/event number
    result.sort(key=lambda er: er[0] << 32 | er[1])
    return result


def main():
    parser = argparse.ArgumentParser()
    parser = argparse.ArgumentParser(description="Extracts a few basic quantities from the xAOD file and dumps them into a text file")
    parser.add_argument("xAODFile", nargs='?', type=str, help="xAOD filename", action="store")
    parser.add_argument("outfilename", nargs='?', help="output text file for results", action="store", default=None)
    parser.add_argument("--outputfilename", help="output text file for results", action="store", default=None)
    parser.add_argument("--extravars", help="Extract extra variables: pt/eta/phi", action="store_true", default=False)
    parser.add_argument("--counter", help="Print event counter mod 100", action="store_true", default=False)
    parser.add_argument("--inputlist", help="Optional list of xAOD file instead of xAODFile parameter", nargs='+', action="store", default=False)
    args = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(-1)

    # Check input file existance
    if not args.inputlist and not os.access(args.xAODFile, os.R_OK):
        print("ERROR, can't access file {}".format(args.xAODFile))
        sys.exit(-1)

    # Check output file ...
    outfilename = ''
    if args.outfilename:
        outfilename = args.outfilename
    elif args.outputfilename:
        outfilename = args.outputfilename

    if outfilename:
        print("Writing to file ", outfilename)

    # Create TChain or single inputfile
    if args.inputlist:
        filelist = ROOT.TChain()
        for filename in args.inputlist:
            filelist.AddFile(filename)
    else:
        filelist = args.xAODFile

    # Setup TEvent object and add inputs
    evt = ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess)
    stat = evt.readFrom(filelist)
    if not stat:
        print("ERROR, failed to open file {} with POOL.TEvent".format(
            args.xAODFile))
        sys.exit(-1)
        pass

    digest = xAODDigest(evt, args.counter, args.extravars)

    if outfilename:
        outstr = open(outfilename, "w")
    else:
        outstr = sys.stdout

    if args.extravars:
        header = ("run", "event", "nTopo", "nIdTracks",
                  "nTauTracks", "nTaus", "tau1pt", "tau1eta", "tau1phi", 
                  "nMuons", "muon1pt", "muon1eta", "muon1phi",
                  "nElec", "elec1pt", "elec1eta", "elec1phi", "nTrueElec", "nFakeElec",
                  "nPhot", "phot1pt", "phot1eta", "phot1phi", "nTruePhot", "nFakePhot")
        row_format_header = "{:>20}" * len(header)
        row_format_header += os.linesep
        row_format_data = "{:d} {:d} " + "{:20.4f}" * (len(header)-2)
        row_format_data += os.linesep
    else:
        header = ("run", "event", "nTopo", "nIdTracks", "nTauTracks", "nTaus", "nMuons", 
                  "nElec", "nTrueElec", "nFakeElec",
                  "nPhot", "nTruePhot", "nFakePhot")
        row_format_header = "{:>12}" * len(header)
        row_format_header += os.linesep
        row_format_data = "{:>12}" * len(header)
        row_format_data += os.linesep

    outstr.write(row_format_header.format(*header))
    for evt in digest:
        outstr.write(row_format_data.format(*evt))

    if outfilename:
        outstr.close()

    return 0


if __name__ == "__main__":

    main()
