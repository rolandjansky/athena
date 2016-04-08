/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
\mainpage MCTruthAlgs

\section introduction Description:
These are a collection of algorithms which work on the 
TrackRecordCollection. In the standard G4 simulation, all the particles 
that pass through a layer in front of the muon spectrometer are 
recorded and saved in the TrackRecordCollection. 3 Algorithms are provided 
here

- TrackRecordFilter, to create a stripped version of the collection. This 
is used to reduce the size of the ESD and AOD files
- MergeTrackRecordCollection, used to copy the main collection to the 
output when running pileup

 
\section content Content:
Given below is a list of the parameters and their names (for usage in root, e.g).<br>
<table>
  <tr>
    <td>Variable</td>
    <td>Name</td>
    <td>Description</td>
  </tr>

  <tr>
    <td>NumberOfTracks</td>
    <td>MuonEntRec_Num</td>
    <td>Number of tracks in the event.</td>
  </tr>  

  <tr>
    <td>pdgID</td>
    <td>MuonEntRec_ID</td>
    <td>PDG Id of the particle (mu-=13, mu+ = -13)</td>
  </tr>

  <tr>
    <td>Energy</td>
    <td>MuonEntRec_Ene</td>
    <td>Energy of the particle (in MeV)</td>
  </tr>

  <tr>
    <td>Eta</td>
    <td>MuonEntRec_Eta</td>
    <td>Eta of the 4-momentum of the particle </td>
  </tr>

  <tr>
    <td>Phi</td>
    <td>MuonEntRec_Phi</td>
    <td>Phi of the 4-momentum of the particle </td>
  </tr>

  <tr>
    <td>X</td>
    <td>MuonEntRec_PosX</td>
    <td> Position x of the particle (in mm) </td>
  </tr>

  <tr>
    <td>Y</td>
    <td>MuonEntRec_PosY</td>
    <td> Position y of the particle (in mm) </td>
  </tr>

  <tr>
    <td>Z</td>
    <td>MuonEntRec_PosZ</td>
    <td> Position z of the particle (in mm) </td>
  </tr>


</table>


@htmlinclude used_packages.html

@include requirements

*/
