/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TileL2Algs_page 

This package contains the code related to the two trigger oriented algorithms that are processed at the TileCal ROD DSPs:

- Low \f$ p_{\rm{T}} \f$ muon tagging
- \f$ E_{\rm{T}} \f$ computation (MET trigger)

These algorithms are implemented in the ROD DSP code and meant to be running online during LHC operation. Their output is encoded in dedicated subfragments in the ROD data format (see TileByteStream package) to contribute to the LVL2 trigger. A specific class, called TileL2 (see TileEvent package), is used to store the results from these ROD DSP algorithms. Each of these algorithms has associated a LVL2 trigger algorithm. In particular, the muon identification performed at the ROD DSPs is used at LVL2 by TrigTileRODMuAlg (see TrigTileMuId package).

@author Aranzazu Ruiz Martinez Aranzazu.Ruiz.Martinez@cern.ch
@author Carlos Solans Carlos.Solans@cern.ch
@author Jose Maneira Jose.Maneira@cern.ch

@section TileL2Algs_Class Classes

The TileL2Algs package contains the following classes:

<ul>

<li>TileRawChannelToL2: emulates the algorithms processed at the TileCal ROD DSP level to contribute to the LVL2 trigger:<br>
<ul>
<li>a low \f$ p_{\rm{T}} \f$ muon tagging algorithm based on TileCal cell energy deposition following projective patterns in eta. The following condition is applied in the 3 layers: \f$ thr_{\rm{low}} \leq  E \leq thr_{\rm{hi}} \f$. The thresholds are used to cut the electronic noise, the minimum bias pileup and the tails of hadronic showers. 
<li>\f$ E_{\rm{T}} \f$ calculation per superdrawer
</ul>
It takes as input the raw channels from TileRawChannelContainer. The resulting TileL2 objects store the information about the number of tagged muons, their coordinates eta and phi, the energy deposited in TileCal, a quality factor and the \f$ E_{\rm{T}} \f$ per superdrawer. There is a TileL2 object per superdrawer, stored in a TileL2Container in StoreGate.

<li>TileMuRODToNtuple: retrieves the TileL2 objects from TileL2Container and dumps the muon tagging information calculated inside the TileCal ROD DSPs in a directory, called TileMuRODTag, in the ntuple. The variables stored in the ntuple are:
<ul>
<li>NMuons: number of muons found in TileCal
<li>EtaMuons: muon eta coordinate
<li>PhiMuons: muon phi coordinate
</ul>

<li>CBNTAA_TileMuROD: retrieves the TileL2 objects from TileL2Container and dumps the muon tagging information calculated inside the TileCal ROD DSPs in AANT. The variables stored in the ntuple are:
<ul>
<li>TileMuId_ROD_NMuons: number of muons found in TileCal
<li>TileMuId_ROD_Eta: muon eta coordinate calculated as the average of the three cell eta coordinates the muon goes through
<li>TileMuId_ROD_Phi: muon phi coordinate
<li>TileMuId_ROD_EnergyDep0: energy deposited by the muon in the A cell
<li>TileMuId_ROD_EnergyDep1: energy deposited by the muon in the BC cell
<li>TileMuId_ROD_EnergyDep2: energy deposited by the muon in the D cell
<li>TileMuId_ROD_Quality: quality factor set to 0 if the energy deposited by the muon is less than the high energy threshold for the 3 cells and set to 1 if the energy deposited by the muon is greater than the high energy threshold for 1 of the 3 cells.
</ul>

<li>CBNTAA_TileL2Met: retrieves the TileL2 objects from TileL2Container and dumps the transverse energy information calculated inside the TileCal ROD DSPs in AANT. The variables stored in the ntuple are:
<ul>
<li>TileL2Met_Hermetic
<li>TileL2Met_Ex
<li>TileL2Met_Ey
<li>TileL2Met_ScalarEt
</ul>

</ul>


*/
