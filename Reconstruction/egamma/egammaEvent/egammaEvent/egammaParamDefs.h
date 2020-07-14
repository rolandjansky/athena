/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAEVENT_EGAMMAPARAMDEFS_H
#define EGAMMAEVENT_EGAMMAPARAMDEFS_H
/**
  @class egammaParamDefs
     This file defines enums for egammaParameters.
     In particular it defines the enums for algoritm Authors

   @author Kyle Cranmer
*/
/*****************************************************************************
Name    : egammaParamDefs.h
Package : offline/Reconstruction/egammaEvent
Author  : Kyle Cranmer
Created : July 2004
Comment : evolved from PhysicsAnalysis/AnalysisCommon/UserAnalysis/cmt
Note    : 

DESCRIPTION:

This file defines enums for egamma parameters
Updated :
          Mar. 01, 2007 (FD) add pointing quantities
          Oct. 19, 2008 (FD) add forward electrons
          Feb. 12, 2009 (FD) add etringnoisedR03SigN variables
          Feb. 23, 2009 (FD) change etconoise20,30 into 
	                     etconoisedR04Sig2 etconoisedR04Sig2
          May. 31, 2009 (FD/RD) add three variables for track extrapolation
          Feb. 01, 2010 (FD) add transverse impact parameter 
          Nov. 21, 2010 (TCD/JM) add etcone(15,20,25,30,35,40)_ptcorrected quantities
*****************************************************************************/

namespace egammaParameters { 

  /// for the Authors of the egammas 
  /** @brief Default author */
  const unsigned int AuthorUnknown=0x0;
  /** @brief Electron reconstructed by standard cluster-based algorithm */
  const unsigned int AuthorElectron=0x1;
  /** @brief Electron reconstructed by the track-based algorithm */
  const unsigned int AuthorSofte =0x2;
  /** @brief Photon reconstructed by standard cluster-based algorithm */
  const unsigned int AuthorPhoton=0x4;
  /** @brief Electron reconstructed by the Forward cluster-based algorithm */
  const unsigned int AuthorFrwd=0x8;
  /** @brief Photon that is duplicated with electron */
  const unsigned int AuthorRConv=0x10; // old name
  /** @brief trigger Electrons */
  const unsigned int AuthorTrigElectron=0x20;
  /** @brief trigger Photons */
  const unsigned int AuthorTrigPhoton=0x40;
  /** @brief Photon reconstructed by SW CaloTopo35 seeded clusters */
  const unsigned int AuthorCaloTopo35=0x80;

