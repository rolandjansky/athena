/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigmuComb_page 
@author Stefano Giagu

@section TrigmuComb_TrigmuCombOverview Overview
This LVL2 algorithm combines a standalone muon reconstructed by the muFast algorithm 
with an inner detector track to improve the determination of the transverse momentum of the 
muon, to reduce the rate of fakes and to help rejectig decay in flight of kaons and pions.
The muComb algorithm takes in input a muFast muon and the collection of ID tracks reconstructed 
in a ROI around the muon direction, and fnd the best matching tracks by back-extrapolating the muFast muon 
to the primary vertex and by comparing the extrapolated eta and phi with the ones of the ID tracks.
The combined muon informations ar saved in a CombinedMuonFeature object atatched to the 
event record for further analysis.



*/
