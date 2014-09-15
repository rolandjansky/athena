/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EMCONVERSIONBUILDER_H
#define EGAMMATOOLS_EMCONVERSIONBUILDER_H
/**
  @class EMConversionBuilder
          EMConversion data object builder. This is an Algorithm class.
	  Selects conversion vertices reconstructed by the ID software
	  after matching them to an EM calorimeter cluster.
	  For normal two-track conversions the original photon track parameters
	  at the conversion vertex are reconstructed. It is then extrapolated
	  to the calorimeter and checked whether it matches the cluster.
	  For single-track conversions, the reconstructed electron track is
	  extrapolated to the calorimeter.
	  A TrackToCalo extrapolation tool that can handle both neutral and charged
	  track parameters (using a Trk::ParametersBase interface) is used throughout.
	  The extrapolation starts at the last track measurement.
	  Matching is done in both eta and phi. The eta matching WAS disabled for TRT-only
	  tracks due to lack of accurate eta prediction BEFORE 12/2011.
  @author D. Zerwas

  changes: June 16, 2010 (JM) bring some stuff from PhotonRecoveryTool here
           Mar, 2014 (BL) xAOD migration and new logic
*/

// INCLUDE HEADER FILES: 
#include "GaudiKernel/ToolHandle.h"
#include "egammaBaseTool.h"
#include "egammaInterfaces/IEMConversionBuilder.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "xAODTracking/VertexContainer.h"

class IEMExtrapolationTools;

class EMConversionBuilder : public egammaBaseTool, virtual public IEMConversionBuilder
{

 public:

  /** @brief Default constructor*/
  EMConversionBuilder(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /** @brief Destructor*/
  ~EMConversionBuilder();
	
  /** @brief initialize method*/
  StatusCode initialize();
  /** @brief execute method*/
  virtual StatusCode contExecute();
  /** @brief execute method*/
  virtual StatusCode hltExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions);
  /** @brief finalize method*/
  StatusCode finalize();

private:
  
  // configuration:
  /** @brief Name of conversion container*/
  std::string		m_conversionContainerName; 

  /** @brief Name of egammaRec container*/
  std::string		m_egammaRecContainerName; 

  /** @brief Helper used to compute angle differences in phi. Corrects within the +/- pi range.*/
  CaloPhiRange          m_phiHelper; 

  /** @brief EMExtrapolationTools */
  ToolHandle<IEMExtrapolationTools>  m_extrapolationTool;
  
 /** @brief Ignore all conversion vertices that contain exclusively TRT-only tracks */
 bool m_rejectAllTRT;
 /** @brief minimum number of TRT hits for TRT-only tracks (both single and double track conversion vertices) */
 int m_minTRTHits;
 /** @brief minimum pT for Si tracks to be considered single-track conversion vertices */
 double m_minSiSingleTrackPt;
 /** @brief minimum pT for TRT-only tracks considered to be single-track conversion vertices */
 double m_minTRTonlySingleTrackPt;
 /** @brief minimum pT for each tracks in TRT-only double-track conversion vertices */  
 double m_minTRTonlyTrackPt;
 /** @brief minimum sum pT for double track conversion vertices */
 double m_minSumPt;
 /** @brief Preference to vertices with more Si tracks when chosing a conversion vertex */
 int m_preferSi;
 

};

#endif