  const double EgParamUndefined = -999.;
  /// Enum for parameter indexes
 /*! \enum Definitons for the egamma parameters */
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // 
    //          NEVER CHANGE THE ORDERING OF THESE!
    //          NEVER DELETE ANY ENTRIES FROM HERE!
    //          ADD NEW VALUES TO THE END ONLY!
    //          Otherwise, you'll break backwards compatibility.
    //
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
  enum ParamDef : unsigned int {
    // common enums    
    /** @brief ET in a cone of R=0.45 in calo, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3*/
    etcone = 0,
    /** @brief ET in a cone with half-opening angle 0.2, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone20 = 1,
    /** @brief ET in a cone with half-opening angle 0.3, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone30 = 2,
    /** @brief ET in a cone with half-opening angle 0.4, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone40 = 3,    
    /** @brief uncorrected energy in presampler in a 1x1 window in cells in eta X phi*/
    e011 = 4, 
    /** @brief uncalibrated energy (sum of cells) in presampler in a 3x3 window in cells in eta X phi*/
    e033 = 5, 
    /** @brief uncalibrated energy (sum of cells) in strips in a 3x2 window in cells in eta X phi*/
    e132 = 6, 
    /** @brief uncalibrated energy (sum of cells) in strips in a 15x2 window in cells in eta X phi*/
    e1152 = 7, 
    /** @brief transverse energy in the first sampling of the hadronic 
  calorimeters behind the cluster calculated from ehad1*/
    ethad1 = 8, 
    /** @brief ET leakage into hadronic calorimeter with exclusion of energy in CaloSampling::TileGap3*/
    ethad = 9, 
    /** @brief E leakage into 1st sampling of had calo (CaloSampling::HEC0 + CaloSampling::TileBar0 + CaloSampling::TileExt0)*/
    ehad1 = 10, 
    /** @brief E1/E = fraction of energy reconstructed in the first sampling, where E1 is energy in all strips belonging to the cluster and E is the total energy reconstructed in the electromagnetic calorimeter cluster */
    f1 = 11,
    /** @brief fraction of energy reconstructed in 3rd sampling*/
    f3 = 12,
    /** @brief E1(3x1)/E = fraction of the energy reconstructed in the first longitudinal compartment of the electromagnetic calorimeter, where E1(3x1) the energy reconstructed in +/-3 strips in eta, centered around the maximum energy strip and E is the energy reconstructed in the electromagnetic calorimeter */
    f1core = 13,
    /** @brief E3(3x3)/E fraction of the energy reconstructed in the third compartment of the electromagnetic calorimeter, where E3(3x3), energy in the back sampling, is the sum of the energy contained in a 3x3 window around the maximum energy cell */ 
    f3core = 14,
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x3 (in cell units eta X phi) */
    e233 = 15,
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x5 */
    e235 = 16, 
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 5x5 */
    e255 = 17, 
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 3x7 */
    e237 = 18,
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7 */
    e277 = 19,
    /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x3 */
    e333 = 20,
    /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x5 */
    e335 = 21,
    /** @brief uncalibrated energy (sum of cells) of the third sampling in a rectangle of size 3x7 */
    e337 = 22,
    /** @brief uncalibrated energy (sum of cells) of the middle sampling in a rectangle of size 7x7 */
    e377 = 23,
    /** @brief shower width using +/-1 strips around the one with the maximal energy deposit: 
  w3 strips = sqrt{sum(Ei)x(i-imax)^2/sum(Ei)}, where i is the number of the strip and imax the strip number of the most energetic one */
    weta1 = 24,
    /** @brief the lateral width is calculated with a window of 3x5 cells using the energy weighted  sum over all cells, which depends on the particle impact point inside the cell: weta2 = 
  sqrt(sum Ei x eta^2)/(sum Ei) -((sum Ei x eta)/(sum Ei))^2, where Ei is the energy of the i-th cell */
    weta2 = 25,  
    /** @brief 2nd max in strips calc by summing 3 strips */
    e2ts1 = 26,
    /** @brief energy of the cell corresponding to second energy maximum in the first sampling */
    e2tsts1 = 27,
    /** @brief shower shape in the shower core : [E(+/-3)-E(+/-1)]/E(+/-1), where E(+/-n) is the energy in +- n strips around the strip with highest energy */
    fracs1 = 28,
    /** @brief same as egammaParameters::weta1 but without corrections 
  on particle impact point inside the cell */
    widths1 = 29,
    /** @brief same as egammaParameters::weta2 but without corrections on particle impact point inside the cell */
    widths2 = 30,
    /** @brief relative position in eta within cell in 1st sampling*/
    poscs1 = 31,
    /** @brief relative position in eta within cell in 2nd sampling*/
    poscs2 = 32,
    /** @brief uncorr asymmetry in 3 strips in the 1st sampling */
    asy1 = 33, 
    /** @brief difference between shower cell and predicted track in +/- 1 cells*/
    pos = 34,
    /** @brief Difference between the track and the shower positions: 
  sum_{i=i_m-7}^{i=i_m+7}E_i x (i-i_m) / sum_{i=i_m-7}^{i=i_m+7}E_i, 
  The difference between the track and the shower positions measured 
  in units of distance between the strips, where i_m is the impact cell 
  for the track reconstructed in the inner detector and E_i is the energy 
  reconstructed in the i-th cell in the eta direction for constant phi given by the track parameters */
    pos7 = 35,
    /** @brief  barycentre in sampling 1 calculated in 3 strips*/
    barys1 = 36,
    /** @brief shower width is determined in a window detaxdphi = 0,0625 ~0,2, corresponding typically to 20 strips in eta : wtot1=sqrt{sum Ei x ( i-imax)^2 / sum Ei}, where i is the strip number and imax the strip number of the first local maximum */
    wtots1 = 37,
    /** @brief energy reconstructed in the strip with the minimal value between the first and second maximum */
    emins1 = 38,
    /** @brief energy of strip with maximal energy deposit*/
    emaxs1 = 39,
    /** @brief 1-ratio of energy in 3x3 over 3x7 cells; 
  E(3x3) = E0(1x1) + E1(3x1) + E2(3x3) + E3(3x3); E(3x7) = E0(3x3) + E1(15x3) + E2(3x7) + E3(3x7) */
    r33over37allcalo = 40,
    /** @brief core energy in em calo 
	E(core) = E0(3x3) + E1(15x2) + E2(5x5) + E3(3x5) */
    ecore = 41,
    /** @brief eta of the track extrapolated to the first sampling (NB : should be equivalent to etaSampling1 */
    EtaCorrMag = 42,
    /** @brief ratio of the cluster energy and the track momentum */
    EoverP = 43,  
    /** @brief difference between the cluster eta (presampler) and 
  the eta of the track extrapolated to the presampler */
    deltaEta0 = 44,
    /** @brief difference between the cluster eta (first sampling) and the eta of the track extrapolated to the first sampling: |eta_stripscluster -eta_ID|, where eta_stripscluster is computed in the first sampling of the electromagnetic calorimeter, where the granularity is very fine, and eta_ID is the pseudo-rapidity of the track extrapolated to the calorimeter */
    deltaEta1 = 45,
    /** @brief difference between the cluster eta (second sampling) and the eta of the track extrapolated to the second sampling */
    deltaEta2 = 46,
    /** @brief difference between the cluster eta (3rd sampling) and 
  the eta of the track extrapolated to the 3rd sampling */
    deltaEta3 = 47,
    /** @brief difference between the cluster phi (presampler) and 
  the eta of the track extrapolated to the presampler */
    deltaPhi0 = 48,
    /** @brief difference between the cluster eta (1st sampling) and 
  the eta of the track extrapolated to the 1st sampling (strips) */
    deltaPhi1 = 49,
    /** @brief difference between the cluster phi (second sampling) and the phi of the track 
  extrapolated to the second sampling : |phi_middlecluster -phi_ID|, where phi_middlecluster 
  is computed in the second compartment of the electromagnetic calorimeter and phi_ID is the 
  azimuth of the track extrapolated to the calorimeter */
    deltaPhi2 = 50,
    /** @brief difference between the cluster eta (3rd sampling) and 
  the eta of the track extrapolated to the 3rd sampling */
    deltaPhi3 = 51,
    /** @brief true if matched track has a hit in first or second pixel layer*/
    isPrimary = 52,
    /** @brief index number in TrackContainer (counting starts at 1 for sake of CBNT)*/
    trackNumber = 53,
    /** @brief number of tracks in isolation window (taken at sampling 2): 
  (# primary tracks if matched track is primary; otherwise all tracks)
  note: overload used to distinguish get/set method */
    tracksInBroadWindow = 54,
    /** @brief true if track was refitted to account for brem effects*/
    trackRefit = 55,
    /** @brief transverse impact parameter (distance of closest approach) after EMBremFit */
    bremD0 = 56,       
    /** @brief azimuth angle of the momentum at the point of closest approach after EMBremFit */
    bremPhi0 = 57,       
    /** @brief 1/pT estimate according to EMBremFit */
    bremInvpT = 58,            
    /** @brief the z value at the point of closest approach after EMBremFit */
    bremZ0 = 59,       
    /** @brief 1/tan(theta) of the track after EMBremFit */
    bremDzDr = 60,       
    /** @brief estimated brem radius from EMBremFit (mm) [x-y plane] */
    bremRadius = 61,       
    /** @brief EMBremFit track impact in 2nd sampling - cluster distance (mm): 
  Delta phi(EMBremFit track impact in 2nd sampling, cluster)*bremClusterRadius */
    bremX = 62,        
    /** @brief error associated to 1/pT by EMBremFit */
    bremInvpTerr = 63,     
    /** @brief number of R hits included in the EMBremFit */
    bremNR = 64,       
    /** @brief number of Z hits included in the EMBremFit */
    bremNZ = 65,       
    /** @brief number of TRT hits included in the EMBremFit */
    bremNTRT = 66,       
    /** @brief cluster radius (mm) [x-y plane] from EMBremFit (#depth) */
    bremClusterRadius = 67,  
    /** @brief Minuit EMBremFit fit status */
    bremFitStatus = 68,    
    /** @brief Flag to tell why the EMBremFit was not called */
    bremErrorFlag = 69,    
    /** @brief chi2 of EMBremFit */
    bremFitChi2 = 70, 
    /** @brief from EMConvert, true if a conversion is matched to the track associated to the cluster */
    convTrackMatch = 71,
    /** @brief from EMConvert, true if a conversion is matched within an angle of 0,05 to the cluster */
    convAngleMatch = 72,
    /** @brief pointing z at vertex reconstructed from the cluster */
    zvertex = 73,     
    /** @brief associated error on zvertex */
    errz = 74,
    /** @brief pointing eta reconstructed from the cluster (first and second sampling) */
    etap = 75,
    /** @brief pointing depth of the shower as calculated in egammaqgcld */
    depth = 76,
    /** @brief ET in a ring of dR<0,40 above 2 sigma from noise*/
    etconoisedR04Sig2 = 77,
    /** @brief ET in a ring of dR0,40 above 3 sigma from noise*/
    etconoisedR04Sig3 = 78,

    

  
    
