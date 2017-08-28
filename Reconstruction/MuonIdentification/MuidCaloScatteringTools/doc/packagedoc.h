/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuidCaloScatteringTools_page MuidCaloScatteringTools Package

This package provides the multiple scattering information for muon tracks upstream the muon spectrometer necessary during the muon track extrapolation from the muon spectrometer entrance to the production vertex and during the combined muon fit. The calorimeter material is modeled using two scattering centers along the muon track. If the material distribution was uniform, the two scaterring centers would have been placed symmetrically around the middle plane of the material in a distance (from this plane) of 1./sqrt(12.) of the total thickness (rms of a uniform distribution). Since this is not the case for the material upstream the muon spectrometer, the middle plane is located at middle of total material thickness , while the rms is calcualted by weighting the material thickness by the inverse of its radiation length.

@author Alan.Poppleton@cern.ch
@author Konstantinos.Nikolopoulos@cern.ch

@section MuidCaloScatteringTools_IntroductionMuidCaloScatteringTools Introduction

This package contains two AlgTools:

MuidCaloScattering : Allocates 2 scattering centers onto the track to represent the Coulomb scattering between the InDet and MuonSpectrometer entrance.

MuidCaloMaterialParam : Contains a model/parametrization of the calorimeter material. This parametrization represents the Coulomb scattering between the InDet and MuonSpectrometer entrance. The parametrized positions and thicknesses (CaloLayers) are stored as a function of eta assuming any track will 'see' 2 scattering centres, taken from an inner and an outer layer.

*/
