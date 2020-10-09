// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODEGAMMA_EGAMMAENUMS_H
#define XAODEGAMMA_EGAMMAENUMS_H

namespace xAOD {

namespace EgammaParameters {

/// @name egamma types
enum EgammaType
{
  electron = 0,
  unconvertedPhoton = 1,
  convertedPhoton = 2,
  NumberOfEgammaTypes = 3
};

/// @name Shower shape types
/// @{
enum ShowerShapeType
{
  /// @brief uncalibrated energy (sum of cells) in presampler in a 1x1 window in
  /// cells in eta X phi
  e011 = 0,
  /// @brief uncalibrated energy (sum of cells) in presampler in a 3x3 window in
  /// cells in eta X phi
  e033 = 1,
  /// @brief uncalibrated energy (sum of cells) in strips in a 3x2 window in
  /// cells in eta X phi
  e132 = 2,
  /// @brief uncalibrated energy (sum of cells) in strips in a 15x2 window in
  /// cells in eta X phi
  e1152 = 3,
  /// @brief transverse energy in the first sampling of the hadronic
  /// calorimeters behind the cluster calculated from ehad1
  ethad1 = 4,
  /// @brief ET leakage into hadronic calorimeter with exclusion of energy in
  /// CaloSampling::TileGap3
  ethad = 5,
  /// @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 +
  /// CaloSampling::TileBar0 + CaloSampling::TileExt0)
  ehad1 = 6,
  /// @brief E1/E = fraction of energy reconstructed in the first sampling,
  /// where E1 is energy in all strips belonging to the cluster and E is the
  /// total energy reconstructed in the electromagnetic calorimeter cluster
  f1 = 7,
  /// @brief fraction of energy reconstructed in 3rd sampling
  f3 = 8,
  /// @brief E1(3x1)/E = fraction of the energy reconstructed in the first
  /// longitudinal compartment of the electromagnetic calorimeter, where E1(3x1)
  /// the energy reconstructed in +/-3 strips in eta, centered around the
  /// maximum energy strip and E is the energy reconstructed in the
  /// electromagnetic calorimeter
  f1core = 9,
  /// @brief E3(3x3)/E fraction of the energy reconstructed in the third
  /// compartment of the electromagnetic calorimeter, where E3(3x3), energy in
  /// the back sampling, is the sum of the energy contained in a 3x3 window
  /// around the maximum energy cell
  f3core = 10,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 3x3 (in cell units eta X phi)
  e233 = 11,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 3x5
  e235 = 12,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 5x5
  e255 = 13,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 3x7
  e237 = 14,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 7x7
  e277 = 15,
  /// @brief uncalibrated energy (sum of cells) of the third sampling in a
  /// rectangle of size 3x3
  e333 = 16,
  /// @brief uncalibrated energy (sum of cells) of the third sampling in a
  /// rectangle of size 3x5
  e335 = 17,
  /// @brief uncalibrated energy (sum of cells) of the third sampling in a
  /// rectangle of size 3x7
  e337 = 18,
  /// @brief uncalibrated energy (sum of cells) of the middle sampling in a
  /// rectangle of size 7x7
  e377 = 19,
  /// @brief shower width using +/-3 strips around the one with the maximal
  /// energy deposit:
  ///	w3 strips = sqrt{sum(Ei)x(i-imax)^2/sum(Ei)}, where i is the number of
  ///the strip and imax the strip number of the most energetic one
  weta1 = 20,
  /// @brief the lateral width is calculated with a window of 3x5 cells using
  /// the energy weighted  sum over all cells, which depends on the particle
  /// impact point inside the cell: weta2 =
  ///	sqrt(sum Ei x eta^2)/(sum Ei) -((sum Ei x eta)/(sum Ei))^2, where Ei is
  ///the energy of the i-th cell
  weta2 = 21,
  /// @brief 2nd max in strips calc by summing 3 strips
  e2ts1 = 22,
  /// @brief energy of the cell corresponding to second energy maximum in the
  /// first sampling
  e2tsts1 = 23,
  /// @brief shower shape in the shower core : [E(+/-3)-E(+/-1)]/E(+/-1), where
  /// E(+/-n) is the energy in ± n strips around the strip with highest energy
  fracs1 = 24,
  /// @brief same as egammaParameters::weta1 but without corrections  on
  /// particle impact point inside the cell
  widths1 = 25,
  /// @brief same as egammaParameters::weta2 but without corrections on particle
  /// impact point inside the cell
  widths2 = 26,
  /// @brief relative position in eta within cell in 1st sampling
  poscs1 = 27,
  /// @brief relative position in eta within cell in 2nd sampling
  poscs2 = 28,
  /// @brief uncorr asymmetry in 3 strips in the 1st sampling
  asy1 = 29,
  /// @brief difference between shower cell and predicted track in +/- 1 cells
  pos = 30,
  /// @brief Difference between the track and the shower positions:
  ///	sum_{i=i_m-7}^{i=i_m+7}E_i x (i-i_m) / sum_{i=i_m-7}^{i=i_m+7}E_i,
  ///	The difference between the track and the shower positions measured
  ///	in units of distance between the strips, where i_m is the impact cell
  ///	for the track reconstructed in the inner detector and E_i is the energy
  ///	reconstructed in the i-th cell in the eta direction for constant phi
  ///given by the track parameters
  pos7 = 31,
  /// @brief  barycentre in sampling 1 calculated in 3 strips
  barys1 = 32,
  /// @brief shower width is determined in a window detaxdphi = 0,0625 ×~0,2,
  /// corresponding typically to 20 strips in
  /// eta : wtot1=sqrt{sum Ei x ( i-imax)^2 / sum Ei}, where i is the strip
  /// number and imax the strip number of the first local maximum
  wtots1 = 33,
  /// @brief energy reconstructed in the strip with the minimal value between
  /// the first and second maximum
  emins1 = 34,
  /// @brief energy of strip with maximal energy deposit
  emaxs1 = 35,
  /// @brief  1-ratio of energy in 3x3 over 3x7 cells;
  ///	E(3x3) = E0(1x1) + E1(3x1) + E2(3x3) + E3(3x3); E(3x7) = E0(3x3) +
  ///E1(15x3) + E2(3x7) + E3(3x7)
  r33over37allcalo = 36,
  /// @brief core energy in em calo  E(core) = E0(3x3) + E1(15x2) + E2(5x5) +
  /// E3(3x5)
  ecore = 37,
  /// @brief  e237/e277
  Reta = 38,
  /// @brief  e233/e237
  Rphi = 39,
  /// @brief (emaxs1-e2tsts1)/(emaxs1+e2tsts1)
  Eratio = 40,
  /// @brief ethad/et
  Rhad = 41,
  /// @brief ethad1/et
  Rhad1 = 42,
  /// @brief e2tsts1-emins1
  DeltaE = 43,
  /// maximum number of enums
  NumberOfShowerShapes = 44
};
/// @}

/// @name Track Match variable types
/// @{
enum TrackCaloMatchType
{
  /// @brief difference between the cluster eta (presampler) and
  /// the eta of the track extrapolated to the presampler
  deltaEta0 = 0,
  ///@brief difference between the cluster eta (first sampling) and the eta of
  ///the track extrapolated to the
  /// first sampling: |eta_stripscluster -eta_ID|, where eta_stripscluster is
  /// computed
  /// in the first sampling of the electromagnetic calorimeter, where the
  /// granularity is very fine, and eta_ID is the pseudo-rapidity
  /// of the track extrapolated to the calorimeter
  deltaEta1 = 1,
  /// @brief difference between the cluster eta (second sampling) and the eta of
  /// the track extrapolated to the second sampling
  deltaEta2 = 2,
  /// @brief difference between the cluster eta (3rd sampling) and
  ///	  the eta of the track extrapolated to the 3rd sampling
  deltaEta3 = 3,
  /// @brief difference between the cluster phi (presampler) and
  ///	  the eta of the track extrapolated to the presampler
  deltaPhi0 = 4,
  /// @brief difference between the cluster eta (1st sampling) and
  ///	  the eta of the track extrapolated to the 1st sampling (strips)
  deltaPhi1 = 5,
  /// @brief difference between the cluster phi (second sampling) and the phi of
  /// the track
  ///	  extrapolated to the second sampling : |phi_middlecluster -phi_ID|,
  ///where phi_middlecluster
  ///	  is computed in the second compartment of the electromagnetic
  ///calorimeter and phi_ID is the
  ///  azimuth of the track extrapolated to the calorimeter
  deltaPhi2 = 6,
  ///@brief difference between the cluster eta (3rd sampling) and
  ///	  the eta of the track extrapolated to the 3rd sampling
  deltaPhi3 = 7,
  /// @brief difference between the cluster phi (sampling 2) and the
  /// eta of the track extrapolated from the last measurement point.
  deltaPhiFromLastMeasurement = 8,
  /// @brief difference between the cluster phi (presampler) and
  ///	  the eta of the track extrapolated to the presampler  from the perigee
  ///with a rescaled
  /// momentum.
  deltaPhiRescaled0 = 9,
  /// @brief difference between the cluster eta (1st sampling) and
  ///	  the eta of the track extrapolated to the 1st sampling (strips) from
  ///the perigee with a rescaled
  /// momentum.
  deltaPhiRescaled1 = 10,
  /// @brief difference between the cluster phi (second sampling) and the phi of
  /// the track
  ///	  extrapolated to the second sampling from the perigee with a rescaled
  /// momentum.
  deltaPhiRescaled2 = 11,
  ///@brief difference between the cluster eta (3rd sampling) and
  /// the eta of the track extrapolated to the 3rd sampling from the perigee
  /// with a rescaled
  /// momentum.
  deltaPhiRescaled3 = 12,
  /// maximum number of enums
  NumberOfTrackMatchProperties = 13
};
///@}

/// @name Vertex Match variable types
/// @{
enum VertexCaloMatchType
{
  /// @brief difference between the cluster eta  and
  /// the eta of the first track of the vertex extrapolated to the second
  /// sampling.
  convMatchDeltaEta1 = 0,