    /** @brief transverse impact parameter (distance of closest approach) after Track Refit */
    refittedTrack_d0 = 79,
    /** @brief azimuth angle of the momentum at the point of closest approach after Track Refit */
    refittedTrack_phi0 = 80,
    /** @brief 1/pT estimate according to Track Refit */
    refittedTrack_qOverP = 81,
    /** @brief the z value at the point of closest approach after Track Refit */
    refittedTrack_z0 = 82,
    /** @brief theta of the track after EMTrackFit */
    refittedTrack_theta = 83,
    
    /** @brief Refitted track perigee covariance matrix item (\f$ \dTrack0,\dTrack0 \f$) */
    refittedTrack_Covd0d0 = 84,
    /** @brief Refitted track perigee covariance matrix item (\f$ \dTrack_0,\zTrack_0 \f$) */
    refittedTrack_Covd0z0 = 85,
    /** @brief Refitted track perigee covariance matrix item (\f$ \dTrack_0,\phiTrack_0 \f$) */
    refittedTrack_Covd0phi = 86,
    /** @brief Refitted track perigee covariance matrix item (d0,theta) */
    refittedTrack_Covd0theta = 87,
    /** @brief Refitted track perigee covariance matrix item (d0,q/p) */
    refittedTrack_Covd0qOverP = 88,
    /** @brief Refitted track perigee covariance matrix item (z0,z0) */
    refittedTrack_Covz0z0 = 89,
    /** @brief Refitted track perigee covariance matrix item (z0,phi) */
    refittedTrack_Covz0phi = 90,
    /** @brief Refitted track perigee covariance matrix item (z0,theta) */
    refittedTrack_Covz0theta = 91,
    /** @brief Refitted track perigee covariance matrix item (z0,q/p) */
    refittedTrack_Covz0qOverP = 92,
    /** @brief Refitted track perigee covariance matrix item (phi,phi) */
    refittedTrack_Covphiphi = 93,
    /** @brief Refitted track perigee covariance matrix item (phi,theta) */
    refittedTrack_Covphitheta = 94,
    /** @brief Refitted track perigee covariance matrix item (phi,q/P) */
    refittedTrack_CovphiqOverP = 95,
    /** @brief Refitted track perigee covariance matrix item (theta,theta) */
    refittedTrack_Covthetatheta = 96,
    /** @brief Refitted track perigee covariance matrix item (theta,q/P) */
    refittedTrack_CovthetaqOverP = 97,
    /** @brief Refitted track perigee covariance matrix item (q/P,q/P) */
    refittedTrack_CovqOverPqOverP = 98,
      
