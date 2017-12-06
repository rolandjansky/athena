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
#include "egammaInterfaces/IEMExtrapolationTools.h"
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"
#include "StoreGate/ReadHandleKey.h"

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
  StatusCode initialize() override;
  /** @brief execute method*/
  virtual StatusCode executeRec(egammaRec* egRec) override;
  /** @brief execute method*/
  virtual StatusCode hltExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions) override;
  /** @brief execute method*/
  virtual StatusCode vertexExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions) override;
  /** @brief finalize method*/
  StatusCode finalize() override;

private:
  
  /** @brief Return true if vertex and cluster pass Pt and E/p cuts **/
  bool passPtAndEoverP(const xAOD::Vertex&, const xAOD::CaloCluster&) const;
  
  /** @brief Return the maximum fraction of TRT tube hits among the tracks **/
  float getMaxTRTTubeHitFraction(const xAOD::Vertex& vertex) const;
  
  // configuration:
  /** @brief Name of conversion container*/
  SG::ReadHandleKey<xAOD::VertexContainer> m_conversionContainerKey {this,
      "ConversionContainerName", "PhotonConversionVertices",
      "Name of the input conversion container"}; 
  
  /** @brief EMExtrapolationTools */
  ToolHandle<IEMExtrapolationTools>  m_extrapolationTool {this,
      "ExtrapolationTool", "EMExtrapolationTools", 
      "Handle of the extrapolation tool"};
  
  /** @brief Ignore all conversion vertices that contain exclusively TRT-only tracks */
  Gaudi::Property<bool> m_rejectAllTRT {this, "RejectAllTRTConversions", false,
      "Ignore all conversion vertices containing exclusively TRT-only tracks"};
  
  /** @brief minimum number of TRT hits for TRT-only tracks (both single and double track conversion vertices) */
  Gaudi::Property<int> m_minTRTHits {this, "minTRTHits", 0,
      "minimum number of TRT hits for TRT-only tracks (both single and double track conversion vertices)"};

  /** @brief minimum pT for single-track conversion vertices */
  Gaudi::Property<float> m_minPt_singleTrack {this, 
      "minPt_singleTrack", 0*CLHEP::GeV,
      "minimum pT for single-track conversion vertices"};

  /** @brief minimum pT for TRT-only single-track conversion vertices */
  Gaudi::Property<float> m_minPt_singleTRT {this,
      "minPt_singleTRT", 2*CLHEP::GeV,
      "minimum pT for TRT-only single-track conversion vertices"};

  /** @brief minimum pT for each track in TRT-only double-track conversion vertices */  
  Gaudi::Property<float> m_minTRTonlyTrackPt {this,
      "minTRTonlyTrackPt", 0*CLHEP::GeV,
      "minimum pT for each track in TRT-only double-track conversion vertices"};

  /** @brief minimum sum pT for double track conversion vertices */
  Gaudi::Property<float> m_minSumPt_double {this,
      "minSumPt_double", 0*CLHEP::GeV,
      "minimum sum pT for double track conversion vertices"};

  /** @brief minimum sum pT for double TRT track conversion vertices */
  Gaudi::Property<float> m_minSumPt_doubleTRT {this,
      "minSumPt_doubleTRT", 2*CLHEP::GeV,
      "minimum sum pT for double TRT track conversion vertices"}; 

  /** @brief maximum E/p for single track conversion vertices (E is not calibrated) */
  Gaudi::Property<float> m_maxEoverP_singleTrack {this,
      "maxEoverP_singleTrack", 10.,
      "Maximum E/p for single track conversion vertices"};

  /** @brief Scale maxEoverP_singleTrack by 1+sf*Et(cluster)/GeV  **/
  Gaudi::Property<float> m_maxEoverP_singleTrack_EtSf {this,
      "maxEoverP_singleTrack_EtSf", 0.01,  
      "Scale maxEoverP_singleTrack by ( 1+sf*Et(cluster)/GeV )"};

  /** @brief "Maximum fraction of tube hits for vertices with TRT tracks  **/ 
  Gaudi::Property<float> m_maxTRTTubeHitFraction {this,
      "maxTRTTubeHitFraction", 999.,
      "Maximum fraction of tube hits for vertices with TRT tracks"}; 
  // for 21.0.X: minTRTPrecisionFraction cut applied InDetTRT_StandaloneScoringTool
  
};

#endif









