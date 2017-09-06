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
#include "xAODTracking/VertexContainerFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

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
  virtual StatusCode contExecute() { return StatusCode::FAILURE; }
  virtual StatusCode contExecute(EgammaRecContainer& cont);
  /** @brief execute method*/
  virtual StatusCode executeRec(egammaRec* egRec);
  /** @brief execute method*/
  virtual StatusCode hltExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions);
  /** @brief execute method*/
  virtual StatusCode vertexExecute(egammaRec* egRec, const xAOD::VertexContainer* conversions);
  /** @brief finalize method*/
  StatusCode finalize();

private:

  /** @brief Return true if vertex and cluster pass Pt and E/p cuts **/
  bool passPtAndEoverP(const xAOD::Vertex&, const xAOD::CaloCluster&) const;
  
  /** @brief Return the maximum fraction of TRT tube hits among the tracks **/
  float getMaxTRTTubeHitFraction(const xAOD::Vertex& vertex) const;
  
  // configuration:
  /** @brief Name of conversion container*/
  std::string		m_conversionContainerName; 


  /** @brief EMExtrapolationTools */
  ToolHandle<IEMExtrapolationTools>  m_extrapolationTool;
  
 /** @brief Ignore all conversion vertices that contain exclusively TRT-only tracks */
 bool m_rejectAllTRT;
 /** @brief minimum number of TRT hits for TRT-only tracks (both single and double track conversion vertices) */
 int m_minTRTHits;
 /** @brief minimum pT for single-track conversion vertices */
 float m_minPt_singleTrack;
 /** @brief minimum pT for TRT-only single-track conversion vertices */
 float m_minPt_singleTRT;
 /** @brief minimum pT for each track in TRT-only double-track conversion vertices */  
 float m_minTRTonlyTrackPt;
 /** @brief minimum sum pT for double track conversion vertices */
 float m_minSumPt_double;
 /** @brief minimum sum pT for double TRT track conversion vertices */
 float m_minSumPt_doubleTRT; 
 /** @brief maximum E/p for single track conversion vertices (E is not calibrated) */
 float m_maxEoverP_singleTrack;
 /** @brief Scale maxEoverP_singleTrack by 1+sf*Et(cluster)/GeV  **/
 float m_maxEoverP_singleTrack_EtSf;
 /** @brief "Maximum fraction of tube hits for vertices with TRT tracks  **/ 
 float m_maxTRTTubeHitFraction; 

};

#endif