    /** @brief Last Measurment Surface Local Position 1 after Track Refit */
    refittedTrackLastM_loc1 = 99,
    /** @brief Last Measurment Surface Local Position 2 after Track Refit */
    refittedTrackLastM_loc2 = 100,
    /** @brief Last Measurment Surface azimuth angle after Track Refit */
    refittedTrackLastM_phi = 101,
    /** @brief Last Measurment Surface theta  after Track Refit */
    refittedTrackLastM_theta = 102,
    /** @brief Last Measurment Surface q/pT estimate according to Track Refit */
    refittedTrackLastM_qOverP = 103,
      
    
    /** @brief The Number of brems identified by the track fitter*/
    hasBrem = 104,
    /** @brief Fraction of the measured perigee momentum lost */
    bremDeltaQoverP = 105,
    /** @brief Error on the fraction of energy lost */
    bremDeltaQoverPErr = 106,
    /** @brief The amount of material traversed as seen by the track (in X0) */
    bremMaterialTraversed = 107,
    /** @brief The author of the Refitted track*/
    bremTrackAuthor = 108,

    /** @brief refitted vertex position (cartesian coordinates) */
    refittedVertex_x = 109, 
    refittedVertex_y = 110, 
    refittedVertex_z = 111,
    /** @brief refitted vertex covariance matrix entries (cartesian coordinates) */
    /** @brief refitted vertex covariance item (x,x) */
    refittedVertex_Covxx = 112,  
    /** @brief refitted vertex covariance item (x,y) */
    refittedVertex_Covxy = 113,  
    /** @brief refitted vertex covariance item (x,z) */
    refittedVertex_Covxz= 114,  
    /** @brief refitted vertex covariance item (y,y) */
    refittedVertex_Covyy = 115,
    /** @brief refitted vertex covariance item (y,z) */
    refittedVertex_Covyz = 116,
    /** @brief refitted vertex covariance item (z,z) */
    refittedVertex_Covzz = 117,      
    /** @brief refitted vertex fit quality variables */
    refittedVertex_chi2 = 118,
    refittedVertex_ndf = 119,
    refittedVertex_numTracks = 120,

    /** @brief refitted track global momentum after vertex fit */
    refittedVertex_track1_phi = 121, 
    refittedVertex_track1_theta = 122, 
    refittedVertex_track1_qOverP = 123, 
    /** @brief refitted track chi2 after vertex fit */
    refittedVertex_track1_chi2 = 124,  
    /** @brief refitted track author */
    refittedVertex_track1_author = 125,  

    /** @brief refitted track global momentum after vertex fit */
    refittedVertex_track2_phi = 126, 
    refittedVertex_track2_theta = 127, 
    refittedVertex_track2_qOverP = 128, 
    /** @brief refitted track chi2 after vertex fit */
    refittedVertex_track2_chi2 = 129,  
    /** @brief refitted track author */
    refittedVertex_track2_author = 130,  

    /** @brief refitted track covariance matrix and correlations with vertex position */
    refittedVertex_Covphi1phi1 = 131,
    refittedVertex_Covphi1theta1 = 132,
    refittedVertex_Covphi1qOverP1 = 133,
    refittedVertex_Covtheta1theta1 = 134,
    refittedVertex_Covtheta1qOverP1 = 135,
    refittedVertex_CovqOverP1qOverP1 = 136,

