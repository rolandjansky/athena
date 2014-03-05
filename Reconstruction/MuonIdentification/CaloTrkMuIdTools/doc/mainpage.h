/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage The CaloTrkMuIdTools Package

@author Gustavo Ordonez Sanz <Gustavo.Ordonez.Sanz@cern.ch>

@section CaloTrkMuIdToolsIntroduction Introduction
	Athena package containing several AlgTools for retrieving tagging muons using energy deposits in the calorimeter.

@section CaloMuonLikeLihoodToolIntroduction CaloMuonLikelihoodTool
	AlgTool for Muon Identification in the Calorimeter.
	Builds likelihood ratio discriminant using distribution 
	of energy ratios for muons and pions.

@section CaloMuonTagIntroduction CaloMuonTag
	Muon tagger using calorimeter deposits.
	A track is tagged when deposits above the noise treshold are found in the 
	last three layers of the hadronic calorimeters (TileBar, TileExt and HEC).
	Upper cuts in the hadronic calorimeter are applied to provide a veto against hadrons.
	Upper cuts in the EM calorimeter are applied to provide a veto against punch-through electrons.
	All cuts are configurable.

@section TrackDepositInCaloToolIntroduction TrackDepositInCaloTool
	Tool for retrieving calorimeter deposits associated to a track (given by Trk::TrackParameters*) 	

@section CaloTrkMuIdToolsShare jobOptions
  The tools can be steered through jobOptions. The default configuration that is being employed for standard reconstruction is 
  set up by the jobOptions found in share. 

@section requirements Requirements
@include requirements

*/

