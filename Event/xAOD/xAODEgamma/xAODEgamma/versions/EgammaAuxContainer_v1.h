// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaAuxContainer_v1.h 744495 2016-05-03 14:33:11Z krasznaa $
#ifndef XAODEGAMMA_VERSIONS_EGAMMAAUXCONTAINER_V1_H
#define XAODEGAMMA_VERSIONS_EGAMMAAUXCONTAINER_V1_H

// System include(s):
#include <vector>
#include "xAODEgamma/EgammaDefs.h"
#include "AthLinks/ElementLink.h"
#include "xAODCaloEvent/CaloClusterContainer.h" 

#include <stdint.h>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {


  /// Temporary container used until we have I/O for AuxStoreInternal
  ///
  /// This class is meant to serve as a temporary way to provide an auxiliary
  /// store with Athena I/O capabilities for the egamma EDM. Will be exchanged for
  /// a generic auxiliary container type (AuxStoreInternal) later on.
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
  ///
  /// $Revision: 744495 $
  /// $Date: 2016-05-03 16:33:11 +0200 (Tue, 03 May 2016) $
  ///

  class EgammaAuxContainer_v1 : public AuxContainerBase {


  protected:

    /// @name xAOD::EgammaAuxContainer constructors
    /// The xAOD::EgammaAuxContainer is supposed to be used only via the ElectronAuxContainter and PhotonAuxContainer.
    EgammaAuxContainer_v1();

  private:

    ///@name vector of links to cluster 
    /// @{

    std::vector< std::vector< ElementLink< CaloClusterContainer > > > caloClusterLinks;

    /// @}


    ///@name IParticle variables
    /// @{
    std::vector<float> pt;             
    std::vector<float> eta;           
    std::vector<float> phi;           
    std::vector<float> m;           
    /// @}

    ///@name Covariance Matrix
    /// @{
    std::vector< std::vector<float> >   EgammaCovarianceMatrix;
    /// @}
    
    ///@name Author
    /// @{
    std::vector< uint16_t > author;
    /// @}


    ///@name Data Quality flag
    /// @{
    std::vector< uint32_t > OQ;
    /// @}
    
    /// @name Shower shape details
    /// @{
    /// @brief uncalibrated energy (sum of cells) in presampler in a 1x1 window in cells in eta X phi
    std::vector< float >  e011;     
    /// @brief uncalibrated energy (sum of cells) in presampler in a 3x3 window in cells in eta X phi
    std::vector< float >  e033;     
    /// @brief uncalibrated energy (sum of cells) in strips in a 3x2 window in cells in eta X phi
    std::vector< float >  e132;     
    /// @brief uncalibrated energy (sum of cells) in strips in a 15x2 window in cells in eta X phi
    std::vector< float >  e1152;    
    /// @brief transverse energy in the first sampling of the hadronic calorimeters behind the cluster calculated from ehad1
    std::vector< float >  ethad1;   
    /// @brief ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3
    std::vector< float >  ethad;    
    /// @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)
    std::vector< float >  ehad1;    
    /// @brief E1/E = fraction of energy reconstructed in the first sampling, where E1 is energy in all strips belonging to the cluster and E is the total energy reconstructed in the electromagnetic calorimeter cluster
    std::vector< float >  f1;       
    /// @brief fraction of energy reconstructed in 3rd sampling
    std::vector< float >  f3;       
    /// @brief E1(3x1)/E = fraction of the energy reconstructed in the first longitudinal compartment of the electromagnetic calorimeter, where E1(3x1) the energy reconstructed in +/-3 strips in eta, centered around the maximum energy strip and E is the energy reconstructed in the electromagnetic calorimeter
    std::vector< float >  f1core;   
    /// @brief E3(3x3)/E fraction of the energy reconstructed in the third compartment of the electromagnetic calorimeter, where E3(3x3), energy in the back sampling, is the sum of the energy contained in a 3x3 window around the maximum energy cell
    std::vector< float >  f3core;   
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x3 (in cell units eta X phi)
    std::vector< float >  e233;     
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x5
    std::vector< float >  e235;     
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 5x5
    std::vector< float >  e255;     
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x7
    std::vector< float >  e237;     
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7
    std::vector< float >  e277;     
    /// @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x3
    std::vector< float >  e333;     
    /// @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x5
    std::vector< float >  e335;     
    /// @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x7
    std::vector< float >  e337;     
    /// @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7
    std::vector< float >  e377;     
    /// @brief shower width using +/-3 strips around the one with the maximal energy deposit: 
    ///	w3 strips = sqrt{sum(Ei)x(i-imax)^2/sum(Ei)}, where i is the number of the strip and imax the strip number of the most energetic one 
    std::vector< float >  weta1;    
    /// @brief the lateral width is calculated with a window of 3x5 cells using the energy weighted  sum over all cells, which depends on the particle impact point inside the cell: weta2 = 
    ///	sqrt(sum Ei x eta^2)/(sum Ei) -((sum Ei x eta)/(sum Ei))^2, where Ei is the energy of the i-th cell 
    std::vector< float >  weta2;    
    /// @brief 2nd max in strips calc by summing 3 strips
    std::vector< float >  e2ts1;    
    /// @brief energy of the cell corresponding to second energy maximum in the first sampling
    std::vector< float >  e2tsts1;  
    /// @brief shower shape in the shower core : [E(+/-3)-E(+/-1)]/E(+/-1), where E(+/-n) is the energy in +- n strips around the strip with highest energy
    std::vector< float >  fracs1;   
    /// @brief same as egammaParameters::weta1 but without corrections  on particle impact point inside the cell
    std::vector< float >  widths1;  
    /// @brief same as egammaParameters::weta2 but without corrections on particle impact point inside the cell
    std::vector< float >  widths2;  
    /// @brief relative position in eta within cell in 1st sampling 
    std::vector< float >  poscs1;   
    /// @brief relative position in eta within cell in 2nd sampling
    std::vector< float >  poscs2;   
    /// @brief uncorr asymmetry in 3 strips in the 1st sampling 
    std::vector< float >  asy1;     
    /// @brief difference between shower cell and predicted track in +/- 1 cells
    std::vector< float >  pos; 
    /// @brief Difference between the track and the shower positions: 
    ///	sum_{i=i_m-7}^{i=i_m+7}E_i x (i-i_m) / sum_{i=i_m-7}^{i=i_m+7}E_i, 
    ///	The difference between the track and the shower positions measured 
    ///	in units of distance between the strips, where i_m is the impact cell 
    ///	for the track reconstructed in the inner detector and E_i is the energy 
    ///	reconstructed in the i-th cell in the eta direction for constant phi given by the track parameters
    std::vector< float >  pos7; 
    /// @brief  barycentre in sampling 1 calculated in 3 strips
    std::vector< float >  barys1;   
    /// @brief shower width is determined in a window detaxdphi = 0,0625 x~0,2, corresponding typically to 20 strips in eta : wtot1=sqrt{sum Ei x ( i-imax)^2 / sum Ei}, where i is the strip number and imax the strip number of the first local maximum
    std::vector< float >  wtots1;   
    /// @brief energy reconstructed in the strip with the minimal value between the first and second maximum
    std::vector< float >  emins1;   
    /// @brief energy of strip with maximal energy deposit
    std::vector< float >  emaxs1;     
    /// @brief  1-ratio of energy in 3x3 over 3x7 cells; 
    ///	E(3x3) = E0(1x1) + E1(3x1) + E2(3x3) + E3(3x3); E(3x7) = E0(3x3) + E1(15x3) + E2(3x7) + E3(3x7)
    std::vector< float > r33over37allcalo;     
    /// @brief core energy in em calo  E(core) = E0(3x3) + E1(15x2) + E2(5x5) + E3(3x5)
    std::vector< float > ecore;
    /// @brief pointing z at vertex reconstructed from the cluster
    std::vector< float > zvertex;
    /// @brief error associated to zvertex 
    std::vector< float > errz;
    /// @brief pointing eta reconstructed from the cluster (first and second sampling)
    std::vector< float > etap; 
    /// @brief pointing depth of the shower as calculated in egammaqgcld
    std::vector< float > depth;
    /// @}

  
    /// @name Isolation variables
    /// @{
    /// @brief ET in a cone of R=0.45 in calo, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone;   
    /// @brief ET in a cone with half-opening angle 0.15, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone15; 
    /// @brief ET in a cone with half-opening angle 0.2, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone20; 
    /// @brief ET in a cone with half-opening angle 0.25, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone25; 
    /// @brief ET in a cone with half-opening angle 0.3, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 
    std::vector< float >  etcone30; 
    /// @brief ET in a cone with half-opening angle 0.35, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone35; 
    /// @brief ET in a cone with half-opening angle 0.4, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3
    std::vector< float >  etcone40; 
    /// @brief summed pt of tracks in a cone with half-opening angle 0.2 (no zvx cut photons, 1mm electrons) 
    std::vector< float > ptcone20;
    /// @brief summed pt of tracks in a cone with half-opening angle 0.3
    std::vector< float > ptcone30;
    /// @brief summed pt of tracks in a cone with half-opening angle 0.4
    std::vector< float > ptcone40;
    /// @brief number of tracks in a cone with half-opening angle 0.2
    std::vector< float > nucone20;
    /// @brief number of tracks in a cone with half-opening angle 0.3
    std::vector< float > nucone30;
    /// @brief number of tracks in a cone with half-opening angle 0.4
    std::vector< float > nucone40;
    /// @brief pt corrected ET in a cone with half-opening angle 0.15 
    std::vector< float >  etcone15_ptcorrected;
    /// @brief pt corrected ET in a cone with half-opening angle 0.2 
    std::vector< float >  etcone20_ptcorrected;
    /// @brief pt corrected ET in a cone with half-opening angle 0.25 
    std::vector< float >  etcone25_ptcorrected;
    /// @brief pt corrected ET in a cone with half-opening angle 0.3 
    std::vector< float >  etcone30_ptcorrected;
    /// @brief pt corrected ET in a cone with half-opening angle 0.35 
    std::vector< float >  etcone35_ptcorrected;
    /// @brief ptcorrected ET in a cone with half-opening angle 0.4 
    std::vector< float >  etcone40_ptcorrected;
    /// @brief pt + ED corrected ET in a cone with half-opening angle 0.2 
    std::vector< float >  etcone20_corrected;
    /// @brief pt + ED corrected ET in a cone with half-opening angle 0.3 
    std::vector< float >  etcone30_corrected;
    /// @brief pt + ED corrected ET in a cone with half-opening angle 0.4 
    std::vector< float >  etcone40_corrected;
    //et cone for topo cluster 
    /// @brief ET in a cone with half-opening angle 0.20 
    std::vector< float >  topoetcone20;
    /// @brief ET in a cone with half-opening angle 0.30 
    std::vector< float >  topoetcone30;
    /// @brief ET in a cone with half-opening angle 0.40 
    std::vector< float >  topoetcone40;
    /// @brief pt corrected ET in a cone with half-opening angle 0.40 
    std::vector< float >  topoetcone40_ptcorrected;
    /// @brief fully corrected ET in a cone with half-opening angle 0.40 
    std::vector< float >  topoetcone40_corrected;
    /// @}

  }; // class EgammaAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::EgammaAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODEGAMMA_VERSIONS_EGAMMAAUXCONTAINER_V1_H