    refittedVertex_Covphi1phi2 = 137,
    refittedVertex_Covphi1theta2 = 138,
    refittedVertex_Covphi1qOverP2 = 139,
    refittedVertex_Covtheta1phi2 = 140,
    refittedVertex_Covtheta1theta2 = 141,
    refittedVertex_Covtheta1qOverP2 = 142,
    refittedVertex_CovqOverP1phi2 = 143,
    refittedVertex_CovqOverP1theta2 = 144,
    refittedVertex_CovqOverP1qOverP2 = 145,

    refittedVertex_Covphi1x = 146,
    refittedVertex_Covphi1y = 147,
    refittedVertex_Covphi1z = 148,
    refittedVertex_Covtheta1x = 149,
    refittedVertex_Covtheta1y = 150,
    refittedVertex_Covtheta1z = 151,
    refittedVertex_CovqOverP1x = 152,
    refittedVertex_CovqOverP1y = 153,
    refittedVertex_CovqOverP1z = 154,

    refittedVertex_Covphi2phi2 = 155,
    refittedVertex_Covphi2theta2 = 156,
    refittedVertex_Covphi2qOverP2 = 157,
    refittedVertex_Covtheta2theta2 = 158,
    refittedVertex_Covtheta2qOverP2 = 159,
    refittedVertex_CovqOverP2qOverP2 = 160,

    refittedVertex_Covphi2x = 161,
    refittedVertex_Covphi2y = 162,
    refittedVertex_Covphi2z = 163,
    refittedVertex_Covtheta2x = 164,
    refittedVertex_Covtheta2y = 165,
    refittedVertex_Covtheta2z = 166,
    refittedVertex_CovqOverP2x = 167,
    refittedVertex_CovqOverP2y = 168,
    refittedVertex_CovqOverP2z = 169,

    /** @brief all energy in a ring : (0.1<DR<0.3) 
	with energy > 2 sigma above noise */
    etringnoisedR03Sig2 = 170,
    /** @brief all energy in a ring : (0.1<DR<0.3) 
	with energy > 3 sigma above noise */
    etringnoisedR03Sig3 = 171,
    /** @brief all energy in a ring : (0.1<DR<0.3) 
	with energy > 4 sigma above noise */
    etringnoisedR03Sig4 = 172,

    refittedVertex_Covd01d01 = 173,
    refittedVertex_Covd01z01 = 174,
    refittedVertex_Covd01phi1 = 175,
    refittedVertex_Covd01theta1 = 176,
    refittedVertex_Covd01qOverP1 = 177,
    refittedVertex_Covz01z01 = 178,
    refittedVertex_Covz01phi1 = 179,
    refittedVertex_Covz01theta1 = 180,
    refittedVertex_Covz01qOverP1 = 181,
    refittedVertex_Covd02d02 = 182,
    refittedVertex_Covd02z02 = 183,
    refittedVertex_Covd02phi2 = 184,
    refittedVertex_Covd02theta2 = 185,
    refittedVertex_Covd02qOverP2 = 186,
    refittedVertex_Covz02z02 = 187,
    refittedVertex_Covz02phi2 = 188,
    refittedVertex_Covz02theta2 = 189,
    refittedVertex_Covz02qOverP2 = 190,

    /** From EMErrorDetail  */

      /** @brief transverse impact parameter (distance of closest approach)  */
    EMConvertedPhoton_d0 = 191,
    /** @brief azimuth angle of the momentum at the point of closest approach */
    EMConvertedPhoton_phi0 = 192,
    /** @brief E estimate  */
    EMConvertedPhoton_Eclus = 193,
    /** @brief the z value of the converted photon */
    EMConvertedPhoton_z0 = 194,
    /** @brief theta of the converted photon */
    EMConvertedPhoton_theta = 195,
    
    /** @brief converted photon covariance matrix item (\f$ \dTrack0,\dTrack0 \f$) */
    EMConvertedPhoton_Covd0d0 = 196,
    /** @brief converted photon  covariance matrix item (\f$ \dTrack_0,\zTrack_0 \f$) */
    EMConvertedPhoton_Covd0z0 = 197,
    /** @brief converted photon  covariance matrix item (\f$ \dTrack_0,\phiTrack_0 \f$) */
    EMConvertedPhoton_Covd0phi = 198,
    /** @brief converted photon covariance matrix item (d0,theta) */
    EMConvertedPhoton_Covd0theta = 199,
    /** @brief converted photon covariance matrix item (d0,E) */
    EMConvertedPhoton_Covd0Eclus = 200,
    /** @brief converted photon  covariance matrix item (z0,z0) */
    EMConvertedPhoton_Covz0z0 = 201,
    /** @brief converted photon  covariance matrix item (z0,phi) */
    EMConvertedPhoton_Covz0phi = 202,
    /** @brief converted photon  covariance matrix item (z0,theta) */
    EMConvertedPhoton_Covz0theta = 203,
    /** @brief converted photon  covariance matrix item (z0,E) */
    EMConvertedPhoton_Covz0Eclus = 204,
    /** @brief converted photon  covariance matrix item (phi,phi) */
    EMConvertedPhoton_Covphiphi = 205,
    /** @brief converted photon  covariance matrix item (phi,theta) */
    EMConvertedPhoton_Covphitheta = 206,
    /** @brief converted photon covariance matrix item (phi,E) */
    EMConvertedPhoton_CovphiEclus = 207,
    /** @brief converted photon  covariance matrix item (theta,theta) */
    EMConvertedPhoton_Covthetatheta = 208,
    /** @brief converted photon covariance matrix item (theta,E) */
    EMConvertedPhoton_CovthetaEclus = 209,
    /** @brief converted photon covariance matrix item (E,E) */
    EMConvertedPhoton_CovEclusEclus = 210,