  /// @brief difference between the cluster eta  and
  /// the eta of the second track of the vertex extrapolated to the second
  /// sampling.
  convMatchDeltaEta2 = 1,

  /// @brief difference between the cluster phi  and
  /// the phi of the first track of the vertex extrapolated to the second
  /// sampling.
  convMatchDeltaPhi1 = 2,

  /// @brief difference between the cluster phi  and
  /// the phi of the second track of the vertex extrapolated to the second
  /// sampling.
  convMatchDeltaPhi2 = 3,

  /// maximum number of enums
  NumberOfVertexMatchProperties = 4
};
///@}

/// @name Conversion types
/// @{
enum ConversionType
{
  /// @brief unconverted photon
  unconverted = 0,

  /// @brief one track only, with Si hits
  singleSi = 1,

  /// @brief one track only, no Si hits (TRT only)
  singleTRT = 2,

  /// @brief two tracks, both with Si hits
  doubleSi = 3,

  /// @brief two tracks, none with Si hits (TRT only)
  doubleTRT = 4,

  /// @brief two tracks, only one with Si hits
  doubleSiTRT = 5,

  /// maximum number of types
  NumberOfVertexConversionTypes
};
///@}

} // End namespace EgammaParameters

} // End namespace xAOD
#endif // XAODEGAMMA_EGAMMAENUMS_H
