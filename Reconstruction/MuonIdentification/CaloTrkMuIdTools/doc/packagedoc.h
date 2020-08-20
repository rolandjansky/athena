/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page CaloTrkMuIdTools_page The CaloTrkMuIdTools Package

@author Gustavo Ordonez Sanz <Gustavo.Ordonez.Sanz@cern.ch>

@section CaloTrkMuIdTools_CaloTrkMuIdToolsIntroduction Introduction
	Athena package containing several AlgTools for retrieving tagging muons using energy deposits in the calorimeter.

@section CaloTrkMuIdTools_CaloMuonLikeLihoodToolIntroduction CaloMuonLikelihoodTool
	AlgTool for Muon Identification in the Calorimeter.
	Builds likelihood ratio discriminant using distribution 
	of energy ratios for muons and pions.

@section CaloTrkMuIdTools_CaloMuonScoreToolIntroduction CaloMuonScoreTool
	AlgTool for Muon Identification in the Calorimeter.
	Computes a muon score representing based on the particle's 
	calorimeter cell energy deposits using a convolutional 
	neural network.

@section CaloTrkMuIdTools_CaloMuonScoreONNXRuntimeSvcIntroduction CaloMuonScoreONNXRuntimeSvc
	Service that maintains a ONNX session which holds 
	a tensorflow model and can perform inference on it.

@section CaloTrkMuIdTools_CaloMuonTagIntroduction CaloMuonTag
	Muon tagger using calorimeter deposits.
	A track is tagged when deposits above the noise treshold are found in the 
	last three layers of the hadronic calorimeters (TileBar, TileExt and HEC).
	Upper cuts in the hadronic calorimeter are applied to provide a veto against hadrons.
	Upper cuts in the EM calorimeter are applied to provide a veto against punch-through electrons.
	All cuts are configurable.

@section CaloTrkMuIdTools_TrackDepositInCaloToolIntroduction TrackDepositInCaloTool
	Tool for retrieving calorimeter deposits associated to a track (given by Trk::TrackParameters*) 	

@section CaloTrkMuIdTools_CaloTrkMuIdToolsShare jobOptions
  The tools can be steered through jobOptions. The default configuration that is being employed for standard reconstruction is 
  set up by the jobOptions found in share. 


*/