     /** @brief transverse impact parameter (distance of closest approach)  */
    EMTrack_d0 = 211,
    /** @brief azimuth angle of the momentum at the point of closest approach */
    EMTrack_phi0 = 212,
    /** @brief Cluster energy  */
    EMTrack_Eclus = 213,
    /** @brief the z value at the point of closest approach  */
    EMTrack_z0 = 214,
    /** @brief theta of the track */
    EMTrack_theta = 215,
    
    /** @brief  track perigee covariance matrix item (\f$ \dTrack0,\dTrack0 \f$) */
    EMTrack_Covd0d0 = 216,
    /** @brief  track perigee covariance matrix item (\f$ \dTrack_0,\zTrack_0 \f$) */
    EMTrack_Covd0z0 = 217,
    /** @brief  track perigee covariance matrix item (\f$ \dTrack_0,\phiTrack_0 \f$) */
    EMTrack_Covd0phi = 218,
    /** @brief  track perigee covariance matrix item (d0,theta) */
    EMTrack_Covd0theta = 219,
    /** @brief fitted track perigee covariance matrix item (d0,E) */
    EMTrack_Covd0Eclus = 220,
    /** @brief  track perigee covariance matrix item (z0,z0) */
    EMTrack_Covz0z0 = 221,
    /** @brief  track perigee covariance matrix item (z0,phi) */
    EMTrack_Covz0phi = 222,
    /** @brief  track perigee covariance matrix item (z0,theta) */
    EMTrack_Covz0theta = 223,
    /** @brief  track perigee covariance matrix item (z0,E) */
    EMTrack_Covz0Eclus = 224,
    /** @brief  track perigee covariance matrix item (phi,phi) */
    EMTrack_Covphiphi = 225,
    /** @brief  track perigee covariance matrix item (phi,theta) */
    EMTrack_Covphitheta = 226,
    /** @brief  track perigee covariance matrix item (phi,E) */
    EMTrack_CovphiEclus = 227,
    /** @brief  track perigee covariance matrix item (theta,theta) */
    EMTrack_Covthetatheta = 228,
    /** @brief  track perigee covariance matrix item (theta,E) */
    EMTrack_CovthetaEclus = 229,
    /** @brief  track perigee covariance matrix item (E,E) */
    EMTrack_CovEclusEclus = 230,

    /** @brief photon eta */
    EMPhoton_eta = 231,
    /** @brief photon phi*/
    EMPhoton_phi0 = 232,
    /** @brief photon energy */
    EMPhoton_Eclus = 233,

    /** @brief covariance matrix photon (eta, eta) */
    EMPhoton_Covetaeta = 234,
    /** @brief covariance matrix photon (eta, phi) */
    EMPhoton_Covetaphi = 235,
    /** @brief covariance matrix photon (eta, E) */
    EMPhoton_CovetaEclus = 236,
    /** @brief covariance matrix photon (phi, phi) */
    EMPhoton_Covphiphi = 237,
    /** @brief covariance matrix photon (phi, E) */
    EMPhoton_CovphiEclus = 238,
    /** @brief covariance matrix photon (E, E) */
    EMPhoton_CovEclusEclus = 239,

    /** @brief link index for multiple track and vertex matches */
    linkIndex = 240,

    /** @brief summed pt of tracks in a cone with half-opening angle 0.3*/
    ptcone30 = 241,

    /** @brief the eta of the converted photon */
    EMConvertedPhoton_eta = 242,
    /** @brief momentum of the converted photon */
    EMConvertedPhoton_momentum = 243,
    /** @brief the eta of the track */
    EMTrack_eta = 244,
    /** @brief track momentum */
    EMTrack_momentum = 245,

   /** @brief difference between the cluster phi (sampling 2) and the
       eta of the track extrapolated from the last measurement point.*/
    deltaPhiFromLastMeasurement = 246,

    /** @brief difference between the cluster phi (sampling 2) and the
        phi of the track extrapolated from the perigee with a rescaled
        momentum. */
    deltaPhiRescaled = 247,

    /** @brief eta position at sampling 1 for track extrapolated from
        the last hit.*/
    etaSampling1 = 248,

    /** @brief enum to indicate which track extrapolation method was
        used for track match */
    extrapMethod = 249,

