# TrkToLeptonPVTool

## Introduction
This package is intended to fit a PrimaryVertex made of identified leptons (e.g. Z->mumu,ee) with the beamspot constraint and check consistency of other tracks in event with this vertex or other PV.

Package works on xAOD (beamspot is taken via the beamspot service) and on DAOD (beamspot is taken from EventInfo).
It works in Athena and AthDerivation

## Usage

Entry:
unique_ptr<xAOD::Vertex> matchTrkToPV(const xAOD::TrackParticle *trk, const xAOD::Vertex * PV, const xAOD::EventInfo *)
returns vertex obtained by merging an input track and already known PV. This fit has NDF=2, its Chi2 can be used to check the track-PV compatibility. No beamspot constraint is used, because it's assumed that the provided PV already has it.

Entry:
unique_ptr<const xAOD::Vertex> npartVertex(const std::vector<const xAOD::TrackParticle*>&, const xAOD::EventInfo *)
returns vertex made of any amount (>=2) of TrackParticles(e.g. leptons) with beamspot constraint. It can be used in case when PV is not known or can be unambigously reconstructed using e.g. leptons from Z decay. The compatibility of any other track to the lepton PV should be checked by subsequent fit npartVertex(2leptons+track).  This 3 particle vertex fit has NDF=5, its Chi2 is a measure of compatibility of the track in quetion to the identified lepton vertex. This is a more powerful check than the matchTrkToPV() based one.