    /** @brief  track combined covariance matrix item (do,d0) */
    EMTrack_Combined_Covd0d0 = 250,
    /** @brief  track combined covariance matrix item (d0,z0) */
    EMTrack_Combined_Covd0z0 = 251,
    /** @brief  track combined covariance matrix item (d0,phi) */
    EMTrack_Combined_Covd0phi = 252,
    /** @brief  track combined covariance matrix item (d0,eta) */
    EMTrack_Combined_Covd0eta = 253,
    /** @brief fitted track combined covariance matrix item (d0,P) */
    EMTrack_Combined_Covd0P = 254,
    /** @brief  track combined covariance matrix item (z0,z0) */
    EMTrack_Combined_Covz0z0 = 255,
    /** @brief  track combined covariance matrix item (z0,phi) */
    EMTrack_Combined_Covz0phi = 256,
    /** @brief  track combined covariance matrix item (z0,eta) */
    EMTrack_Combined_Covz0eta = 257,
    /** @brief  track combined covariance matrix item (z0,P) */
    EMTrack_Combined_Covz0P = 258,
    /** @brief  track combined covariance matrix item (phi,phi) */
    EMTrack_Combined_Covphiphi = 259,
    /** @brief  track combined covariance matrix item (phi,eta) */
    EMTrack_Combined_Covphieta = 260,
    /** @brief  track combined covariance matrix item (phi,P) */
    EMTrack_Combined_CovphiP = 261,
    /** @brief  track combined covariance matrix item (eta,eta) */
    EMTrack_Combined_Covetaeta = 262,
    /** @brief  track combined covariance matrix item (eta,P) */
    EMTrack_Combined_CovetaP = 263,
    /** @brief  track combined covariance matrix item (P,P) */
    EMTrack_Combined_CovPP = 264,

    //delta eta and delta phi for conversions (standard fit and refit) (EMConvert)
    convMatchDeltaEta1 = 265,
    convMatchDeltaPhi1 = 266,
    convMatchDeltaEta2 = 267,
    convMatchDeltaPhi2 = 268,
    convMatchDeltaEta1_refit = 269,
    convMatchDeltaPhi1_refit = 270,
    convMatchDeltaEta2_refit = 271,
    convMatchDeltaPhi2_refit = 272,

    hasSiliconHits = 273,

    /** @brief transverse impact parameter with respect beam spot */
    trackd0_physics  = 274,
    /** @brief transverse impact parameter with respect to primary vertex */
    trackd0_pv  = 275,

    /** @brief eta of the track extrapolated to the presampler */
    EtaExtraps0 = 276,
    /** @brief eta of the track extrapolated to the 1st sampling */
    EtaExtraps1 = 277,
    /** @brief eta of the track extrapolated to the 2nd sampling */
    EtaExtraps2 = 278,
    /** @brief eta of the track extrapolated to the 3rd sampling */
    EtaExtraps3 = 279,
    /** @brief phi of the track extrapolated to the presampler */
    PhiExtraps0 = 280,
    /** @brief phi of the track extrapolated to the 1st sampling */
    PhiExtraps1 = 281,
    /** @brief phi of the track extrapolated to the 2nd sampling */
    PhiExtraps2 = 282,
    /** @brief phi of the track extrapolated to the 3rd sampling */
    PhiExtraps3 = 283,    

    /** @brief rotation of the track in the magnetic field: difference
     * between the phi direction of the track at the perigee and the
     * phi of the track position extrapolated to sampling 2  */
    deltaPhiRot = 284,

    /** @brief ET in a cone with half-opening angle 0.15, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone15 = 285,
    /** @brief ET in a cone with half-opening angle 0.25, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone25 = 286,
    /** @brief ET in a cone with half-opening angle 0.35, with exclusion of a window of size 7x5 in electromagnetic calorimeter and exclude CaloSampling::TileGap3 */
    etcone35 = 287,

    /** @brief expectHitInBLayer (set to 1 if true)*/
    expectHitInBLayer = 288,

    /** @brief PhiRescaled to the presampler */
    PhiRescaled0 = 289,
    /** @brief PhiRescaled to the 1st sampling (strips) */
    PhiRescaled1 = 290,
    /** @brief PhiRescaled to the 2nd sampling */
    PhiRescaled2 = 291,
    /** @brief PhiRescaled to the 3rd sampling */
    PhiRescaled3 = 292,

    /** @brief summed pt of tracks in a cone with half-opening angle 0.2 
	(no zvx cut photons, 1mm electrons) */
    ptcone20 = 293,
    /** @brief summed pt of tracks in a cone with half-opening angle 0.4*/
    ptcone40 = 294,

    /** @brief number of tracks in a cone with half-opening angle 0.2*/
    nucone20 = 295,
    /** @brief number of tracks in a cone with half-opening angle 0.2*/
    nucone30 = 296,
    /** @brief number of tracks in a cone with half-opening angle 0.2*/
    nucone40 = 297,

    /** @brief ambiguity resolultion result, electron/photon */
    ambiguityResult = 298,

    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.15 */
    etcone15_ptcorrected = 299,
    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.20 */
    etcone20_ptcorrected = 300,
    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.25 */
    etcone25_ptcorrected = 301,
    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.30 */
    etcone30_ptcorrected = 302,
    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.35 */
    etcone35_ptcorrected = 303,
    /** @brief pT-leakage corrections applied to ET in a cone with half-opening angle 0.40 */
    etcone40_ptcorrected = 304,    

    /** @brief deltaPhi from the las point */
    deltaPhiLast =305,

    /** @brief Topo cluster ET in a cone with half-opening angle 0.20 */
    topoetcone20= 306,
    /** @brief Topo cluster ET in a cone with half-opening angle 0.30 */
    topoetcone30= 307,
    /** @brief Topo cluster ET in a cone with half-opening angle 0.30 */
    topoetcone40= 308,
    /** @brief Topo cluster pT-leakage corrections applied to ET in a cone with half-opening angle 0.40 */
    topoetcone40_ptcorrected = 309,    
    /** @brief Topo cluster pT-leakage + ED corrections applied to ET in a cone with half-opening angle 0.40 */
    topoetcone40_corrected = 310,    


    /** @brief ET in a ring of dR<20, cell abs(ET) above 2 sigma from noise*/
    etconoisedR02SigAbs2 = 311,
    /** @brief ET in a ring of dR<20 cell abs (ET) above 3 sigma from noise*/
    etconoisedR02SigAbs3 = 312,
 
    /** @brief ET in a ring of dR<30, cell abs(ET) above 2 sigma from noise*/
    etconoisedR03SigAbs2 = 313,
    /** @brief ET in a ring of dR<30 cell abs (ET) above 3 sigma from noise*/
    etconoisedR03SigAbs3 = 314,

    /** @brief ET in a ring of dR<40, cell abs(ET) above 2 sigma from noise*/
    etconoisedR04SigAbs2 = 315,
    /** @brief ET in a ring of dR<40 cell abs (ET) above 3 sigma from noise*/
    etconoisedR04SigAbs3 = 316,

    /** @brief all energy in a ring : (0.1<DR<0.4) 
	with energy > 2 sigma above noise */
    etringnoisedR04Sig2 = 317,
    /** @brief all energy in a ring : (0.1<DR<0.4) 
	with energy > 3 sigma above noise */
    etringnoisedR04Sig3 = 318,

    /** @brief all energy in a ring : (0.1<DR<0.2) 
	with abs(energy) > 2 sigma above noise */
    etringnoisedR02SigAbs2 = 319,
    /** @brief all energy in a ring : (0.1<DR<0.2) 
	with abs(energy) > 3 sigma above noise */
    etringnoisedR02SigAbs3 = 320,

    /** @brief all energy in a ring : (0.1<DR<0.3) 
	with abs(energy) > 2 sigma above noise */
    etringnoisedR03SigAbs2 = 321,
    /** @brief all energy in a ring : (0.1<DR<0.3) 
	with abs(energy) > 3 sigma above noise */
    etringnoisedR03SigAbs3 = 322,

    /** @brief all energy in a ring : (0.1<DR<0.4) 
	with abs(energy) > 2 sigma above noise */
    etringnoisedR04SigAbs2 = 323,
    /** @brief all energy in a ring : (0.1<DR<0.4) 
	with abs(energy) > 3 sigma above noise */
    etringnoisedR04SigAbs3 = 324,
    
    /** @brief pT-leakage + ED corrections applied to ET in a cone with half-opening angle 0.20 */
    etcone20_corrected = 325,
    /** @brief pT-leakage + ED corrections applied to ET in a cone with half-opening angle 0.30 */
    etcone30_corrected = 326,
    /** @brief pT-leakage + ED corrections applied to ET in a cone with half-opening angle 0.40 */
    etcone40_corrected = 327,    

    /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    ptcone20_zpv05 = 328,
    /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    ptcone30_zpv05 = 329,
    /** @brief summed pt of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    ptcone40_zpv05 = 330,
    /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    nucone20_zpv05 = 331,
    /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    nucone30_zpv05 = 332,
    /** @brief number of tracks in a cone with half-opening angle 0.2, |zpv| < 5mm */
    nucone40_zpv05 = 333,

    /** @brief transverse impact parameter with respect beam spot */
    trackd0sigma_physics  = 334,
    /** @brief transverse impact parameter with respect to primary vertex */
    trackd0sigma_pv  = 335,

    // One more than the last code in use.
    // This should be updated when new pameters are added.
    // No other constants should ever be changed!
    lastEgammaParam = 336

  };
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // 
    //          NEVER CHANGE THE ORDERING OF THESE!
    //          NEVER DELETE ANY ENTRIES FROM HERE!
    //          ADD NEW VALUES TO THE END ONLY!
    //          Otherwise, you'll break backwards compatibility.
    //
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 
    // IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT 


   struct ROOT6_NamespaceAutoloadHook{}; 
}
#endif // EGAMMAEVENT_EGAMMAPARAMDEFS_H



